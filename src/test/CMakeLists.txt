set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}")
set(incl_dir "${src_dir}/../include")

# TODO: comfortable function for that
add_executable(test_str test_str.cpp)
target_link_libraries(test_str ${AUDIO_LIBRARIES} lib)

