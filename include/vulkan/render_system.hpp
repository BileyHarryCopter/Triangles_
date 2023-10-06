#pragma once

#include "device.hpp"
#include "object.hpp"
#include "pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace VKRenderSystem
{

class RenderSystem 
{

    VKDevice::Device&                       device_;

    std::unique_ptr<VKPipeline::Pipeline> pipeline_;
    VkPipelineLayout                pipelineLayout_;

public:
    RenderSystem(VKDevice::Device &device, VkRenderPass renderPass);
    ~RenderSystem();

    RenderSystem(const RenderSystem &) = delete;
    RenderSystem &operator=(const RenderSystem &) = delete;

    void renderObjects(VkCommandBuffer commandBuffer, std::vector<VKObject::Object> &Objects);

private:
    void createPipelineLayout();
    void createPipeline(VkRenderPass renderPass);

};

}  // namespace lve
