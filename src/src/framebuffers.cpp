#include "swapchain.hpp"

namespace VKSwapchain
{

    void Swapchain::createFramebuffers()
    {
        swapchainframebuffers_.resize(swapchainimageviews_.size());

        for (size_t i = 0; i < swapchainimageviews_.size(); i++) 
        {
            VkImageView attachments[]       =                  {swapchainimageviews_[i]};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass      =                               renderpass_;
            framebufferInfo.attachmentCount =                                         1;
            framebufferInfo.pAttachments    =                               attachments;
            framebufferInfo.width           =                    swapchainextent_.width;
            framebufferInfo.height          =                   swapchainextent_.height;
            framebufferInfo.layers          =                                         1;

            if (vkCreateFramebuffer(device_.get_logic(), &framebufferInfo, nullptr, &swapchainframebuffers_[i]) != VK_SUCCESS)
                throw std::runtime_error("failed to create framebuffer!");
        }
    }

}   //  end of the VKSwapchain namespace
