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
 * @section Define Side class.
 *
 */

class Side
{
public:
    using CIterator = std::vector<size_t>::const_iterator;
    using Iterator = std::vector<size_t>::iterator;

    Side(void) : title{}, seconds{} {}

    void setTitle(const std::string & t) { title = t; }
    void reserve(size_t len) { tracks.reserve(len); }

    void push(size_t ref);
    void pop(void);

    const std::string & getTitle() const { return title; }
    size_t getValue() const { return seconds; }

    size_t size(void) const { return tracks.size(); }
    CIterator begin(void) const { return tracks.begin(); }
    CIterator end(void) const { return tracks.end(); }
    Iterator begin(void) { return tracks.begin(); }
    Iterator end(void) { return tracks.end(); }

    bool stream(std::ostream & os, bool plain=false, bool csv=false) const;
    bool summary(std::ostream & os, bool plain=false) const;

    void clear(void) { seconds = 0; tracks.clear(); }

    std::vector<size_t> getRefs() const { return tracks; }

private:
    std::string title;
    size_t seconds;
    std::vector<size_t> tracks;

    static bool streamTrack(std::ostream & os, size_t ref, bool plain=false, bool csv=false);

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

    const std::string & getTitle() const { return title; }
    size_t getValue(void) const { return seconds; }

    size_t size(void) const { return sides.size(); }
    Iterator begin(void) const { return sides.begin(); }
    Iterator end(void) const { return sides.end(); }

    bool stream(std::ostream & os, bool plain=false, bool csv=false) const;
    bool summary(std::ostream & os, bool plain=false) const;

    void clear(void) { seconds = 0; for (auto item : sides) item.clear(); sides.clear(); }

    Side & operator[](size_t index) { return sides[index]; }

private:
    std::string title;
    size_t seconds;
    std::vector<Side> sides;

};

#endif //!defined _SIDE_H_INCLUDED_
