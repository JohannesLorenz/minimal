cmake_minimum_required(VERSION 2.8)

#check_submodule(threadpool "0.8.0")
#check_submodule(libfs "1.0.0")
#check_submodule(zlib "4.0.0")
#check_submodule(libfs "1.0.0")

function(print_submodules submodules)
	foreach(submodule ${submodules})
		MESSAGE(STATUS "submodule: ${submodule}")
	endforeach()
endfunction()


# todo: ask git ;)
list(APPEND submodules threadpool ringbuffer)
print_submodules("${submodules}")

#print_submodules()

# compiler - must be set before the project initialisation
set(WARN_EXC "")

if(COMPILER STREQUAL "gcc")
	# enable these flags if you get tons of errors from boost...
	#set(WARN_EXC "-Wno-unused-local-typedefs -Wno-maybe-uninitialized")
	set(CMAKE_CXX_FLAGS_RELEASE "-flto")
	set(FLTO_FLAGS "-flto")

	SET( CMAKE_CXX_COMPILER "g++" )
	SET( CMAKE_C_COMPILER "gcc" )
elseif(COMPILER STREQUAL "clang")
	set(FLTO_FLAGS "") # not supported by clang on most platforms

	SET( CMAKE_CXX_COMPILER "clang++" )
	SET( CMAKE_C_COMPILER "clang" )
elseif(COMPILER STREQUAL "stoat")
	set(FLTO_FLAGS "") # not supported

	SET( CMAKE_CXX_COMPILER "/usr/local/bin/stoat-compile++" )
	SET( CMAKE_CC_COMPILER "/usr/local/bin/stoat-compile" )
else()
	message(FATAL_ERROR "Please specify -DCOMPILER=[gcc|clang|stoat]")
endif()

# inform libraries to not to reset the compiler
# this would not be clean and also cause a cmake bug
SET(COMPILER_SET "1")

# general things
project(minimal)

SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "0")
SET(VERSION_PATCH "1")
SET(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

MESSAGE("")
MESSAGE("WELCOME")
MESSAGE("This is the Makefile for `${PROJECT_NAME} ${VERSION}'")
MESSAGE("")
MESSAGE("READING CONFIGURATION...")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

# help
# TODO: $myproject from project()
# MESSAGE("This is the Makefile for ${MYPROJECT} (version ${VERSION}).")
# MESSAGE("Options: (-D...=..., cmake -L for details)")
# MESSAGE(" * COMPILER: ${COMPILER} (allowed: [gcc|clang])")

# TODO: move a lot of stuff to src directory

# variables
SET(COMPILER "" CACHE STRING "Compiler to use (allowed: [gcc|clang|stoat])")
SET(ZYN_BINARY "" CACHE STRING "/usr/bin/zynaddsubfx")

# testing
enable_testing ()

# directories
SET(src_base "${CMAKE_CURRENT_SOURCE_DIR}/src/")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/src/") # for config.h
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src/include")

# functions
function(cp_script script_name)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/${script_name}
		${CMAKE_CURRENT_BINARY_DIR}/${script_name} COPYONLY)
endfunction()

# special headers
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/config.h.in
	${CMAKE_CURRENT_BINARY_DIR}/src/config.h)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/src/version.h.in
	${CMAKE_CURRENT_BINARY_DIR}/src/version.h)

# libraries
MESSAGE("CHECKING FOR LIBRARIES...")

# TODO?!?
find_package(PkgConfig REQUIRED)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.49.0
	REQUIRED
	COMPONENTS graph)

#todo: -> rtosc?
#pkg_search_module(LIBLO liblo>=0.25 REQUIRED)
#mark_as_advanced(LIBLO_LIBRARIES)
#include_directories(${LIBLO_INCLUDE_DIRS})
#
#list(APPEND AUDIO_LIBRARIES ${LIBLO_LIBRARIES})
#list(APPEND AUDIO_LIBRARY_DIRS ${LIBLO_LIBRARY_DIRS})

pkg_check_modules(JACK jack)

SET (JackEnable ${JACK_FOUND} CACHE BOOL
	"Enable support for JACK Audio Connection toolKit")

if(JackEnable)
	list(APPEND AUDIO_LIBRARIES ${JACK_LIBRARIES})
	list(APPEND AUDIO_LIBRARY_DIRS ${JACK_LIBRARY_DIRS})
	add_definitions(-DJACK=1)
endif(JackEnable)

pkg_check_modules(SNDFILE sndfile)
list(APPEND AUDIO_LIBRARIES ${SNDFILE_LIBRARIES})
list(APPEND AUDIO_LIBRARY_DIRS ${SNDFILE_LIBRARY_DIRS})

