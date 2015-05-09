# Rules
This file describes rules and guides for coding in minimal.

## Main thread only
There are certain things that are only allowed by the main thread before or after a cycle:
  * changing the sample rate
  * changing the tempo

## Reserved words
The following words may not occur in the source:
  * cerr, cout, printf, puts (except in io.h/io.cpp)
  * useconds, seconds (except for types)

