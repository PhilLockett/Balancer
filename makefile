# Makefile for Logger unit tests.
objects  = TrackSort.o
objects += Side.o
objects += Opts.o
objects += Configuration.o
objects += Utilities.o
objects += Shuffle.o
objects += Split.o

headers  = TextFile.h
headers += Side.h
headers += Opts.h
headers += Configuration.h
headers += Utilities.h

options = -std=c++20

TrackSort:	$(objects)	$(headers)
	g++ $(options) -o TrackSort $(objects)

%.o:	%.cpp	$(headers)
	g++ $(options) -c -o $@ $<

format:
	tfc -s -u -r TrackSort.cpp
	tfc -s -u -r Side.cpp
	tfc -s -u -r Side.h
	tfc -s -u -r Configuration.cpp
	tfc -s -u -r Configuration.h
	tfc -s -u -r Utilities.cpp
	tfc -s -u -r Utilities.h
	tfc -s -u -r Shuffle.cpp
	tfc -s -u -r Split.cpp

clean:
	rm -f *.exe *.o
