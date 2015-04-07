set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}")
set(incl_dir "${src_dir}/../include")

add_executable(maths maths.cpp)
target_link_libraries(maths ${AUDIO_LIBRARIES} lib) # TODO: audio libs?

# TODO: comfortable function for that
add_executable(commands commands.cpp)
target_link_libraries(commands ${AUDIO_LIBRARIES} lib)

add_test(maths maths)
add_test(commands commands)

