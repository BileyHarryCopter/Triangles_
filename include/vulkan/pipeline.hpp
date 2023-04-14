#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

#include "utility.hpp"
#include "device.hpp"
#include "swapchain.hpp"

namespace VKPipeline
{

const std::string VERT_SHADER_FILE_NAME = "../../src/src/shader/vert.spv";
const std::string FRAG_SHADER_FILE_NAME = "../../src/src/shader/frag.spv";

class Pipeline final
{

    VkDevice                   device_ = VK_NULL_HANDLE;
    VkRenderPass           renderpass_ = VK_NULL_HANDLE;

    VkShaderModule   vertshadermodule_ = VK_NULL_HANDLE;
    VkShaderModule   fragshadermodule_ = VK_NULL_HANDLE;

    VkPipelineLayout   pipelinelayout_ = VK_NULL_HANDLE;
    VkPipeline       graphicspipeline_ = VK_NULL_HANDLE;


    void createGraphicsPipeline();

public:

    Pipeline (VKDevice::Device& device, VKSwapchain::Swapchain& swapchain);
    ~Pipeline();

    VkRenderPass get_renderpass() { return renderpass_; }

    void bind(VkCommandBuffer commandBuffer);
};

}   //  end of VKPipeline namespace
