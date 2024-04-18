/**
 * @file    Configuration.cpp
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
 * Implementation of the track splitter command line configuration Singleton.
 */

#include <future>
#include <iostream>

#include "Opts.h"
#include "Configuration.h"


/**
 * @section Command line parameter initialisation.
 */

/**
 * Display version message.
 *
 * @param  name - of application.
 */
int Configuration::version(void) const
{
    std::cout << "Version 1.0 of " << name << '\n';

    return 2;
}

const Opts::OptsType optList
{
    { 'h', "help",      NULL,       "This help page and nothing else." },
    { 'v', "version", NULL,         "Display version." },
    { 0,   NULL,        NULL,       "" },
    { 'i', "input",     "file",     "Input file name containing the track listing." },
    { 't', "timeout",   "seconds",  "The maximum time to spend looking." },
    { 'd', "duration",  "seconds",  "Maximum length of each side." },
    { 'e', "even",      NULL,       "Require an even number of sides." },
    { 'b', "boxes",     "count",    "Maximum number of containers (sides)." },
    { 's', "shuffle",   NULL,       "Re-order tracks for optimal fit." },
    { 'p', "plain",     NULL,       "Display lengths in seconds instead of hh:mm:ss." },
    { 'c', "csv",       NULL,       "Generate output as comma separated variables." },
    { 'a', "divider",   "char",     "Character used to separate csv fields." },
    { 'x', NULL,        NULL,       "" },

};
Opts optSet{optList, "    "};

/**
 * @brief Display help message.
 * 
 * @param name of application.
 * @return auto indicate processing should be aborted.
 */
int Configuration::help(const std::string & error) const
{
    std::cout << "Usage: " << name << " [Options]\n";
    std::cout << '\n';
    std::cout << "  Splits a list of tracks across multiple sides of a given length.\n";
    std::cout << '\n';
    std::cout << "  Options:\n";
    std::cout << optSet;

    if (error.empty())
        return 1;

    std::cerr << "\nError: " << error << "\n";

    if (optSet.isErrors())
    {
        std::cerr << "\n";
        optSet.streamErrors(std::cerr);
    }

    return -1;
}


/**
 * @brief Process command line parameters with help from getopt_long() and
 * update global variables.
 * 
 * @param argc command line argument count.
 * @param argv command line argument vector.
 * @return int error value or 0 if no errors.
 */
int Configuration::parseCommandLine(int argc, char *argv[])
{
    setName(argv[0]);
    if (argc < 2)
        return help("valid arguments required.");

    optSet.process(argc, argv);
    if (optSet.isErrors())
        return help("valid arguments required.");

    for (const auto & option : optSet)
    {
        switch (option.getOpt())
        {
        case 'h': return help("");
        case 'v': return version();

        case 'i': setInputFile(option.getArg()); break;
        case 't': setTimeout(option.getArg()); break;
        case 'd': setDuration(option.getArg()); break;
        case 'e': enableEven(); break;
        case 'b': setBoxes(option.getArg()); break;
        case 's': enableShuffle(); break;
        case 'p': enablePlain(); break;
        case 'c': enableCSV(); break;
        case 'a': setDivider(option.getArg()); break;

        case 'x': enableDebug(); break;

        default: return help("internal error.");
        }
    }

    return 0;
}


/**
 * @brief Initialise 'TrackSort' using command line input and ensure we only
 * do it once.
 * 
 * @param argc command line argument count.
 * @param argv command line argument vector.
 * @return int error value or 0 if no errors.
 */
int Configuration::initialise(int argc, char *argv[])
{
    static std::once_flag virgin;
    int ret = 0;

    std::call_once(virgin, [&](){ ret = parseCommandLine(argc, argv); });

    return ret;
}


/**
 * @section track splitter configuration data.
 */

/**
 * @brief Display the configuration defined by the command line parameters.
 * 
 * @param os output stream on which to display.
 */
void Configuration::display(std::ostream &os) const
{
    os << "Config is " << std::string{isValid() ? "" : "NOT "} << "valid\n";
    os << "Input file name:  " << getInputFile() << '\n';
    os << "Timeout: " << getTimeout() << "s\n";
    os << "Disc duration: " << getDuration() << "s\n";
    if (isEven())
        os << "An even number of sides requested.\n";
    os << "Boxes: " << getBoxes() << "\n";
    if (isShuffle())
        os << "Optimal reordering of tracks requested.\n";
    if (isPlain())
        os << "Display lengths in seconds instead of hh:mm:ss.\n";
    if (isCSV())
        os << "Comma separated variable output requested separated by " << getDivider() << ".\n";
}

/**
 * @brief check the validity of the configuration defined by the command line
 * parameters.
 * 
 * @param showErrors if true, send error messages to standard error. 
 * @return true if the configuration is valid.
 * @return false otherwise.
 */
bool Configuration::isValid(bool showErrors)
{
    namespace fs = std::filesystem;

    const auto & inputFile{getInputFile()};

    if (inputFile.string().empty())
    {
        if (showErrors)
            std::cerr << "\nInput file must be specified.\n";

        return false;
    }

    if (!fs::exists(inputFile))
    {
        if (showErrors)
            std::cerr << "\nInput file " << inputFile << " does not exist.\n";

        return false;
    }

    auto duration{getDuration()};
    auto boxes{getBoxes()};
    if (((duration == 0) && (boxes == 0)) || ((duration != 0) && (boxes != 0)))
    {
        if (showErrors)
            std::cerr << "\nEither duration or sides (boxes) must be specified, but not both\n";
        
        return false;
    }

    if ((boxes != 0) && (isEven()))
    {
        if (showErrors)
            std::cerr << "\nNumber of side specified as " << boxes << ", so even flag is ignored.\n";
    }

    return true;
}

