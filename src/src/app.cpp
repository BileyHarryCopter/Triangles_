#include "app.hpp"

namespace VKEngine
{

    void App::run()
    {
        while(!window_.shouldClose())
        {
            glfwPollEvents();
            renderer_.drawFrame();
        }

        vkDeviceWaitIdle(device_.get_logic());
    }

}   //  end of VKEngine namespace
