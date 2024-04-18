/**
 * @file    Side.cpp
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
 * Basic utility code for the track splitter.
 */

#include "Side.h"
#include "Utilities.h"
#include "Configuration.h"


/**
 * @section Define Track class.
 *
 */

Track::Track(std::string line)
{
    // Get duration string from the beginning of the line.
    auto pos = line.find_first_of(whitespace);
    if (pos == std::string::npos)
        return;

    std::string duration(line);
    duration.resize(pos);
    seconds = timeStringToSeconds(duration);

    // Get track title from whatever is after duration.
    pos = line.find_first_not_of(whitespace, pos);
    if (pos == std::string::npos)
        return;

    title = line.substr(pos);
}

std::string Track::toString(bool plain, bool csv) const
{
    std::string time{plain ? std::to_string(seconds) : secondsToTimeString(seconds)};

    const std::string c{Configuration::getDivider()};
    std::string s{};
    if (csv)
        s = "Track" + c + time + c + "\"" + title + "\"" + c;
    else
        s = time + " - " + title;

    return s;
}


/**
 * @section Define Side class.
 *
 */

void Side::push(const Track & track)
{
    tracks.push_back(track);
    seconds += track.getValue();
}

void Side::pop()
{
    seconds -= tracks.back().getValue();
    tracks.pop_back();
}

std::string Side::toString(bool plain, bool csv) const
{
    std::string time{plain ? std::to_string(seconds) : secondsToTimeString(seconds)};

    const std::string c{Configuration::getDivider()};
    std::string s{};
    if (csv)
        s = "Side" + c + time + c + "\"" + title + ", " + std::to_string(size()) + " tracks\"" + c;
    else
        s = title + " - " + std::to_string(size()) + " tracks";
    s += '\n';

    for (const auto & track : tracks)
        s += track.toString(plain, csv) + "\n";

    if (!csv)
        s += time + '\n';

    return s;
}

