/**
 * @file    Force.cpp
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
 * @section Define Find class.
 *
 */

class Find
{
public:
    using Iterator = std::vector<Side>::const_iterator;

    Find(const size_t, const size_t, const size_t);

    bool addTracksToSides(void);
    bool isSuccessful(void) const { return success; }
    bool show(std::ostream & os) const;
    bool showAll(std::ostream & os, bool plain=false, bool csv=false) const;

private:
    bool look(const int sideIndex = 0, const int trackIndex = 0);
    bool snapshot(double latest);
    void proceed(int sideIndex, int trackIndex);
    void reject(int sideIndex, int trackIndex);

    const size_t duration;
    const size_t sideCount;
    const size_t trackCount;

    size_t usedCount;
    bool success;

    Album sides;

    double dev;
    Album best;
    Timer timer;
};

Find::Find(const size_t dur, const size_t tim, const size_t count) :
    duration{dur}, sideCount{count}, trackCount{Configuration::size()},
    usedCount{}, success{}, sides{},
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

void Find::proceed(int sideIndex, int trackIndex)
{
    ++usedCount;
    sides.push(sideIndex, Configuration::getRef(trackIndex));
    Configuration::setInUse(trackIndex);
}

void Find::reject(int sideIndex, int trackIndex)
{
    --usedCount;
    Configuration::clearInUse(trackIndex);
    sides.pop(sideIndex);
}


bool Find::snapshot(double latest)
{
    dev = latest;
    best.clear();
    best = sides;

    return true;
}

bool Find::look(const int sideIndex, const int trackIndex)
{
    // const size_t trackIndex(Configuration::getIndexFromRef(ref));
    // std::cout << "look(" << sideIndex << ", " << trackIndex << ")\n";

    if ((!timer.isWorking()) || (sideIndex == sideCount))// || (dev < 20.0))
        return true;


    for (int index{trackIndex}; index < trackCount; ++index)
    {
        if (Configuration::isInUse(index))
            continue;

        if (sides.getValue(sideIndex) + Configuration::getValue(index) <= duration)
        {
            proceed(sideIndex, index);

            if (usedCount == trackCount)
            {
                const auto latest{sides.deviation()};
                if (latest < dev)
                    snapshot(latest);
            }
            else
            {
                look(sideIndex, index+1);
            }

            reject(sideIndex, index);
        }
        else
        {
            // sides.stream(std::cout);
            look(sideIndex+1);
        }
    }

    return false;
}

bool Find::addTracksToSides(void)
{
    timer.start();

    success = look();
    success = true;

    timer.terminate();

    return success;
}

bool Find::show(std::ostream & os) const
{
    os << "deviation " << dev << "\n";
    best.summary(os);

    return success;
}


bool Find::showAll(std::ostream & os, bool plain, bool csv) const
{
    best.stream(os, plain, csv);

    return success;
}


int bruteForceTrackArranging(void)
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
        std::cout << "Total duration " << secondsToTimeString(total) << "\n";
        std::cout << "Required duration " << secondsToTimeString(duration) << "\n";
        std::cout << "Required timeout " << secondsToTimeString(timeout) << "\n";
        std::cout << "Optimum number of sides " << optimum << "\n";
        std::cout << "Minimum side length " << secondsToTimeString(length) << "\n";
    }

    Find find{duration, timeout, optimum};
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
