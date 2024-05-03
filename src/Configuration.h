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
 * 'Balancer' is a command-line utility for balancing 'tracks' across multiple
 * sides.
 *
 * Common interfaces for the Balancer.
 */

#if !defined _CONFIGURATION_H_INCLUDED_
#define _CONFIGURATION_H_INCLUDED_

#include <string>
#include <filesystem>

#include "Utilities.h"


/**
 * @section Define Item class.
 *
 */

class Item
{
public:
    Item(const std::string & line);

    static size_t merge(size_t index, size_t value) { return index << 32 | value & 0xFFFFFFFF; }
    static size_t sepIndex(size_t ref) { return (ref >> 32) & 0x0FFFFFFF; }
    static size_t sepValue(size_t ref) { return ref & 0xFFFFFFFF; }

    const std::string & getLabel(void) const { return title; }
    size_t getIndex(void) const { return sepIndex(ref); }
    size_t getValue(void) const { return sepValue(ref); }
    size_t getRef(void) const { return ref & 0xEFFFFFFF'FFFFFFFF; }

    void setIndex(size_t index) { ref = merge(index, getRef()); }

    bool streamItem(std::ostream & os) const;

    void setInUse(void) { ref |= 0x10000000'00000000; }
    void clearInUse(void) { ref &= 0xEFFFFFFF'FFFFFFFF; }
    bool isInUse(void) const { return ref & 0x10000000'00000000; }

private:
    std::string title;
    size_t ref;
};


/**
 * @section Balancer configuration data.
 *
 * Implementation of the Balancer comand line configuration Singleton.
 */

class Configuration
{
//- Item list support.
private:
    std::vector<Item> items;
    size_t total;
    double dev;

    static Item & getItem(size_t index) { return instance().items[index]; }

public:
    using Iterator = std::vector<Item>::const_iterator;

    static size_t size(void) { return instance().items.size(); }
    static size_t getTotal(void) { return instance().total; }
    static double getDeviation(void) { return instance().dev; }

    static bool isValidIndex(size_t index) { return index < instance().size(); }

    static const std::string & getLabel(size_t index) { return getItem(index).getLabel(); }
    static size_t getValue(size_t index) { return getItem(index).getValue(); }

    static size_t getRef(size_t index) { return getItem(index).getRef(); }
    static const std::string & getLabelFromRef(size_t ref) { return getLabel(Item::sepIndex(ref)); }
    static size_t getValueFromRef(size_t ref) { return Item::sepValue(ref); }
    static size_t getIndexFromRef(size_t ref) { return Item::sepIndex(ref); }

    static void setInUse(size_t index) { getItem(index).setInUse(); }
    static void clearInUse(size_t index) { getItem(index).clearInUse(); }
    static bool isInUse(size_t index) { return getItem(index).isInUse(); }

    static Iterator begin(void) { return instance().items.begin(); }
    static Iterator end(void) { return instance().items.end(); }

    static bool streamItems(std::ostream & os);
    static bool streamItem(std::ostream & os, size_t index) { return getItem(index).streamItem(os); }


//- Command line parsing support.
private:
//- Hide the default constructor and destructor.
    Configuration(void) : items{},
        name{"Balancer"}, inputFile{}, timeout{60}, seconds{}, even{},
        boxes{}, shuffle{}, force{}, plain{}, csv{}, delimiter{','}, debug{}
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
    bool force;
    bool plain;
    bool csv;
    char delimiter;
    bool debug;

    void setName(std::string value) { name = value; }
    void setInputFile(std::string name) { inputFile = name; }
    void setTimeout(std::string time) { timeout = timeStringToSeconds(time); }
    void setDuration(std::string time) { seconds = timeStringToSeconds(time); }
    void enableEven() { even = true; }
    void setBoxes(std::string count) { boxes = std::stoi(count); }
    void enableShuffle() { shuffle = true; }
    void enableForce() { force = true; }
    void enablePlain() { plain = true; }
    void enableCSV() { csv = true; }
    void setDelimiter(std::string div) { delimiter = div[0]; }
    void enableDebug(void) { debug = true; }

    int help(const std::string & error) const;
    int version(void) const;
    int parseCommandLine(int argc, char *argv[]);
    int setUp(int argc, char *argv[]);
    int loadTracks(void);

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
    static bool isForce(void) { return instance().force; }
    static bool isPlain(void) { return instance().plain; }
    static bool isCSV(void) { return instance().csv; }
    static char getDelimiter(void) { return instance().delimiter; }
    static bool isDebug(void) { return instance().debug; }

    static bool isValid(bool showErrors = false);


};


#endif //!defined _CONFIGURATION_H_INCLUDED_
