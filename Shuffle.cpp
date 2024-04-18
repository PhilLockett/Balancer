/**
 * @file    Shuffle.cpp
 * @author  Phil Lockett <phillockett65@gmail.com>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * https://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * 'TrackSort' is a command-line utility for splitting tracks across multiple
 * sides.
 *
 * Basic code for the track shuffling
 */

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "Side.h"
#include "Utilities.h"
#include "Configuration.h"


/**
 * @section Define Indexer class.
 *
 * Indexer provides a cycling index from 0 to 'limit' inclusive, starting from
 * 'first'.  The intention is to provide an even spread when inserting into a
 * 2 dimensional container. The index either increases or decreases depending
 * on whether 'first' is odd or even. 
 */

template<typename T=int>
class Indexer
{
public:
    Indexer(T first, T limit);
    T operator()(void) const { return index; }
    T inc();

private:
    T step, index, start, end;
};

/**
 * @brief Construct a new Indexer< T>:: Indexer object.
 * 
 * @tparam T numeric type to use for index (int by default)
 * @param first index to use.
 * @param limit maximum index.
 */
template<typename T>
Indexer<T>::Indexer(T first, T limit) :
    step{1}, index{(first / 2) % limit}, start{0}, end{limit-1}
{
    if (first & 1)
    {
        step = -1;
        index = limit - 1 - index;
        std::swap(start, end);
    }
}

/**
 * @brief Increment the index.
 * 
 * @tparam T numeric type to use for index (int by default)
 * @return T the new index.
 */
template<typename T>
T Indexer<T>::inc()
{
    if (index == end)
        index = start;
    else
        index += step;

    return index;
}


/**
 * @section Define SideRef class.
 *
 */

class SideRef
{
public:
    SideRef(const std::vector<Track> & trackList) : seconds{}, tracks{trackList} {}

    void push(size_t);
    void pop();

    size_t getValue() const { return seconds; }

    size_t size(void) const { return trackRefs.size(); }
    void clear() { seconds = 0; trackRefs.clear(); }

    std::vector<size_t> getRefs() const { return trackRefs; }

private:
    size_t seconds;
    const std::vector<Track> & tracks;
    std::vector<size_t> trackRefs;

};

void SideRef::push(size_t track)
{
    trackRefs.push_back(track);
    seconds += tracks[track].getValue();
}

void SideRef::pop()
{
    seconds -= tracks[trackRefs.back()].getValue();
    trackRefs.pop_back();
}


/**
 * @section Define Finder class.
 *
 */

class Finder
{
public:
    using Iterator = std::vector<SideRef>::const_iterator;

    Finder(const std::vector<Track> &, const size_t, const size_t, const size_t);

    bool addTracksToSides(void);
    bool isSuccessful(void) const { return success; }
    bool show(std::ostream & os) const;
    std::string trackToString(size_t i, bool plain, bool csv) const;
    std::string sideToString(const std::vector<size_t> &, const std::string &, bool, bool) const;
    bool showAll(std::ostream & os, bool plain=false, bool csv=false) const;

    size_t size(void) const { return sides.size(); }
    Iterator begin(void) { return sides.begin(); }
    Iterator end(void) { return sides.end(); }

private:
    bool look(int track);
    bool snapshot(double latest);

    const size_t duration;
    const size_t sideCount;
    const size_t trackCount;

    bool forward;
    int trackIndex;
    int sideIndex;
    bool success;

    const std::vector<Track> & tracks;
    std::vector<SideRef> sides;

    double dev;
    std::vector<std::vector<size_t>> best;
    Timer timer;
};

Finder::Finder(const std::vector<Track> & trackList, const size_t dur, const size_t tim, const size_t count) :
    duration{dur}, sideCount{count}, trackCount{trackList.size()},
    forward{true}, trackIndex{}, sideIndex{}, success{}, tracks{trackList}, sides{},
    dev{std::numeric_limits<double>::max()}, best{}, timer{tim}
{
    sides.reserve(sideCount);
    best.reserve(sideCount);
    SideRef side{trackList};
    for (int i = 0; i < sideCount; ++i)
        sides.push_back(side);
}


bool Finder::snapshot(double latest)
{
    dev = latest;
    best.clear();
    for (const auto & side : sides)
        best.push_back(side.getRefs());

    return true;
}

