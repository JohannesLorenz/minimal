set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}")
set(incl_dir "${src_dir}/../include")

# TODO: comfortable function for that
add_executable(commands commands.cpp)
target_link_libraries(commands ${AUDIO_LIBRARIES} lib)

add_test(commands commands)
