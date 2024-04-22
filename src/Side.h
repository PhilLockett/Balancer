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
 * 'Balancer' is a command-line utility for balancing 'tracks' across multiple
 * sides.
 *
 * Class definitions for the Balancer.
 */

#if !defined _SIDE_H_INCLUDED_
#define _SIDE_H_INCLUDED_

#include <string>
#include <vector>

#include "Configuration.h"


/**
 * @section Define Track class.
 *
 */

class Track
{
public:
    Track(size_t r) : ref{r} {}

    const std::string & getTitle(void) const { return Configuration::getLabelFromRef(ref); }
    size_t getValue(void) const { return Configuration::getValueFromRef(ref); }

    std::string toString(bool plain=false, bool csv=false) const;
    bool stream(std::ostream & os, bool plain=false, bool csv=false) const;

private:
    size_t ref;
};


/**
 * @section Define Side class.
 *
 */

class Side
{
public:
    using CIterator = std::vector<Track>::const_iterator;
    using Iterator = std::vector<Track>::iterator;

    Side(void) : title{}, seconds{} {}

    void setTitle(const std::string & t) { title = t; }
    void reserve(size_t len) { tracks.reserve(len); }

    void push(const Track & track);
    void push(size_t ref);
    void pop(void);

    std::string getTitle() const { return title; }
    size_t getValue() const { return seconds; }

    Track& operator[](std::size_t idx) { return tracks[idx]; }
    const Track& operator[](std::size_t idx) const { return tracks[idx]; }
    size_t size(void) const { return tracks.size(); }
    CIterator begin(void) const { return tracks.begin(); }
    CIterator end(void) const { return tracks.end(); }
    Iterator begin(void) { return tracks.begin(); }
    Iterator end(void) { return tracks.end(); }

    std::string toString(bool plain=false, bool csv=false) const;
    bool stream(std::ostream & os, bool plain=false, bool csv=false) const;

    void clear(void) { seconds = 0; tracks.clear(); }

private:
    std::string title;
    size_t seconds;
    std::vector<Track> tracks;

};


/**
 * @section Define Album class.
 *
 */

class Album
{
public:
    using Iterator = std::vector<Side>::const_iterator;

    Album(void) : title{}, seconds{} {}

    void setTitle(const std::string & t) { title = t; }
    void reserve(size_t len) { sides.reserve(len); }
    void push(const Side & side);
    void pop(void);

    double deviation(void) const;

    std::string getTitle(void) const { return title; }
    size_t getValue(void) const { return seconds; }

    size_t size(void) const { return sides.size(); }
    Iterator begin(void) const { return sides.begin(); }
    Iterator end(void) const { return sides.end(); }

    std::string toString(bool plain=false, bool csv=false) const;
    bool stream(std::ostream & os, bool plain=false, bool csv=false) const;

    void clear(void) { seconds = 0; for (auto item : sides) item.clear(); sides.clear(); }

private:
    std::string title;
    size_t seconds;
    std::vector<Side> sides;

};

#endif //!defined _SIDE_H_INCLUDED_