bool Finder::look(int trackIndex)
{
    if ((!timer.isWorking()) || (dev < 20.0))
        return true;

    if (trackIndex == trackCount)
    {
        const auto latest{deviation<SideRef>(sides)};
        if (latest < dev)
            snapshot(latest);

        return true;
    }

    Indexer side{trackIndex, (int)sideCount};
    for (int i = 0; i < sideCount; ++i, side.inc())
    {
        auto & trackRef{tracks[trackIndex]};
        auto & sideRef{sides[side()]};
        if (sideRef.getValue() + trackRef.getValue() <= duration)
        {
            sideRef.push(trackIndex);
            look(trackIndex+1);
            sideRef.pop();
        }
    }

    return false;
}

bool Finder::addTracksToSides(void)
{
    timer.start();

    success = look(0);
    success = true;

    timer.terminate();

    return success;
}

bool Finder::show(std::ostream & os) const
{
    os << "deviation " << dev << "\n";
    int i = 0;
    for (const auto & side : best)
    {
        size_t total{};
        for (const auto & track : side)
            total += tracks[track].getValue();
        os << "Side " << std::to_string(++i) << " - " << side.size() << " tracks " << secondsToTimeString(total) << "\n";
    }

    return success;
}

std::string Finder::trackToString(size_t track, bool plain, bool csv) const
{
    const size_t seconds{tracks[track].getValue()};
    std::string time{plain ? std::to_string(seconds) : secondsToTimeString(seconds)};

    const std::string title{tracks[track].getTitle()};
    const std::string c{Configuration::getDivider()};
    std::string s{};
    if (csv)
        s = "Track" + c + time + c + "\"" + title + "\"" + c;
    else
        s = time + " - " + title;

    return s;
}

std::string Finder::sideToString(const std::vector<size_t> & side, const std::string & title, bool plain, bool csv) const
{
    size_t seconds{};
    for (const auto & track : side)
        seconds += tracks[track].getValue();

    std::string time{plain ? std::to_string(seconds) : secondsToTimeString(seconds)};

    const std::string c{Configuration::getDivider()};
    std::string s{};
    if (csv)
        s = "Side" + c + time + c + "\"" + title + ", " + std::to_string(side.size()) + " tracks\"" + c;
    else
        s = title + " - " + std::to_string(side.size()) + " tracks";
    s += '\n';

    for (const auto & track : side)
        s += trackToString(track, plain, csv) + "\n";

    if (!csv)
        s += time + "\n\n";

    return s;
}

bool Finder::showAll(std::ostream & os, bool plain, bool csv) const
{
    size_t index{};
    for (const auto & side : best)
    {
        const std::string title{"Side " + std::to_string(++index)};
        os << sideToString(side, title, plain, csv);
    }

    return success;
}


int shuffleTracksAcrossSides(std::vector<Track> & tracks)
{
    const auto showDebug{Configuration::isDebug()};

    // Sort track list, longest to shortest.
    auto comp = [](const Track & a, const Track & b) { return a.getValue() > b.getValue(); };
    std::sort(tracks.begin(), tracks.end(), comp);

    // Calculate total play time.
    auto lambda = [](size_t a, const Track & b) { return a + b.getValue(); };
    size_t total = std::accumulate(tracks.begin(), tracks.end(), 0, lambda);

    const size_t timeout{Configuration::getTimeout()};  // Get user requested timeout.
    size_t duration{Configuration::getDuration()};      // Get user requested maximum side length.
    const size_t boxes{Configuration::getBoxes()};      // Get user requested number of sides (boxes).

    size_t optimum{};   // The number of sides required.
    size_t length{};    // The minimum side length.

    if (duration)
    {
        // Calculate number of sides required.
        optimum = total / duration;
        if (total % duration)
            optimum++;
        if ((optimum & 1) && (Configuration::isEven()))
            optimum++;

        length = total / optimum;       // Calculate minimum side length.
    }
    else
    {
        optimum = boxes;
        length = total / optimum;       // Calculate minimum side length.

        duration = length + (*tracks.begin()).getValue();
    }

    if (showDebug)
    {
        std::cout << "Total duration " << secondsToTimeString(total) << "\n";
        std::cout << "Required duration " << secondsToTimeString(duration) << "\n";
        std::cout << "Required timeout " << secondsToTimeString(timeout) << "\n";
        std::cout << "Optimum number of sides " << optimum << "\n";
        std::cout << "Minimum side length " << secondsToTimeString(length) << "\n";
    }

    Finder find{tracks, duration, timeout, optimum};
    find.addTracksToSides();
    if (find.isSuccessful())
    {
        if (showDebug)
        {
            std::cout << "Packed sides\n";
            find.show(std::cout);
        }

        const auto csv{Configuration::isCSV()};
        if (!csv)
            std::cout << "\nThe recommended sides are\n";

        find.showAll(std::cout, Configuration::isPlain(), csv);
    }

    return 0;
}
