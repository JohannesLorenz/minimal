# todo: own variable
list(APPEND AUDIO_LIBRARIES rtosc)
list(APPEND AUDIO_LIBRARY_DIRS "${CMAKE_CURRENT_BINARY_DIR}/rtosc")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

include_directories(../rtosc/include)

add_subdirectory(test)
add_subdirectory(include)
add_subdirectory(core)
add_subdirectory(songs)


#add_subdirectory(ca)
#add_subdirectory(io)
#add_subdirectory(rotor)

#if(BUILD_QT_GUI)
#	add_subdirectory(gui_qt)
#endif(BUILD_QT_GUI)

#if(BUILD_USR_DIR)
#	add_subdirectory(usr)
#endif(BUILD_USR_DIR)

#cp_script(test.sh)

#add_subdirectory(tsa)
