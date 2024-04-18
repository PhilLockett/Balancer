/**
 * @file    Side.h
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
 * Class definitions for the track splitter.
 */

#if !defined _SIDE_H_INCLUDED_
#define _SIDE_H_INCLUDED_

#include <string>
#include <vector>


/**
 * @section Define Track class.
 *
 */

class Track
{
public:
    Track(std::string line);

    std::string getTitle() const { return title; }
    size_t getValue() const { return seconds; }

    std::string toString(bool plain=false, bool csv=false) const;

private:
    std::string title;
    size_t seconds;
};


/**
 * @section Define Side class.
 *
 */

class Side
{
public:
    using Iterator = std::vector<Track>::const_iterator;

    Side() : title{}, seconds{} {}

    void setTitle(const std::string & t) { title = t; }
    void push(const Track & track);
    void pop();

    std::string getTitle() const { return title; }
    size_t getValue() const { return seconds; }

    size_t size(void) const { return tracks.size(); }
    Iterator begin(void) { return tracks.begin(); }
    Iterator end(void) { return tracks.end(); }

    std::string toString(bool plain=false, bool csv=false) const;

    void clear() { seconds = 0; tracks.clear(); }

private:
    std::string title;
    size_t seconds;
    std::vector<Track> tracks;

};

#endif //!defined _SIDE_H_INCLUDED_
