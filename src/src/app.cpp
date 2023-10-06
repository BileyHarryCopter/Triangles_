#include "app.hpp"

#include "render_system.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <cassert>
#include <stdexcept>

namespace VKEngine
{

    void App::run()
    {
        VKRenderSystem::RenderSystem renderSystem{device_, renderer_.getSwapChainRenderPass()};

        while(!window_.shouldClose())
        {
            glfwPollEvents();

            if (auto commandBuffer = renderer_.beginFrame())
            {
                renderer_.beginSwapchainRenderpass(commandBuffer);
                renderSystem.renderObjects(commandBuffer, objects_);
                renderer_.endSwapchainRenderpass(commandBuffer);
                renderer_.endFrame();
            }

        }

        vkDeviceWaitIdle(device_.get_logic());
    }

    void App::loadObjects()
    {
        //  here the logical part of the intersection of tringles should be connected with visualization part
        std::vector<VKModel::Vertex> vertices 
        {
            {{ 0.0,-0.5}, { 1.0, 0.0, 0.0}}, 
            {{ 0.5, 0.5}, { 0.0, 1.0, 0.0}}, 
            {{-0.5, 0.5}, { 0.0, 0.0, 1.0}}
        };

        auto model = std::make_shared<VKModel::Model>(device_, vertices);  //  accessing to one model for multiple objects

        auto triangle   = VKObject::Object::createObject();
        triangle.model_ =                            model;
        triangle.color_ = {0.1f, 0.8f, 0.1f};
        triangle.transform2D_.translation.x = 0.2f;
        triangle.transform2D_.scale = {2.0f, 0.5f};
        triangle.transform2D_.rotation = 0.25 * glm::two_pi<float>();

        objects_.push_back(std::move(triangle));
    }

}   //  end of VKEngine namespace
