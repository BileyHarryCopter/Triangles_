#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include "device.hpp"

namespace VKModel
{

struct Vertex
{
    glm::vec2 position;
    glm::vec3    color;

    static std::vector<VkVertexInputBindingDescription>     get_binding_descriptions();
    static std::vector<VkVertexInputAttributeDescription> get_attribute_descriptions();
};

class Model final
{
    VKDevice::Device&                        device_;
    VkBuffer            vertexbuff_ = VK_NULL_HANDLE;
    VkDeviceMemory   vertexbuffmem_ = VK_NULL_HANDLE;   //  for precise memory management
    uint32_t           vertexcount_ =              0;

public:

    Model (VKDevice::Device& device, const std::vector<Vertex>& vertices);
    ~Model();

    Model(const Model &rhs) = delete;
    Model &operator=(const Model& rhs) = delete;

    void bind(VkCommandBuffer commandbuffer);
    void draw(VkCommandBuffer commandbuffer);

private:
    void createVertexBuffer(const std::vector<Vertex>& vertices);
};

void createBuffer(VKDevice::Device& device, VkDeviceSize size,
                  VkBufferUsageFlags usage, VkMemoryPropertyFlags properties,
                  VkBuffer &buffer, VkDeviceMemory &bufferMemory);

}   //  end of the VKModel namespace
