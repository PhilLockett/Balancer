# Balancer
`Balancer` is a command-line utility for balancing 'tracks' across multiple 
'sides'.

**USE AT OWN RISK.**

## Overview
This utility reads a text file containing a list of 'track' lengths and titles
and splits them across multiple 'sides' based on the options provided.

Although the original intention for `Balancer` was for arranging a compilation of
'tracks' across multiple 'sides', it can be used more generically.
The required input is a simple list of values and labels.
The value does not have to be a representation of time, it can be just an integer. 
Similarly the output values can also be integers.
`Balancer` can be used to distribute a number of values (with associated labels)
evenly over a number of boxes (sides/containers etc.).
To achieve this, use the `-p` (or `--plain`) option. 

To use `Balancer` you will need a C++ compiler and `make` utility installed. 

## Cloning and Installing
To clone and install this code, execute the following unix/linux commands:

    $ git clone https://github.com/PhilLockett/Balancer.git
    $ cd Balancer/
    $ chmod +x fix.sh
    $ ./fix.sh
    $ ./configure
    $ make
    $ sudo make install

## Usage
With `Balancer` compiled the following command will display the help page:

    $ Balancer -h
    Usage: Balancer [Options]

      Splits a list of tracks across multiple sides of a given length.

      Options:
        -h --help               This help page and nothing else.
        -v --version            Display version.

        -i --input <file>       Input file name containing the track listing.
        -t --timeout <seconds>  The maximum time to spend looking.
        -d --duration <seconds> Maximum length of each side.
        -e --even               Require an even number of sides.
        -b --boxes <count>      Maximum number of containers (sides).
        -s --shuffle            Re-order tracks for optimal fit.
        -f --force              Re-order tracks using brute force method (slow).
        -p --plain              Display lengths in seconds instead of hh:mm:ss.
        -c --csv                Generate output as comma separated variables.
        -a --delimiter <char>   Character used to separate csv fields.

### Track list file (mandatory)
A text file containing the track list is specified using `-i` or `--input`
option, followed by the name of the text file. Each line of the text file has
the track length followed by whitespace followed by the track title. The track
length can be specified using the hh:mm:ss format, the mm:ss format or simply
as seconds.

### Maximum processing timeout
By default the software takes a maximum of 60 seconds to order the tracks. To
change this timeout limit, use `-t` or `--timeout` option and specify the
number of seconds. The timeout can also be specified using the hh:mm:ss format,
or the mm:ss format.

### Side length
To specify the maximum length of a side use `-d` or `--duration` followed by
the number of seconds. The duration can also be specified using the hh:mm:ss
format, or the mm:ss format. If the side length is specified, the number of
sides is calculated from the total length of all the tracks, however, this 
side count can be evened up with the `-e` option.

This option and the `-b` option are mutually exclusive.

### Even number of sides
If the side length is specified and an even number of sides is required, use
`-e` or `--even`.

### Number of sides
If a specific number of sides is required use `-b` or `--boxes` followed by the
number required.

This option and the `-d` option are mutually exclusive.

### Re-ordering tracks
By default the original track order is maintained. 
If this is not necessary, use `-s` or `--shuffle` option. This allows the 
software to shuffle the order of the tracks to achieve the best balance of 
time possible.
This option also attempts to evenly distribute the number of tracks across the 
sides.
This uses a very different algorithm and can take considerably longer, so
setting `--timeout` may be necessary to get the best results.

### Best fit
If achieving the best, most balanced, distribution (time wise) is required, use
the `-f` or `--force` option.
This brute force approach tries every combination of tracks, but does not try 
to evenly distribute the number of tracks across the sides, so some sides may
have considerably more tracks than others.
This algorithm is slow, so setting `--timeout` may be necessary to get the best
results.

### Disabling the time formatting (plain output)
To display lengths as integers, instead of the hh:mm:ss time format, use the 
`-p` or`--plain` option.
This may be easier when parsing the output or is useful if items other than 
tracks are to be balanced.
When plain output is requested, the word `item` is used instead of `track`, 
`box` is used instead of `side` and `value` is used instead of `duration`.

### Comma separated values output
If output as comma separated values is required use `-c` or `--csv`. This is 
useful when output is to be processed by another application. Three columns are
generated. The first column identifies the entry type and is either the word 
"Side" or "Track". 

If the type is "Side", the second column is the length of the side and the third
column is a label indicating the side number and the track count. 

If the type is "Track", the second column is the length of the track and the 
third column is a label indicating the track title. 

Even when plain output is requested, the words "Side" and "Track" are still 
used to simplify output parsers.

By default the values are separated by a comma, but this can be changed using
the `-a` or `--delimiter` option followed by the character to use (which should
be singularly quoted).

### Example track list
The following track list example shows various ways of representing the length
of a track, however it is not required to mix formats, rather it is recommended
to use the same length format throughout. This example shows the different
formats that can be used.

    00:02:00	Sgt. Pepper's Lonely Hearts Club Band
    00:02:42	With a Little Help from My Friends
    03:28	Lucy in the Sky with Diamonds
    02:48	Getting Better
    2:36	Fixing a Hole
    3:25	She's Leaving Home
    157	Being for the Benefit of Mr. Kite!
    305	Within You out You
    157	When I'm Sixty-Four
    00:02:42	Lovely Rita
    00:02:42	Good Morning Good Morning
    1:18	Sgt. Pepper's Lonely Hearts Club Band (Reprise)
    5:38	A Day in the Life

For more examples of input and expected output test files as well as example 
commands, check out the 
[BalancerTest](https://github.com/PhilLockett/BalancerTest) code.

## Uninstalling and cleaning up
To uninstall `Balancer`, 'cd' to the same directory used to run the commands
'./configure', 'make' and 'make install', and run the following command:

    $ sudo make uninstall

Once uninstalled, delete the `Balancer` directory and all it's contents (it can easily
be cloned again if needed).

## Points of interest
This code has the following points of interest:

  * Implemented as an automake project.
  * This is an updated version of [TrackSort](https://github.com/PhilLockett/TrackSort)
  * Uses 'Opts' to help handle command line parameters.
  * The command line parameters are stored in the Configuration class.
  * The Configuration class is implemented as a singleton.
  * The Configuration setters are private so only methods can use them.
  * Textfile.h is used to read the track list file.
  * The Timer class provides a crude timeout mechanism that can be cancelled.
  * Standard deviation is used to compare side lengths.
  * The standard deviation parameterises both the item type and container type.
