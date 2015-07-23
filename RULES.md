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

All components need the PKG_CONFIG_PATH - either for compiling (song, plugin),
or for getting the LD_LIBRARY_PATH before running (minimal-core).

song: PKG_CONFIG_PATH: libminimal, plugin (both for building)
  => LD_LIBRARY_PATH: like PKG
plugin: PKG_CONFIG_PATH: minimal (for building)
  => LD_LIBRARY_PATH: like PKG => src/env script
minimal-core: PKG_CONFIG_PATH: libminimal (LD before running),
  => LD_LIBRARY_PATH: like PKG
libminimal: no paths at all

If a component needs to know the LD library path, you need to specify it once
in CMAKE. Then, it will be known.

The paths that you specify, are, of course, the ones of installed libraries.
It is currently not possible to have different LD paths for builds and
installations.

