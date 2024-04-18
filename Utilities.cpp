/**
 * @file    Utilities.cpp
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

#include <sstream>
#include <vector>
#include <thread>

#include "Side.h"
#include "Utilities.h"
#include "TextFile.h"

/**
 * @section basic utility code.
 */

const std::string whitespace{" \t"};
const std::string digit{"0123456789"};

/**
 * @brief Break a time string (H:M:S) down to get total number of seconds.
 * Also handles M:S and S formats.
 * 
 * @param buffer time string to parse.
 * @return size_t the equivalent number of seconds.
 */
size_t timeStringToSeconds(std::string buffer)
{
    size_t digits{};
    size_t seconds{};
    size_t pos{};

    for (int i = 0; i < 3; ++i)
    {
        buffer = buffer.substr(digits);
        pos = buffer.find_first_of(digit);
        if (pos == std::string::npos)
            break;

        seconds *= 60;
        buffer = buffer.substr(pos);
        seconds += std::stoi(buffer, &digits);
    }

    return seconds;
}


/**
 * @brief Generates a time string in the form H:M:S from the given seconds.
 * 
 * @param seconds number of seconds to represent.
 * @param sep optional seperator or ':' if none specified.
 * @return std::string time string in the form H:M:S.
 */
std::string secondsToTimeString(size_t seconds, const std::string & sep)
{
    std::ostringstream ss;

    size_t hours{seconds / 3600};
    seconds -= hours * 3600;

    size_t minutes{seconds / 60};
    seconds -= minutes * 60;

    ss.width(2);
    ss.fill('0');
    ss << hours << sep;
    ss.width(2);
    ss << minutes << sep;
    ss.width(2);
    ss << seconds;

    return ss.str();
}

/**
 * @brief Builds a vector of Tracks from the input file.
 * 
 * @param inputFile Name of input file.
 * @return std::vector<Track> input file represented as a list of Tracks
 */
std::vector<Track> buildTrackListFromInputFile(const std::filesystem::path & inputFile)
{
    TextFile input{inputFile};
    input.read();

    std::vector<Track> tracks{};

    for (const auto & line : input)
        tracks.emplace_back(line);

    return tracks;
}


/**
 * @section Define Timer class.
 *
 */

void Timer::waiter(void)
{
    using namespace std::literals::chrono_literals;

    while (true)
    {
        std::this_thread::sleep_for(1s);

        std::lock_guard<std::mutex> lock(counterMutex);
        if (!working)
        {
            // std::cout << "Aborting!\n";
            break;
        }

        if (--counter <= 0)
        {
            // std::cout << "Done waiting\n";
            working = false;
            break;
        }

        // std::cout << counter << "\n";
    }
}

void Timer::start(void)
{
    // std::cout << "starting\n";
    {
        std::lock_guard<std::mutex> lock(counterMutex);

        // std::cout << "Go!\n";
        counter = duration;
        working = true;
    }
    cyberdyne = std::async(std::launch::async, &Timer::waiter, this);
    // std::cout << "Done starting\n";
}

void Timer::terminate(void)
{
    // std::cout << "terminating\n";
    std::lock_guard<std::mutex> lock(counterMutex);
    working = false;
    counter = 1;

    using namespace std::literals::chrono_literals;
    if (cyberdyne.valid())
    {
        // std::cout << "waiting for thread\n\n\n";
        cyberdyne.wait_for(1s);
    }
}
