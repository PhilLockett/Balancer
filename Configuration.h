/**
 * @file    Configuration.h
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
 * Common interfaces for the track splitter.
 */

#if !defined _CONFIGURATION_H_INCLUDED_
#define _CONFIGURATION_H_INCLUDED_

#include <string>
#include <filesystem>

#include "Utilities.h"

/**
 * @section track splitter configuration data.
 *
 * Implementation of the track splitter comand line configuration Singleton.
 */

class Configuration
{
private:
//- Hide the default constructor and destructor.
    Configuration(void) : 
        name{"TrackSort"}, inputFile{}, timeout{60}, seconds{}, even{},
        boxes{}, shuffle{}, plain{}, csv{}, divider{','}, debug{}
        {  }
    virtual ~Configuration(void) {}

    void display(std::ostream &os) const;

    std::string name;
    std::filesystem::path inputFile;
    size_t timeout;
    size_t seconds;
    bool even;
    size_t boxes;
    bool shuffle;
    bool plain;
    bool csv;
    char divider;
    bool debug;

    void setName(std::string value) { name = value; }
    void setInputFile(std::string name) { inputFile = name; }
    void setTimeout(std::string time) { timeout = timeStringToSeconds(time); }
    void setDuration(std::string time) { seconds = timeStringToSeconds(time); }
    void enableEven() { even = true; }
    void setBoxes(std::string count) { boxes = std::stoi(count); }
    void enableShuffle() { shuffle = true; }
    void enablePlain() { plain = true; }
    void enableCSV() { csv = true; }
    void setDivider(std::string div) { divider = div[0]; }
    void enableDebug(void) { debug = true; }

    int help(const std::string & error) const;
    int version(void) const;
    int parseCommandLine(int argc, char *argv[]);

public:
//- Delete the copy constructor and assignement operator.
    Configuration(const Configuration &) = delete;
    void operator=(const Configuration &) = delete;

    int initialise(int argc, char *argv[]);
    friend std::ostream & operator<<(std::ostream &os, const Configuration &A) { A.display(os); return os; }

    static Configuration & instance() { static Configuration neo; return neo; }

    static std::string & getName(void) { return instance().name; }
    static std::filesystem::path & getInputFile(void) { return instance().inputFile; }
    static size_t getTimeout(void) { return instance().timeout; }
    static size_t getDuration(void) { return instance().seconds; }
    static bool isEven(void) { return instance().even; }
    static size_t getBoxes(void) { return instance().boxes; }
    static bool isShuffle(void) { return instance().shuffle; }
    static bool isPlain(void) { return instance().plain; }
    static bool isCSV(void) { return instance().csv; }
    static char getDivider(void) { return instance().divider; }
    static bool isDebug(void) { return instance().debug; }

    static bool isValid(bool showErrors = false);

};

#endif //!defined _CONFIGURATION_H_INCLUDED_
