/**
 * @file    Opts.cpp
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
 * Implementation of Opts, a class providing simple command line 
 * processing as well as generating a help page options list.
 */

#include <iostream>
#include <algorithm>
#include <iterator>
#include <string_view>

#include "Opts.h"


/**
 * @brief Stream command line option descriptions.
 * 
 * @param os output stream.
 */
void Opts::display(std::ostream &os) const
{
    // Calculate longest preamble.
    size_t longest{};
    for (const Opt & i : options)
    {
        if (i.isSpacer())
            continue;

        int length{1};
        if (i.isName())
            length += i.getNameLen() + 2;

        if (i.isArg())
            length += i.getArgLen() + 3;

        if (length > longest)
            longest = length;
    }

    const std::string padding(longest, ' ');

    // Output options.
    for (const Opt & i : options)
    {
        if (i.isSpacer())
        {
            os << "\n";
            continue;
        }

        if (!i.isDesc())
            continue;

        os << indent;
        if (i.isVal())
            os << "-" << i.getId() << " ";
        else
            os << "   ";

        std::string work{};
        if (i.isName())
            work += "--" + i.getNameString();

        if (i.isArg())
            work += " <" + i.getArgString() + ">";
            
        work += padding;
        const std::string_view view{work.c_str(), longest};

        os << view << i.getDescString() << "\n";
    }
}

Opts::OptIter Opts::find(char v) const
{
    if (v <= 0)
        return std::end(options);

    auto is = [v](const Opt & p) { return p.getId() == v; };

    return std::find_if(std::begin(options), std::end(options), is);
}

Opts::OptIter Opts::find(const char * v) const
{
    auto is = [v](const Opt & p) { return p.isName() ? strcmp(p.getName(), v) == 0 : false; };

    return std::find_if(std::begin(options), std::end(options), is);
}

bool Opts::isShortOpt(const char * v)
{
    if (strlen(v) != 2)
        return false;

    if (v[0] != '-')
        return false;

    if (v[1] == '-')
        return false;

    return true;
}

bool Opts::isLongOpt(const char * v)
{
    if (strlen(v) <= 2)
        return false;

    if (v[0] != '-')
        return false;

    if (v[1] != '-')
        return false;

    return true;
}

Opts::OptIter Opts::findShortOpt(const char * v) const
{
    if (isShortOpt(v))
        return find(v[1]);

    return std::end(options);
}

Opts::OptIter Opts::findLongOpt(const char * v) const
{
    // if (isLongOpt(v))
	// 	std::cout << "isLongOpt(" << v << ")\n";
    if (isLongOpt(v))
        return find(v+2);

    return std::end(options);
}

void Opts::process(int argc, char *argv[])
{
	// std::cout << "Opts::process(" << argc << ")\n";

    argList.clear();
    errorList.clear();

	for (int i{1}; i < argc; ++i)
    {
        const char * arg{argv[i]};
        OptIter iter{findShortOpt(arg)};
        if (iter != std::end(options))
        {
            if (iter->isArg())
            {
                if (i < argc-1)
                    argList.emplace_back(iter, argv[++i]);
                else
                    errorList.push_back(arg);
            }
            else
                argList.emplace_back(iter, "");

            continue;
        }

        iter = findLongOpt(arg);
        if (iter != std::end(options))
        {
            if (iter->isArg())
            {
                if (i < argc-1)
                    argList.emplace_back(iter, argv[++i]);
                else
                    errorList.push_back(arg);
            }
            else
                argList.emplace_back(iter, "");

            continue;
        }

        errorList.push_back(arg);
    }
}

std::string Opts::firstError(int * index) const
{
    if (!isErrors())
    {
        return "";
    }

    *index = 0;

    return errorList.at(0);
}

std::string Opts::nextError(int * index) const
{
    if (!isErrors())
    {
        return "";
    }

    (*index)++;
    if (*index >= errorList.size())
    {
        return "";
    }

    return errorList.at(*index);
}

void Opts::streamErrors(std::ostream &os) const
{
	os << "Unrecognised arguments[" << errorList.size() << "]:\n";
	for (const auto & i : errorList)
		os << "  \"" << i << "\"\n";
	os << "\n";
}
