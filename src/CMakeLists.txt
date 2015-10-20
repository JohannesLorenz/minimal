# todo: own variable
#list(APPEND AUDIO_LIBRARIES osc)
#list(APPEND AUDIO_LIBRARY_DIRS "${CMAKE_CURRENT_BINARY_DIR}/osc")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

#include_directories(../osc/include)

add_subdirectory(test)
add_subdirectory(include)
add_subdirectory(core)
add_subdirectory(songs)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/minimal.pc.in
	${CMAKE_CURRENT_BINARY_DIR}/minimal.pc @ONLY)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/minimal.pc"
	DESTINATION "${INSTALL_LIB_DIR}/pkgconfig")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/version.h"
		"${CMAKE_CURRENT_BINARY_DIR}/config.h"
		DESTINATION "${INSTALL_INC_DIR}/ringbuffer")
# ^ TODO: into common files

#cp_script(test.sh)

