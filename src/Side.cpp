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
 * 'Balancer' is a command-line utility for balancing 'tracks' across multiple
 * sides.
 *
 * Basic utility code for the Balancer.
 */

#include <iostream>

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

    const std::string c{Configuration::getDelimiter()};
    std::string s{};
    if (csv)
        s = "Track" + c + time + c + "\"" + title + "\"";
    else
        s = time + " - " + title;

    return s;
}

bool Track::stream(std::ostream & os, bool plain, bool csv) const
{
    std::string time{plain ? std::to_string(seconds) : secondsToTimeString(seconds)};

    const std::string c{Configuration::getDelimiter()};
    if (csv)
        os << "Track" << c << time << c << "\"" << title << "\"";
    else
        os << time << " - " << title;
    os << "\n";

    return true;
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

    const std::string c{Configuration::getDelimiter()};
    std::string s{};
    if (csv)
        s = "Side" + c + time + c + "\"" + title + ", " + std::to_string(size()) + " tracks\"";
    else
        s = title + " - " + std::to_string(size()) + " tracks";
    s += '\n';

    for (const auto & track : tracks)
        s += track.toString(plain, csv) + "\n";

    if (!csv)
        s += time + '\n';

    return s;
}

bool Side::stream(std::ostream & os, bool plain, bool csv) const
{
    std::string time{plain ? std::to_string(seconds) : secondsToTimeString(seconds)};

    const std::string c{Configuration::getDelimiter()};
    if (csv)
        os << "Side" << c << time << c << "\"" << title << ", " << std::to_string(size()) << " tracks\"";
    else
        os << title << " - " << std::to_string(size()) << " tracks";
    os << "\n";

    for (const auto & track : tracks)
        track.stream(os, plain, csv);

    if (!csv)
        os << time << "\n\n";

    return true;
}


/**
 * @section Define Album class.
 *
 */

void Album::push(const Side & side)
{
    sides.push_back(side);
    seconds += side.getValue();
}

void Album::pop()
{
    seconds -= sides.back().getValue();
    sides.pop_back();
}


/**
 * @brief Calculate the standard deviation of the lengths of the list of sides.
 * 
 * @return double the calculated the standard deviation.
 */
double Album::deviation(void) const
{
    // Calculate total play time.
    auto lambdaSum = [](size_t a, const Side & b) { return a + b.getValue(); };
    size_t total = std::accumulate(sides.begin(), sides.end(), 0, lambdaSum);
    // std::cout << "total " << total << "\n";

    double mean{(double)total / sides.size()};
    // std::cout << "mean " << mean << "\n";

    auto lambdaVariance = [mean](double a, const Side & b) { return a + std::pow((mean - b.getValue()), 2); };
    double variance = std::accumulate(sides.begin(), sides.end(), 0.0, lambdaVariance);
    // std::cout << "variance " << variance << "\n";
    variance /= sides.size();
    // std::cout << "variance " << variance << "\n";

    return std::sqrt(variance);
}

std::string Album::toString(bool plain, bool csv) const
{
    std::string s{};
    for (const auto & side : sides)
        s += side.toString(plain, csv) + "\n";

    return s;
}

bool Album::stream(std::ostream & os, bool plain, bool csv) const
{
    for (const auto & side : sides)
        side.stream(std::cout, plain, csv);

    return true;
}
