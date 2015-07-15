# Rules
This file describes rules and guides for coding in minimal.

## Main thread only
There are certain things that are only allowed by the main thread before or after a cycle:
  * changing the sample rate
  * changing the tempo

## fx graph

## ports
TODO: not

## scheduling

## Reserved words
The following words may not occur in the source:
  * cerr, cout, printf, puts (except in io.h/io.cpp)
  * useconds, seconds (except for types)


## LD_LIBRARY_PATH and PKG_CONFIG_PATH

There are four components:
 * minimal-core
 * libminimal
 * song
 * plugin (loaded by song-library)

? Only the core really needs the LD_LIBRARY_PATH, however, it can be useful to
set it for other components, e.g. to run ldd.

The PKG_CONFIG_PATH is needed by all except the core in order to be built.

song: PKG_CONFIG_PATH: libminimal, plugin
plugin: PKG_CONFIG_PATH: minimal, LD_LIBRARY_PATH: minimal => src/env script



