#include "render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace VKRenderSystem {

struct SimplePushConstantData 
{
    glm::mat4 transform{1.f};
    glm::vec2 offset;
    alignas(16) glm::vec3 color;
};

    RenderSystem::RenderSystem(VKDevice::Device &device, VkRenderPass renderPass): device_{device} 
    {
        createPipelineLayout();
        createPipeline(renderPass);
    }

    RenderSystem::~RenderSystem() 
    {
        vkDestroyPipelineLayout(device_.get_logic(), pipelineLayout_, nullptr);
    }

    void RenderSystem::createPipelineLayout() 
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(device_.get_logic(), &pipelineLayoutInfo, nullptr, &pipelineLayout_) !=
            VK_SUCCESS)
            throw std::runtime_error("failed to create pipeline layout!");        
    }

    void RenderSystem::createPipeline(VkRenderPass renderPass) 
    {
        assert(pipelineLayout_ != nullptr && "Cannot create pipeline before pipeline layout");

        VKPipeline::PipelineConfigInfo pipelineConfig{};
        VKPipeline::Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout_;

        pipeline_ = std::make_unique<VKPipeline::Pipeline>(device_, pipelineConfig);
    }

    void RenderSystem::renderObjects(VkCommandBuffer commandBuffer, std::vector<VKObject::Object> &objects)
    {
        pipeline_->bind(commandBuffer);

        for (auto & object : objects)
        {
            object.transform3D_.rotation.y = glm::mod(object.transform3D_.rotation.y + 0.0016f, glm::two_pi<float>());
            object.transform3D_.rotation.x = glm::mod(object.transform3D_.rotation.x + 0.0032f, glm::two_pi<float>());

            SimplePushConstantData                 push_data{};
            push_data.color  =                   object.color_;
            push_data.transform =   object.transform3D_.mat4();

            vkCmdPushConstants (commandBuffer, pipelineLayout_, 
                                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                                0, sizeof(SimplePushConstantData), &push_data); 

            object.model_ -> bind(commandBuffer);
            object.model_ -> draw(commandBuffer);
        }
    }

}  // namespace lve