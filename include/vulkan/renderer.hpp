#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <memory>

#include "device.hpp"
#include "pipeline.hpp"
#include "swapchain.hpp"

namespace VKRenderer
{

class Renderer final
{

    VKDevice::Device&                          device_;
    VKSwapchain::Swapchain&                 swapchain_;
    VKPipeline::Pipeline&                    pipeline_;
    std::vector<VkCommandBuffer>        commandbuffer_;

public:

    Renderer (VKDevice::Device& device, VKSwapchain::Swapchain& swapchain, VKPipeline::Pipeline& pipeline);
    ~Renderer() {};

    void drawFrame();

private:

    void createCommandBuffers();
    void recordCommandBuffer(VkCommandBuffer commandbuffer, uint32_t imageIndex);
};

}   //  end of the VKRenderer namespace
