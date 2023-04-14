#include "swapchain.hpp"

#include <limits>
#include <algorithm>

namespace VKSwapchain
{

    Swapchain::Swapchain(VKWindow::Window& window, VKDevice::Device& device) : 
                         device_{device}, surface_{device.get_surface()}
    {
        createSwapChain(window);
        createImageViews();
        createRenderPass();
        createFramebuffers();
        createSyncObjects();
    }

    Swapchain::~Swapchain()
    {
        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            vkDestroySemaphore(device_.get_logic(), renderfinishedsemaphore_[i], nullptr);
            vkDestroySemaphore(device_.get_logic(), imageavailablesemaphore_[i], nullptr);
            vkDestroyFence    (device_.get_logic(),           inflightfence_[i], nullptr);
        }

        for (auto imageview : swapchainimageviews_)
            vkDestroyImageView(device_.get_logic(), imageview, nullptr);

        vkDestroySwapchainKHR(device_.get_logic(), swapchain_, nullptr);
    }

    void Swapchain::createSyncObjects() 
    {
        renderfinishedsemaphore_.resize(MAX_FRAMES_IN_FLIGHT);
        imageavailablesemaphore_.resize(MAX_FRAMES_IN_FLIGHT);
                  inflightfence_.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType     =     VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags     =            VK_FENCE_CREATE_SIGNALED_BIT;

        for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            if (vkCreateSemaphore(device_.get_logic(), &semaphoreInfo, nullptr, &imageavailablesemaphore_[i]) != VK_SUCCESS ||
                vkCreateSemaphore(device_.get_logic(), &semaphoreInfo, nullptr, &renderfinishedsemaphore_[i]) != VK_SUCCESS ||
                vkCreateFence    (device_.get_logic(),     &fenceInfo, nullptr,           &inflightfence_[i]) != VK_SUCCESS)
                throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
    {
        for (const auto& availableFormat : availableFormats)
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && 
                availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                return availableFormat;

        return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) 
    {
        for (const auto& availablePresentMode : availablePresentModes)
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
                return availablePresentMode;

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities,
                                VKWindow::Window& window) 
    {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return capabilities.currentExtent;
        else 
        {
            int width, height;
            glfwGetFramebufferSize(window.get(), &width, &height);

            VkExtent2D actualExtent = 
            {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width  = std::clamp(actualExtent.width,
                                             capabilities.minImageExtent.width,
                                             capabilities.maxImageExtent.width);
            actualExtent.height = std::clamp(actualExtent.height,
                                             capabilities.minImageExtent.height,
                                             capabilities.maxImageExtent.height);

            return actualExtent;
        }
    }

    void Swapchain::createSwapChain(VKWindow::Window& window)
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device_.get_phys(), surface_);

        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat   (swapChainSupport.formats_);
        VkPresentModeKHR   presentMode   = chooseSwapPresentMode(swapChainSupport.presentModes_);
        VkExtent2D         extent        = chooseSwapExtent      (swapChainSupport.capabilities_, 
                                                                                         window);

        uint32_t imageCount              =      swapChainSupport.capabilities_.minImageCount + 1;
        if (swapChainSupport.capabilities_.maxImageCount > 0 && 
            imageCount > swapChainSupport.capabilities_.maxImageCount)
            imageCount = swapChainSupport.capabilities_.maxImageCount;

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface =                                    surface_;

        createInfo.minImageCount    =                          imageCount;
        createInfo.imageFormat      =                surfaceFormat.format;
        createInfo.imageColorSpace  =            surfaceFormat.colorSpace;
        createInfo.imageExtent      =                              extent;
        createInfo.imageArrayLayers =                                   1;
        createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        auto indices = device_.get_indices();
        uint32_t queueFamilyIndices[] = {indices.get_graphics_value(),
                                         indices.get_present_value()};

        if (indices.get_graphics_value() != indices.get_present_value())
        {
            createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount =                          2;
            createInfo.pQueueFamilyIndices   =         queueFamilyIndices;
        } 
        else
        {
            createInfo.imageSharingMode      =  VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount =                          0;        
            createInfo.pQueueFamilyIndices   =                    nullptr;  
        }

        createInfo.preTransform   = swapChainSupport.capabilities_.currentTransform;
        createInfo.compositeAlpha =               VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode    =                                     presentMode;
        createInfo.clipped        =                                         VK_TRUE;
        createInfo.oldSwapchain   =                                  VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(device_.get_logic(), &createInfo, nullptr, &swapchain_) != VK_SUCCESS)
            throw std::runtime_error("failed to create swap chain!");

        vkGetSwapchainImagesKHR(device_.get_logic(), swapchain_, &imageCount, nullptr);
        swapchainimages_.resize (imageCount);
        vkGetSwapchainImagesKHR(device_.get_logic(), swapchain_, &imageCount, swapchainimages_.data());

        swapchainimageformat_ = surfaceFormat.format;
        swapchainextent_      =               extent;
    }

    void Swapchain::createImageViews()
    {
        swapchainimageviews_.resize(swapchainimages_.size());

        for (size_t i = 0; i < swapchainimages_.size(); i++) 
        {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image                           =                      swapchainimages_[i];
            createInfo.viewType                        =                    VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format                          =                    swapchainimageformat_;
            createInfo.components.r                    =            VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g                    =            VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b                    =            VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a                    =            VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask     =                VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel   =                                        0;
            createInfo.subresourceRange.levelCount     =                                        1;
            createInfo.subresourceRange.baseArrayLayer =                                        0;
            createInfo.subresourceRange.layerCount     =                                        1;

            if (vkCreateImageView(device_.get_logic(), &createInfo, nullptr, &swapchainimageviews_[i]) != VK_SUCCESS)
                throw std::runtime_error("failed to create image views!");
        }
    }

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
    {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities_);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            details.formats_.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats_.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0) 
        {
            details.presentModes_.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes_.data());
        }

        return details;
    }

}   //  end of VKSwapchain namespace
