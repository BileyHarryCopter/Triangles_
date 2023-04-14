#include "device.hpp"

namespace VKDevice
{

    Device::Device(VKInstance::Instance &instance) : surface_{instance.get_surface()}
    {
        pickPhysicalDevice(instance);
        createLogicalDevice(instance);
        createCommandPool();
    }

    Device::~Device()
    {
        vkDestroyCommandPool(logicdevice_, commandpool_, nullptr);
        vkDestroyDevice(logicdevice_, nullptr);
    }

}   //  end of VKDevice namespace
