set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}")
set(incl_dir "${src_dir}/../include")

file(GLOB main_src ${src_dir}/*.cpp)
file(GLOB main_hdr ${src_dir}/*.h)

#add_definitions("-ldl")
add_executable(minimal ${main_src} ${main_hdr})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -ldl")
target_link_libraries(minimal lib ${AUDIO_LIBRARIES})


