/**
 * @file    Split.cpp
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
 * @brief Splits a list of tracks across multiple sides using the upper side
 * length limit of 'duration'.
 * 
 * @param duration limit of a side.
 * @return Album list of sides containing the tracks.
 */
static Album addTracksToSides(size_t duration)
{
    // std::cout << "Add tracks to sides\n";
    Album album{};
    Side side{};
    for (const Item & item : Configuration::instance())
    {
        if (side.getValue() + item.getValue() <= duration)
        {
            side.push(item.getValue(), item.getTitle());
        }
        else
        {
            const auto count{album.size() + 1};
            const std::string title{"Side " + std::to_string(count)};
            side.setTitle(title);
            album.push(side);
            side.clear();
            side.push(item.getValue(), item.getTitle());
        }
    }
    if (side.size() != 0)
    {
        const auto count{album.size() + 1};
        const std::string title{"Side " + std::to_string(count)};
        side.setTitle(title);
        album.push(side);
        side.clear();
    }

    return album;
}

/**
 * @brief Determine if the minimum side length is too short by checking if the
 * current number of sides exceeds the required number of sides.
 * 
 * @param required number of sides.
 * @param current number of sides.
 * @return true if the current number of sides exceeds the required.
 * @return false otherwise.
 */
static bool isMinimumTooShort(size_t required, size_t current)
{
    if (required < current)
        return true;

    return false;
}

/**
 * @brief Determine if the maximum side length is too long by checking that the
 * length of the last side is close to the average length of all previous
 * sides. If it isn't, then the previous sides are too greedy.
 * 
 * @param sides currently being considered.
 * @return true if last side doesn't hold enough tracks, false otherwise.
 */
static bool isMaximumTooLong(const Album & album)
{
    const auto count{album.size() - 1};
    if (count <= 0)
        return false;

    // Calculate the standard deviation of all side lengths.
    if (album.deviation() > 10.0)
         return true;

    return false;
}

/**
 * @brief Reads the track list from the user specified file and optimally
 * splits them across multiple sides so that the sides have similar lengths.
 * 
 * @return int error value of 0.
 */
int splitTracksAcrossSides(void)
{
    const auto showDebug{Configuration::isDebug()};

    // Calculate total play time.
    auto lambda = [](size_t a, const Item & b) { return a + b.getValue(); };
    const size_t total = std::accumulate(Configuration::begin(), Configuration::end(), 0, lambda);

    const size_t timeout{Configuration::getTimeout()};  // Get user requested timeout.
    size_t duration{Configuration::getDuration()};      // Get user requested maximum side length.
    const size_t boxes{Configuration::getBoxes()};      // Get user requested number of sides (boxes).

    Album album{};      // The list of sides containing a list of tracks.
    size_t optimum{};   // The number of sides required.
    size_t length{};    // The minimum side length.

    if (duration)
    {
        album = addTracksToSides(duration); // Calculate 'packed' sides -> minimum sides needed.

        // Calculate number of sides required.
        optimum = album.size();
        if ((optimum & 1) && (Configuration::isEven()))
            optimum++;

        length = total / optimum;       // Calculate minimum side length.
    }
    else
    {
        optimum = boxes;
        length = total / optimum;       // Calculate minimum side length.

        auto comp = [](const Item & a, const Item & b) { return a.getValue() < b.getValue(); };
        auto max = std::max_element(Configuration::begin(), Configuration::end(), comp);

        duration = length + (*max).getValue();
    }

    if (showDebug)
    {
        std::cout << "Total duration " << secondsToTimeString(total) << "\n";
        std::cout << "Required timeout " << secondsToTimeString(timeout) << "\n";
        std::cout << "Required duration " << secondsToTimeString(duration) << "\n";
        std::cout << "Required side count " << boxes << "\n";
        std::cout << "Optimum number of sides " << optimum << "\n";
        std::cout << "Minimum side length " << secondsToTimeString(length) << "\n";
    }

    // Home in on optimum side length.
    Timer timer{timeout};
    size_t minimum{length};
    size_t maximum{duration};

    timer.start();
    while (minimum <= maximum)
    {
        size_t median{(minimum + maximum + 1) / 2};
        if (showDebug)
            std::cout << "\nSuggested length " << secondsToTimeString(median) << "\n";

        album.clear();
        album = addTracksToSides(median);

        if (showDebug)
        {
            std::cout << "Suggested sides\n";
            for (const auto & side : album)
                std::cout << side.getTitle() << " - " << side.size() << " tracks " << secondsToTimeString(side.getValue()) << "\n";
        }

        if ((median == minimum) || (median == maximum))
        {
            break;
        }
        else
        if (isMinimumTooShort(optimum, album.size()))
        {
            minimum = median;
            if (showDebug)
            {
                std::cout << "Minimum set to " << secondsToTimeString(minimum) << "\n";
                std::cout << "Maximum is " << secondsToTimeString(maximum) << "\n";
            }
        }
        else
        if (isMaximumTooLong(album))
        {
            maximum = median;
            if (showDebug)
            {
                std::cout << "Minimum is " << secondsToTimeString(minimum) << "\n";
                std::cout << "Maximum set to " << secondsToTimeString(maximum) << "\n";
            }
        }
        else
        {
            break;
        }

        if (!timer.isWorking())
        {
            if (showDebug)
                std::cout << "Abort!!!\n";
            break;
        }
    }
    timer.terminate();

    const auto csv{Configuration::isCSV()};
    if (!csv)
        std::cout << "\nThe recommended sides are\n";

    album.stream(std::cout, Configuration::isPlain(), csv);

    return 0;
}
