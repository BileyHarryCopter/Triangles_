aux_source_directory(./src SRC_LIST)

add_executable(VKSOURCES ${SRC_LIST})

target_include_directories (VKSOURCES
                            PUBLIC ${GLFW_INCLUDE_DIRS}
                            PUBLIC ${Vulkan_INCLUDE_DIRS}
                            PUBLIC ${CMAKE_THREAD_LIBS_INIT}
)

target_link_libraries (VKSOURCES glfw vulkan dl X11 Xxf86vm Xrandr Xi)
