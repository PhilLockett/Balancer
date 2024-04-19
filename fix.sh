#! /bin/sh
#
# 'git' clones files in alphabetical order. This script corrects the timestamps
# so that 'make' doesn't try to rebuild unnecessarily.
#
# This is a more thorough explanation for why running this script is necessary.
# The 'git clone' command pulls files in alphabetical order and the files
# timestamps reflect this. Even though the timestamps only differ by
# milliseconds, this is still significant to 'make'. The automake file
# 'configure', generates 'make' files which define 'targets' which are
# dependent on 'prerequisites'. If the 'prerequisite' is newer than the
# 'target', 'make' attempts to rebuild the 'target'. In the case of the
# 'targets' listed below, this is unnecessary and requires automake and
# libtools to be installed. This code does not require automake and libtools
# and without them being installed 'make' fails. Executing this script forces
# the 'targets' below to be newer than their 'prerequisites' which stops 'make'
# from attempting to build the 'terget'.
#
# Run 'fix.sh' before 'configure', e.g.:
#
#    ./fix.sh
#    ./configure
#    make
#    make install

chmod +x configure
touch aclocal.m4
touch Makefile.in
touch configure
touch src/Makefile.in
touch src/config.h.in
