set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}")
set(incl_dir "${src_dir}/../include")

file(GLOB main_src ${src_dir}/*.cpp)
file(GLOB main_hdr ${src_dir}/*.h)

check_symbol_exists(RTLD_PRIVATE dlfcn.h HAVE_RTLD_PRIVATE)

add_executable(minimal-bin ${main_src} ${main_hdr})
set_target_properties(minimal-bin
	PROPERTIES OUTPUT_NAME minimal)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -ldl")
target_link_libraries(minimal-bin minimal threadpool ${AUDIO_LIBRARIES})

add_subdirectory(threadpool)

install(TARGETS minimal-bin
	RUNTIME DESTINATION bin)

