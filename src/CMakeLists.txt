# todo: own variable
#list(APPEND AUDIO_LIBRARIES rtosc)
#list(APPEND AUDIO_LIBRARY_DIRS "${CMAKE_CURRENT_BINARY_DIR}/rtosc")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

#include_directories(../rtosc/include)

add_subdirectory(test)
add_subdirectory(include)
add_subdirectory(core)
add_subdirectory(songs)

#cp_script(test.sh)