link_directories(${AUDIO_LIBRARY_DIRS})

# compiler details
if(COMPILER STREQUAL "stoat")
	SET(CMAKE_BUILD_TYPE "Debug") # for line detection etc
else()
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
	message(FATAL_ERROR "Please set -DCMAKE_BUILD_TYPE=[Debug|Release]")
endif()
endif()
endif()

# note: the order of -Wall -Wno-unknown-pragmas is important
# TODO: -Werror
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra -Wall ${WARN_EXC} -std=c++11 -pipe")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 ${FLTO_FLAGS}")

# install paths
get_property(LIB64 GLOBAL PROPERTY FIND_LIBRARY_USE_LIB64_PATHS)
if("${LIB64}" STREQUAL "TRUE")
    set(LIBSUFFIX 64)
else()
    set(LIBSUFFIX "")
endif()

set(MINIMAL_LIB_DIR "lib${LIBSUFFIX}" CACHE PATH "Installation directory for libraries")
set(MINIMAL_INC_DIR "include" CACHE PATH "Installation directory for headers")
mark_as_advanced(
	MINIMAL_LIB_DIR
	MINIMAL_INC_DIR
	)

# subdirectories
MESSAGE("READING SUB-DIRECTORIES...")
#add_subdirectory(rtosc)
add_subdirectory(src)
add_subdirectory(doc)

# custom targets
add_custom_target(stoat2 stoat -c
	-w /usr/local/share/stoat/whitelist.txt
	-b /usr/local/share/stoat/blacklist.txt
	-w ${CMAKE_CURRENT_SOURCE_DIR}/data/stoat_suppression_list.txt
	-w ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ringbuffer/data/stoat_suppression_list.txt
	-r ${CMAKE_CURRENT_BINARY_DIR}/src/)

# CPack
SET(MY_NAME_CAP ${PROJECT_NAME})
#SET(MY_NAME_LOW ${PROJECT_NAME}) # (TODO: tolower)
STRING(TOLOWER ${MY_NAME_CAP} MY_NAME_LOW)
#SET(MY_PACKAGE_FILE_NAME "${MY_NAME_LOW}-${VERSION}-${OS}-${MY_HOST_STR}")
SET(MY_PACKAGE_FILE_NAME "${MY_NAME_LOW}-${VERSION}")
SET(MY_SRC_PACKAGE_FILE_NAME "${MY_NAME_LOW}-${VERSION}")

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${MY_NAME_CAP} - a minimal osc sequencer")
IF(WIN32)
SET(CPACK_PACKAGE_EXECUTABLES "${MY_NAME_LOW}.exe")
ELSE(WIN32)
SET(CPACK_PACKAGE_EXECUTABLES "${MY_NAME_LOW}")
SET(CPACK_STRIP_FILES "bin/${MY_NAME_LOW}") # TODO: bin?
ENDIF(WIN32)
SET(CPACK_PACKAGE_FILE_NAME ${MY_PACKAGE_FILE_NAME})
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "${MY_SRC_PACKAGE_FILE_NAME}")
SET(CPACK_PACKAGE_VENDOR "${MY_NAME_CAP} Developers")
SET(CPACK_PACKAGE_VERSION ${VERSION})
SET(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE.txt")
SET(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")
SET(CPACK_SOURCE_GENERATOR "TBZ2")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${MY_SRC_PACKAGE_FILE_NAME}")
INCLUDE(CPack)

# summary
# TODO: make this like in zyn
MESSAGE("")
MESSAGE("SUMMARY:")
MESSAGE(" * Project: ${PROJECT_NAME} ${VERSION}")
MESSAGE(" * Compilers Used: ${COMPILER} (${CMAKE_CXX_COMPILER}, ${CMAKE_CC_COMPILER})")
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	set(MSG_BUILD_TYPE_FLAG ${CMAKE_CXX_FLAGS_DEBUG})
else()
	set(MSG_BUILD_TYPE_FLAG ${CMAKE_CXX_FLAGS_RELEASE})
endif()
MESSAGE(" * Build Type: ${CMAKE_BUILD_TYPE} (${MSG_BUILD_TYPE_FLAG})")
MESSAGE(" * Zyn binary (-DZYN_BINARY): ${ZYN_BINARY}")
MESSAGE(" * Building Doc: No - Type make doc if you want")
MESSAGE(" * Executing Tests: No - Type make test if you want")
MESSAGE("")
MESSAGE("Now, type make and pray...")
MESSAGE("")

