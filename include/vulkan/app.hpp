#pragma once

#include <iostream>

#include "window.hpp"
#include "instance.hpp"
#include "device.hpp"
#include "renderer.hpp"
#include "object.hpp"

namespace VKEngine
{

class App final
{
    VKWindow::Window               window_;
    VKInstance::Instance         instance_;
    VKDevice::Device               device_;
    VKRenderer::Renderer         renderer_;

    std::vector<VKObject::Object> objects_;

public:
    App() : 
        window_{VKWindow::DEFAULT_WIDTH, 
                VKWindow::DEFAULT_HEIGHT, 
                VKWindow::DEFAULT_WINDOW_NAME},
        instance_{window_}, device_{instance_}, renderer_ {window_, device_}
    {
        loadObjects();
    }
    ~App()= default;

    void run();

private:
    void loadObjects();
    
};

}   //  end of VKEngine class
