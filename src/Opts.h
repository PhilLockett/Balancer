/**
 * @file    Opts.h
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
 * Interface to and implementation of the Opt, Arg and Opts classes.
 */

#if !defined(_OPTS_C_H__20230307_2025__INCLUDED_)
#define _OPTS_C_H__20230307_2025__INCLUDED_

#include <iostream>
#include <vector>
#include <string>
#include <cstring>


class Opt
{
private:
    const int    id;
    const char * name;
    const char * arg;
    const std::string desc;

public:
    Opt(int v, const char * n, const char * a, const std::string & d) : id{v}, name{n}, arg{a}, desc{d}
    {}

    int getIdInt(void) const { return id; }
    char getId(void) const { return (char)id; }

    const char * getName(void) const { return name; }
    const char * getArg(void) const { return arg; }
    const char * getDesc(void) const { return desc.c_str(); }

    std::string getNameString(void) const { return std::string(getName()); }
    std::string getArgString(void) const { return std::string(getArg()); }
    const std::string & getDescString(void) const { return desc; }

    bool isVal(void) const { return isalnum(id); }
    bool isSpacer(void) const { return getIdInt() == 0; }
    bool isName(void) const { return getName() != NULL; }
    bool isArg(void) const { return getArg() != NULL; }
    bool isDesc(void) const { return !getDescString().empty(); }

    int getNameLen(void) const { return strlen(getName()); }
    int getArgLen(void) const { return strlen(getArg()); }
    int getDescLen(void) const { return strlen(getDesc()); }

};

class Arg
{
public:
    using OptsType = std::vector<Opt>;
    using OptIter = OptsType::const_iterator;

private:
    const OptIter iter;
    const char * arg;

public:
    Arg(OptIter i, const char * a) : iter{i}, arg{a}
    {}

    const OptIter getIter(void) const { return iter; }

    /**
     * @brief Get the current short option.
     * 
     * @return int the current option character.
     */
    int getOpt(void) const { return iter->getIdInt(); }
    int getIdInt(void) const { return iter->getIdInt(); }
    char getId(void) const { return iter->getId(); }


    const char * getArgPtr(void) const { return arg; }

    /**
     * @brief Get the argument associated with the current option.
     * 
     * @return std::string the argument.
     */
    const std::string getArg(void) const { return std::string(getArgPtr()); }
    const int getArgInt(void) const { return atoi(getArgPtr()); }
    const float getArgFloat(void) const { return atof(getArgPtr()); }

    /**
     * @brief Indicate if the current option has an argument.
     * 
     * @return true if the option has an argument, false otherwise.
     */
    bool isArg(void) const { return !getArg().empty(); }
    int getArgLen(void) const { return getArg().length(); }

    /**
     * @brief Get the current long option.
     * 
     * @return std::string the long option.
     */
    std::string getLong(void) const { return iter->getNameString(); }

    bool isShort(void) const { return iter->getIdInt(); }
    bool isLong(void) const { return iter->isName(); }
    std::string getArgName(void) const { return iter->getArgString(); }
    std::string getDesc(void) const { return iter->getDescString(); }

};



class Opts
{
public:
    using OptsType = std::vector<Opt>;
    using OptIter = OptsType::const_iterator;

    using ArgsType = std::vector<Arg>;
    using Iterator = ArgsType::const_iterator;

    Iterator begin(void) { return argList.begin(); }
    Iterator end(void) { return argList.end(); }

private:
    const OptsType & options;
    const std::string indent;
    ArgsType argList;

    std::vector<std::string> errorList;

    void display(std::ostream &os) const;

    OptIter find(char v) const;
    OptIter find(const char * v) const;

    static bool isShortOpt(const char * v);
    static bool isLongOpt(const char * v);

    OptIter findShortOpt(const char * v) const;
    OptIter findLongOpt(const char * v) const;

public:
    /**
     * @brief Construct a new Opts object
     * 
     * @param list Vector of Opt entries describing each option.
     * @param indent string displayed at the begining of each line.
     */
    Opts(const std::vector<Opt> & list, const std::string ind)
        : options{list}, indent{ind}, argList{}, errorList{}
    {}

    /**
     * @brief Process the command line parameters.
     * 
     * @param  argc - command line argument count.
     * @param  argv - command line argument vector.
     */
    void process(int argc, char *argv[]);

    bool isErrors(void) const { return !errorList.empty(); }
    std::string firstError(int * index) const;
    std::string nextError(int * index) const;
    void streamErrors(std::ostream &os) const;

    friend std::ostream & operator<<(std::ostream &os, const Opts &A) { A.display(os); return os; }

};


#endif //!defined _OPTS_C_H__20230307_2025__INCLUDED_

