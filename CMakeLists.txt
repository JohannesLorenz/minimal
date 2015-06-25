# settings
cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
include(cmake/init_project.txt)

#check_submodule(threadpool "0.8.0")
#check_submodule(libfs "1.0.0")
#check_submodule(zlib "4.0.0")
#check_submodule(libfs "1.0.0")

#print_submodules()

project(minimal)
SET(DESCRIPTION "a minimal osc sequencer")

SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "0")
SET(VERSION_PATCH "1")

# variables
# SET(COMPILER "" CACHE STRING "Compiler to use (allowed: [gcc|clang|stoat])")
SET(ZYN_BINARY "" CACHE STRING "/usr/bin/zynaddsubfx")

# TODO -> src
MESSAGE("Checking for libraries...")

# TODO?!?
find_package(PkgConfig REQUIRED)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED OFF)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.49.0
	REQUIRED
	COMPONENTS graph)

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

# own submodules

macro(find_submodules submodules)
	foreach(submodule ${submodules})
		string(REGEX REPLACE "[^A-Za-z].*" "" fname ${submodule})
		MESSAGE("FNAME: ${fname} vs ${submodule}")

		SET(CUR_FOUND "0")
		SET(CUR_LIBRARIES "")
		SET(CUR_LIBRARY_DIRS "")

		pkg_search_module(CUR ${submodule})
		# MESSAGE(${PC_MODULE_FOUND})
		if(PC_${fname}_FOUND STREQUAL "1")
			list(APPEND AUDIO_LIBRARIES ${CUR_LIBRARIES})
			list(APPEND AUDIO_LIBRARY_DIRS ${CUR_LIBRARY_DIRS})
		else()
			#MESSAGE(STATUS "submodule nf: ${submodule}")
			add_custom_target(${fname}_fetch COMMAND "echo" "FETCHING SUBMODULE")
			list(APPEND COMPILED_SUBMODULES ${fname})
		endif()
	endforeach()
endmacro()

# todo: ask git ;)
#list(APPEND submodules threadpool>=0.8.0 ringbuffer>=0.8.0)
#find_submodules("${submodules}")

#MESSAGE("ALD now: ${AUDIO_LIBRARY_DIRS}")

include_directories("submodules/ringbuffer/include")


link_directories(${AUDIO_LIBRARY_DIRS})

#add_custom_target(init "git" "submodule" "init" "--" ${COMPILED_SUBMODULES})
#add_custom_target(fetch git submodule update)
#add_dependencies(fetch init)

# custom targets
add_custom_target(stoat2 stoat -c
	-w /usr/local/share/stoat/whitelist.txt
	-b /usr/local/share/stoat/blacklist.txt
	-w ${CMAKE_CURRENT_SOURCE_DIR}/data/stoat_suppression_list.txt
	-w ${CMAKE_CURRENT_SOURCE_DIR}/src/include/ringbuffer/data/stoat_suppression_list.txt
	-r ${CMAKE_CURRENT_BINARY_DIR}/src/)

# processing
include(cmake/process_project.txt)

# TODO: make this colored like in zyn
print_summary_base()
MESSAGE(" * Zyn binary (-DZYN_BINARY): ${ZYN_BINARY}")

