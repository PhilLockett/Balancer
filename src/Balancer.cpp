/**
 * @file    Balancer.cpp
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
 * System entry point.
 *
 * Build using:
 *    g++ -std=c++20 -c -o Balancer.o Balancer.cpp
 *    g++ -std=c++20 -c -o Side.o Side.cpp
 *    g++ -std=c++20 -c -o Configuration.o Configuration.cpp
 *    g++ -std=c++20 -c -o Utilities.o Utilities.cpp
 *    g++ -std=c++20 -c -o Shuffle.o Shuffle.cpp
 *    g++ -std=c++20 -c -o Split.o Split.cpp
 *    g++ -std=c++20 -o Balancer Balancer.o Side.o Configuration.o Utilities.o Shuffle.o Split.o
 *
 * Test using:
 *    ./Balancer -i Tracks.txt -d 19:40
 *
 */

#include <iostream>

#include "Configuration.h"


/**
 * @section System entry point.
 *
 */
extern int shuffleTracksAcrossSides(void);
extern int bruteForceTrackArranging(void);
extern int splitTracksAcrossSides(void);

/**
 * @brief System entry point.
 * 
 * @param argc command line argument count.
 * @param argv command line argument vector.
 * @return int error value or 0 if no errors.
 */
int main(int argc, char *argv[])
{
    const int i{Configuration::instance().initialise(argc, argv)};
    if (i < 0)      // Error?
    {
        return 1;   // Abort with Error!
    }
    else if (i > 0) // No further processing?
    {
        return 0;   // Done!
    }

    if (Configuration::isDebug())
        std::cout << Configuration::instance() << '\n';

//- If all is well, read track list file and generate the output.
    if (Configuration::isShuffle())
    {
        return shuffleTracksAcrossSides();
    }
    if (Configuration::isForce())
    {
        return bruteForceTrackArranging();
    }

    return splitTracksAcrossSides();
}

