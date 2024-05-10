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
 * 'Balancer' is a command-line utility for balancing 'tracks' across multiple
 * sides.
 *
 * Basic code for the Balancer.
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

template<typename T=size_t>
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
 * @section Define Finder class.
 *
 */

class Finder
{
public:
    using Iterator = std::vector<Side>::const_iterator;

    Finder(const size_t, const size_t, const size_t);

    bool addTracksToSides(void);
    bool isSuccessful(void) const { return success; }
    bool show(std::ostream & os) const;
    bool showAll(std::ostream & os, bool plain=false, bool csv=false) const;

private:
    bool look(size_t ref);
    bool snapshot(double latest);

    const size_t duration;
    const size_t sideCount;
    const size_t lastTrack;

    bool success;

    Album sides;

    double dev;
    Album best;
    Timer timer;
};

Finder::Finder(const size_t dur, const size_t tim, const size_t count) :
    duration{dur}, sideCount{count}, lastTrack{Configuration::size() - 1},
    success{}, sides{},
    dev{std::numeric_limits<double>::max()}, best{}, timer{tim}
{
    sides.reserve(sideCount);
    best.reserve(sideCount);
    Side side{};
    for (int i = 0; i < sideCount; ++i)
    {
        const std::string title{"Side " + std::to_string(i+1)};
        side.setTitle(title);
        sides.push(side);
    }
}


bool Finder::snapshot(double latest)
{
    dev = latest;
    best.clear();
    best = sides;

    return true;
}

bool Finder::look(size_t ref)
{
    const size_t trackIndex(Configuration::getIndexFromRef(ref));

    if ((!timer.isWorking()) || (dev < 20.0))
        return true;

    Indexer sideIndex{trackIndex, sideCount};
    for (int i{}; i < sideCount; ++i, sideIndex.inc())
    {
        if (sides.getValue(sideIndex()) + Configuration::getValueFromRef(ref) <= duration)
        {
            sides.push(sideIndex(), ref);

            if (trackIndex == lastTrack)
            {
                const auto latest{sides.deviation()};
                if (latest < dev)
                    snapshot(latest);
            }
            else
            {
                look(Configuration::getRef(trackIndex+1));
            }

            sides.pop(sideIndex());
        }
    }

    return false;
}

bool Finder::addTracksToSides(void)
{
    timer.start();

    success = look(Configuration::getRef(0));
    success = true;

    timer.terminate();

    return success;
}

bool Finder::show(std::ostream & os) const
{
    os << "Side deviation " << dev << "\n";
    best.summary(os);

    return success;
}


bool Finder::showAll(std::ostream & os, bool plain, bool csv) const
{
    best.stream(os, plain, csv);

    return success;
}


int shuffleTracksAcrossSides(void)
{
    const auto showDebug{Configuration::isDebug()};

    const size_t total{Configuration::getTotal()};      // Get calculate total play time.
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

        const size_t candidate1{Configuration::getLongest()};
        const size_t candidate2{(total * 11) / (optimum * 10)};
        duration = std::max(candidate1, candidate2);
    }

    if (showDebug)
    {
        streamValues(std::cout, "Required duration", duration);
        std::cout << "Optimum number of sides " << optimum << "\n";
        streamValues(std::cout, "Minimum side length", length);
    }

    Finder find{duration, timeout, optimum};
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
