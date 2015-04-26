# 0 Greetings
Welcome to the installation!

**Contents**

  1. Requirements
  2. Installation
  3. Realtime check with stoat
  4. Running
  5. Debugging

# 1 Requirements
You will need the following libraries, headers and tools:
  * at least g++ 4.8 or clang 3.3
  * boost
  * [cmake](http://www.cmake.org/)
  * [jack](https://github.com/jackaudio) (TODO: jack2 in the future?)

There are plugins that might require: # TODO: this is a core requirement yet
  * [zynaddsubfx](http://zynaddsubfx.sourceforge.net/), a version from at least April 2015 (TODO: make optional)
  * [sndfile](http://www.mega-nerd.com/libsndfile/)

# 2 Installation
The initial installation and updating are different processes.

For the initial installation, type in this directory:
```sh
git submodule init
git submodule update

mkdir build
cd build
# for a release build using clang (suggested), where /path/to/zynaddsubfx is
# the binary executable for zynaddsubfx
cmake	-DCOMPILER=clang \
	-DCMAKE_BUILD_TYPE=Release \
	-DZYN_BINARY=/path/to/zynaddsubfx \
	..
```

In order to update the code, go to the main directory and type
```sh
./update
```

# 3 Realtime check with stoat
Make sure [stoat](https://github.com/fundamental/stoat) is (root-)installed on your disk.

Instead of using the cmake code from above, use
```sh
cmake -DCOMPILER=stoat ..
```

Then, run
```sh
make stoat2
```

The output should prompt a total of 0 errors.

# 4 Running
You can try playing a song now. Be careful to set the volume to as loud as it
usually is when you use zynaddsubfx.
```sh
./src/core/minimal ./src/songs/libdemo.so
```

# 5 Debugging
TODO

