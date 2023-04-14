#include "window.hpp"

namespace VKWindow
{
    
    void Window::initWindow()
    {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE,   GLFW_FALSE);

        window_ = glfwCreateWindow(width_, height_, window_name_.data(), nullptr, nullptr);
    }

}   //  end of VKWindow namespace
