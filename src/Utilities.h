/**
 * @file    Utilities.h
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

#if !defined _UTILITIES_H_INCLUDED_
#define _UTILITIES_H_INCLUDED_

// #include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <cmath>
#include <numeric>

class Side;

/**
 * @section basic utility code.
 */

extern const std::string whitespace;
extern const std::string digit;

extern size_t timeStringToSeconds(std::string buffer);
extern std::string secondsToTimeString(size_t seconds, const std::string & sep = ":");


/**
 * @section Define Timer class.
 *
 */

#include <mutex>
#include <future>

class Timer
{
public:
    Timer(size_t init) : working{}, duration{init}, counter{init} {}

    void start(void);
    void terminate(void);

    void set(size_t init) { std::lock_guard<std::mutex> lock(counterMutex); duration = init; counter = init; }
    void reset(void) { std::lock_guard<std::mutex> lock(counterMutex); counter = duration; }
    bool isWorking(void) const { std::lock_guard<std::mutex> lock(counterMutex); return working; }

private:
    void waiter(void);

    bool working;
    size_t duration;
    size_t counter;
    mutable std::mutex counterMutex;
    std::future<void> cyberdyne;

};


#endif //!defined _UTILITIES_H_INCLUDED_
