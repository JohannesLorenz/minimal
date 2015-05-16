# todo: own variable
#list(APPEND AUDIO_LIBRARIES rtosc)
#list(APPEND AUDIO_LIBRARY_DIRS "${CMAKE_CURRENT_BINARY_DIR}/rtosc")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

#include_directories(../rtosc/include)

add_subdirectory(test)
add_subdirectory(include)
add_subdirectory(core)
add_subdirectory(songs)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/minimal.pc.in
	${CMAKE_CURRENT_BINARY_DIR}/minimal.pc @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/minimal.pc"
	DESTINATION "${MINIMAL_LIB_DIR}/pkgconfig")

#cp_script(test.sh)

