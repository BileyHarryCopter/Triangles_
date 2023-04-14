#include "pipeline.hpp"

namespace VKPipeline
{

    Pipeline::Pipeline(VKDevice::Device& device, VKSwapchain::Swapchain& swapchain) 
                        : device_{device.get_logic()}, renderpass_{swapchain.get_renderpass()}
    {
        createGraphicsPipeline();
    }

    Pipeline::~Pipeline()
    {
        vkDestroyShaderModule  (device_, fragshadermodule_, nullptr);
        vkDestroyShaderModule  (device_, vertshadermodule_, nullptr);
        vkDestroyPipeline      (device_, graphicspipeline_, nullptr);
        vkDestroyPipelineLayout(device_,   pipelinelayout_, nullptr);
    }


    VkShaderModule createShaderModule(const std::vector<char>& code, const VkDevice& device)
    {
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType    =    VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize =                                    code.size();
        createInfo.pCode    = reinterpret_cast<const uint32_t*>(code.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
            throw std::runtime_error("failed to create shader module!");

        return shaderModule;
    }

    void Pipeline::createGraphicsPipeline()
    {
        auto vertShaderCode = Service::readfile(VERT_SHADER_FILE_NAME);
        auto fragShaderCode = Service::readfile(FRAG_SHADER_FILE_NAME);

        vertshadermodule_ = createShaderModule(vertShaderCode, device_);
        fragshadermodule_ = createShaderModule(fragShaderCode, device_);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage  =                          VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module =                                   vertshadermodule_;
        vertShaderStageInfo.pName  =                                              "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage  =                        VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module =                                   fragshadermodule_;
        fragShaderStageInfo.pName  =                                              "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount                                 = 0;
        vertexInputInfo.vertexAttributeDescriptionCount                               = 0;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology                      = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable        =                            VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount =                                                     1;
        viewportState.scissorCount  =                                                     1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType      = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable        =                                      VK_FALSE;
        rasterizer.rasterizerDiscardEnable =                                      VK_FALSE;
        rasterizer.polygonMode             =                          VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth               =                                          1.0f;
        rasterizer.cullMode                =                         VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace               =                       VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable         =                                      VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable                                   = VK_FALSE;
        multisampling.rasterizationSamples                     = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | 
                                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable    =                                             VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable     =                                                 VK_FALSE;
        colorBlending.logicOp           =                                         VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount   =                                                        1;
        colorBlending.pAttachments      =                                    &colorBlendAttachment;
        colorBlending.blendConstants[0] =                                                     0.0f;
        colorBlending.blendConstants[1] =                                                     0.0f;
        colorBlending.blendConstants[2] =                                                     0.0f;
        colorBlending.blendConstants[3] =                                                     0.0f;


        std::vector<VkDynamicState> dynamicStates = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount =          static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates    =                                 dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount         =                                             0;
        pipelineLayoutInfo.pushConstantRangeCount =                                             0;

        if (vkCreatePipelineLayout(device_, &pipelineLayoutInfo, nullptr, &pipelinelayout_) != VK_SUCCESS)
            throw std::runtime_error("failed to create pipeline layout!");

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount          =                                               2;
        pipelineInfo.pStages             =                                    shaderStages;
        pipelineInfo.pVertexInputState   =                                &vertexInputInfo;
        pipelineInfo.pInputAssemblyState =                                  &inputAssembly;
        pipelineInfo.pViewportState      =                                  &viewportState;
        pipelineInfo.pRasterizationState =                                     &rasterizer;
        pipelineInfo.pMultisampleState   =                                  &multisampling;
        pipelineInfo.pColorBlendState    =                                  &colorBlending;
        pipelineInfo.pDynamicState       =                                   &dynamicState;
        pipelineInfo.layout              =                                 pipelinelayout_;
        pipelineInfo.renderPass          =                                     renderpass_;
        pipelineInfo.subpass             =                                               0;
        pipelineInfo.basePipelineHandle  =                                  VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicspipeline_) != VK_SUCCESS)
            throw std::runtime_error("failed to create graphics pipeline!");
    }

    void Pipeline::bind_with_cmdbuffer(VkCommandBuffer commandBuffer)
    {
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicspipeline_);
    }

}   //  end of VKPipeline namespace
