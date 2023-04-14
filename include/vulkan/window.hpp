#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

namespace VKWindow
{

constexpr std::size_t DEFAULT_WIDTH         =  800;
constexpr std::size_t DEFAULT_HEIGHT        =  600;
const     std::string DEFAULT_WINDOW_NAME   = "VK";

class Window final
{

    using size_t     =    std::size_t;
    using str_t      =    std::string;
    using window_ptr =    GLFWwindow*;

    window_ptr                window_;
    str_t                window_name_;
    size_t width_    =              0,
           height_   =              0;

    void initWindow();

public:

    Window (const size_t& width, const size_t height, const str_t& name) :
                              width_{width}, height_{height}, window_name_{name}
    {
        initWindow();
    }
    ~Window()
    {
        glfwDestroyWindow(window_);
        glfwTerminate();
    }
    //  To avoid double call of glfwDestroyWindow()
    Window (const Window& window) = delete;
    Window &operator=(const Window &window) = delete;

    bool shouldClose()  { return glfwWindowShouldClose(window_); }
    window_ptr get()    { return window_; };
};

}   //  end of VKWindow class
