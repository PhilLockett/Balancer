# TrackSort
`TrackSort` is a command-line utility for splitting 'tracks' across multiple
'sides'.

**USE AT OWN RISK.**

## Overview
This utility reads a text file containing a list of track lengths and titles
and splits them across multiple 'sides' based on the options provided.

A more generic description could be that it reads a list of values with labels
(tracks) and balances these across multiple containers (sides). Use the `-p`
option to display the values as integers.

To use `TrackSort` you will need a C++ compiler and `make` utility installed. 

## Cloning and compiling
To clone and compile this code, execute the following unix/linux commands:

    git clone https://github.com/PhilLockett/TrackSort.git
    cd TrackSort/
    make

## Usage
With `TrackSort` compiled the following command will display the help page:

    $ ./TrackSort -h
    Usage: ./TrackSort [Options]

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
            -p --plain              Display lengths in seconds instead of hh:mm:ss.
            -c --csv                Generate output as comma separated variables.
            -a --divider <char>     Character used to separate csv fields.

### Track list file (mandatory)
The text file containing the track list is specified using `-i` or `--input`.
Each line of the text file has the track length followed by the track title.
The track length can be specified using the hh:mm:ss format, but can also use
mm:ss format or simply as seconds.

### Maximum processing timeout
By default the software takes a maximum of 60 seconds to order the tracks. To
change this timeout limit use `-t` or `--timeout` and specify the number of
seconds. The timeout can also be specified using the hh:mm:ss format, or the
mm:ss format.

### Side length
To specify the maximum length of a side use `-d` or `--duration` and specify
the number of seconds. The duration can also be specified using the hh:mm:ss
format, or the mm:ss format. This option and the `-b` option are mutually
exclusive.

### Even number of sides
If an even number of sides is required use `-e` or `--even`.

### Number of sides
If a specific number of sides is required use `-b` or `--boxes` followed by the
number required. This option and the `-d` option are mutually exclusive.

### Re-ordering tracks
If maintaining the original track order is not necessary use `-s` or
`--shuffle`. This allows the software to shuffle the order of the tracks to
achieve the best balance of time possible. This uses a very different
algorithm and takes considerably longer, so setting `--timeout` may be
necessary to get the best results.

### Disabling the time formatting
If displaying lengths in seconds instead of hh:mm:ss is required use `-p` or
`--plain`. This may be easier to process or is useful if items other than
tracks are to be balanced.

### Comma separated values output
If output as comma separated values is required use `-c` or `--csv`. This is 
useful when output is to be processed by another application. The first column
represents the type and is either the word "Side" or "Track". The second column
is the total length of the side or the length of the track. The third column is
a label indicating the side number or the track title. By default the values
are separated by a comma, but this can be changed using the `-a` or `--divider`
followed by the character to use (which may need to be singularly quoted).

### Example track list
The following track list example shows various ways of representing the length
of a track, however it is not required to mix formats, but it is recommended to
use the same format throughout.

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

## Points of interest
This code has the following points of interest:

  * Uses getopt_long() to help handle command line parameters.
  * The command line parameters are stored in the Configuration class.
  * The Configuration class is implemented as a singleton.
  * The Configuration setters are private so only methods can use them.
  * The Timer class provides a timeout mechanism that can be cancelled.
  * Standard deviation is used to compare side lengths.
  * The standard deviation parameterises both the item type and container type.
