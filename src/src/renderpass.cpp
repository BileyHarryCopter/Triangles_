#include "swapchain.hpp"

namespace VKSwapchain
{

    void Swapchain::createRenderPass()
    {
        
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format         =            swapchainimageformat_;
        colorAttachment.samples        =            VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp         =      VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp        =     VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp  =  VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout  =        VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout    =  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment  =                                        0;
        colorAttachmentRef.layout      = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint      = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount   =                               1;
        subpass.pColorAttachments      =             &colorAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass          =                            VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass          =                                              0;
        dependency.srcStageMask        =  VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask       =                                              0;
        dependency.dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
                                             VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        dependency.dstAccessMask       =           VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount =                                         1;
        renderPassInfo.pAttachments    =                          &colorAttachment;
        renderPassInfo.subpassCount    =                                         1;
        renderPassInfo.pSubpasses      =                                  &subpass;

        if (vkCreateRenderPass(device_.get_logic(), &renderPassInfo, nullptr, &renderpass_) != VK_SUCCESS)
            throw std::runtime_error("failed to create render pass!");
    }

}   //  end of VKDevice namespace
