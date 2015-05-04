set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}")
set(incl_dir "${src_dir}/../include")

# TODO: comfortable function for that
add_library(demo MODULE demo.cpp)
target_link_libraries(demo lib)

add_library(debug_proj MODULE debug_proj.cpp)
target_link_libraries(debug_proj lib)

