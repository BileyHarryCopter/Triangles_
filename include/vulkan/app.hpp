#pragma once

#include <iostream>

#include "window.hpp"
#include "instance.hpp"
#include "device.hpp"
#include "pipeline.hpp"
#include "swapchain.hpp"
#include "renderer.hpp"

namespace VKEngine
{

class App final
{
    VKWindow::Window        window_;
    VKInstance::Instance    instance_;
    VKDevice::Device        device_;
    VKSwapchain::Swapchain  swapchain_;
    VKPipeline::Pipeline    pipeline_;
    VKRenderer::Renderer    renderer_;

public:
    App() : 
        window_{VKWindow::DEFAULT_WIDTH, 
                VKWindow::DEFAULT_HEIGHT, 
                VKWindow::DEFAULT_WINDOW_NAME},
        instance_{window_}, device_{instance_},
        swapchain_{window_, device_},
        pipeline_{device_, swapchain_}, renderer_ {device_, swapchain_, pipeline_}
    {}
    ~App()= default;

    void run();
    
};

}   //  end of VKEngine class
