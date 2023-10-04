aux_source_directory(./src SRC_LIST)

add_executable(VKSOURCES ${SRC_LIST})

target_include_directories (VKSOURCES
                            PUBLIC ${GLFW_INCLUDE_DIRS}
                            PUBLIC ${Vulkan_INCLUDE_DIRS}
                            PUBLIC ${CMAKE_THREAD_LIBS_INIT}
)

target_link_libraries (VKSOURCES glfw vulkan dl X11 Xxf86vm Xrandr Xi)

# a part which necessary for compiling .vert and .frag files
add_custom_command(
    OUTPUT frag.spv
    COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/src/src/shader.frag -o frag.spv
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/src/shader.frag
    VERBATIM)

add_custom_command(
    OUTPUT vert.spv
    COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/src/src/shader.vert -o vert.spv
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/src/shader.vert
    VERBATIM)
