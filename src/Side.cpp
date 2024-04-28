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
 * @section Define Track support.
 *
 */

bool Side::streamTrack(std::ostream & os, size_t ref, bool plain, bool csv)
{
    const std::string & title{ Configuration::getLabelFromRef(ref) };
    size_t value{ Configuration::getValueFromRef(ref) };

    std::string time{plain ? std::to_string(value) : secondsToTimeString(value)};

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

size_t Side::push(size_t ref)
{
    tracks.push_back(ref);
    const size_t inc{Configuration::getValueFromRef(ref)};
    seconds += inc;

    return inc;
}

void Side::pop(size_t inc)
{
    seconds -= inc;
    tracks.pop_back();
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
        streamTrack(os, track, plain, csv);

    if (!csv)
        os << time << "\n\n";

    return true;
}

bool Side::summary(std::ostream & os, bool plain) const
{
    const std::string time{plain ? std::to_string(seconds) : secondsToTimeString(seconds)};
    os << getTitle() << " - " << size() << " tracks " << time << "\n";

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

void Album::pop(void)
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
    double mean{(double)seconds / sides.size()};
    // std::cout << "mean " << mean << "\n";

    auto lambdaVariance = [mean](double a, const Side & b) { return a + std::pow((mean - b.getValue()), 2); };
    double variance = std::accumulate(sides.begin(), sides.end(), 0.0, lambdaVariance);
    // std::cout << "variance " << variance << "\n";
    variance /= sides.size();
    // std::cout << "variance " << variance << "\n";

    return std::sqrt(variance);
}

bool Album::stream(std::ostream & os, bool plain, bool csv) const
{
    for (const auto & side : sides)
        side.stream(std::cout, plain, csv);

    return true;
}

bool Album::summary(std::ostream & os, bool plain) const
{
    for (const auto & side : sides)
        side.summary(std::cout, plain);

    return true;
}
