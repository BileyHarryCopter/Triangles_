#include "renderer.hpp"

namespace VKRenderer
{
    Renderer::Renderer (VKDevice::Device& device, VKSwapchain::Swapchain& swapchain, VKPipeline::Pipeline& pipeline) : 
                        device_{device}, swapchain_{swapchain}, pipeline_{pipeline}
    {
        loadModels();
        createCommandBuffers();
    }

    void Renderer::loadModels()
    {
        //  here the logical part of the intersection of tringles should be connected with visualization part
        std::vector<VKModel::Vertex> vertices {{{0.0, 0.5}}, {{-0.25, 0.0}}, {{0.25, 0.0}},
                                               {{0.0,-0.1}}, {{0.25, -0.5}}, {{-0.25,-0.5}}};

        model_ = std::make_unique<VKModel::Model>(device_, vertices);
    }

    void Renderer::createCommandBuffers()
    {
        commandbuffer_.resize(VKSwapchain::MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        =                          device_.get_cmdpool();
        allocInfo.level              =                VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount =   static_cast<uint32_t>(commandbuffer_.size());

        if (vkAllocateCommandBuffers(device_.get_logic(), &allocInfo, commandbuffer_.data()) != VK_SUCCESS)
            throw std::runtime_error("failed to allocate command buffers!");

        for (auto imageindex = 0; imageindex < VKSwapchain::MAX_FRAMES_IN_FLIGHT; ++imageindex)
            recordCommandBuffer(commandbuffer_[imageindex], imageindex);
    }

    void Renderer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) 
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
            throw std::runtime_error("failed to begin recording command buffer!");

        auto swapchain_extent = swapchain_.get_extent();
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass        =              swapchain_.get_renderpass();
        renderPassInfo.framebuffer       =   swapchain_.get_framebuffer(imageIndex);
        renderPassInfo.renderArea.offset =                                   {0, 0};
        renderPassInfo.renderArea.extent =                         swapchain_extent;

        VkClearValue clearColor          = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount   =                            1;
        renderPassInfo.pClearValues      =                  &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x        =                            0.0f;
        viewport.y        =                            0.0f;
        viewport.width    = (float)  swapchain_extent.width;
        viewport.height   = (float) swapchain_extent.height;
        viewport.minDepth =                            0.0f;
        viewport.maxDepth =                            1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset =          {0, 0};
        scissor.extent = swapchain_extent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);            

        //  hear we need to bind pipeline
        pipeline_.bind(commandBuffer);
        model_ -> bind(commandBuffer);
        model_ -> draw(commandBuffer);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
            throw std::runtime_error("failed to record command buffer!");
    }

    void Renderer::drawFrame()
    {
        vkWaitForFences(device_.get_logic(), 1, &swapchain_.get_fence(), VK_TRUE, UINT64_MAX);
        vkResetFences  (device_.get_logic(), 1, &swapchain_.get_fence());

        uint32_t imageIndex;
        vkAcquireNextImageKHR(device_.get_logic(), swapchain_.get_swapchain(), UINT64_MAX, 
                              swapchain_.get_available_img_semaphore(), VK_NULL_HANDLE, &imageIndex);

        vkResetCommandBuffer(commandbuffer_[swapchain_.get_index_currentframe()], 0);
        recordCommandBuffer (commandbuffer_[swapchain_.get_index_currentframe()], imageIndex);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore      waitSemaphores[] =           {swapchain_.get_available_img_semaphore()};

        VkPipelineStageFlags waitStages[] =      {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount     =                                                    1;
        submitInfo.pWaitSemaphores        =                                       waitSemaphores;
        submitInfo.pWaitDstStageMask      =                                           waitStages;
        submitInfo.commandBufferCount     =                                                    1;
        submitInfo.pCommandBuffers        = &commandbuffer_[swapchain_.get_index_currentframe()];

        VkSemaphore signalSemaphores[]    =           {swapchain_.get_finished_rndr_semaphore()};
        submitInfo.signalSemaphoreCount   =                                                    1;
        submitInfo.pSignalSemaphores      =                                     signalSemaphores;

        if (vkQueueSubmit(device_.get_graphics_queue(), 1, &submitInfo, swapchain_.get_fence()) != VK_SUCCESS)
            throw std::runtime_error("failed to submit draw command buffer!");

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount =                                  1;
        presentInfo.pWaitSemaphores    =                   signalSemaphores;

        VkSwapchainKHR swapChains[] = {swapchain_.get_swapchain()};
        presentInfo.swapchainCount  =                            1;
        presentInfo.pSwapchains     =                   swapChains;
        presentInfo.pImageIndices   =                  &imageIndex;

        vkQueuePresentKHR(device_.get_present_queue(), &presentInfo);

        swapchain_.currentframe_update();
    }
}
