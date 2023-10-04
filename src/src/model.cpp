#include "model.hpp"

#include <cassert>
#include <cstring>

namespace VKModel
{

    Model::Model (VKDevice::Device& device, const std::vector<Vertex>& vertices) :
                  device_{device}
    {
        createVertexBuffer(vertices);
    }

    Model::~Model()
    {
        vkDestroyBuffer(device_.get_logic(),    vertexbuff_, nullptr);
        vkFreeMemory   (device_.get_logic(), vertexbuffmem_, nullptr);
    }

    void Model::createVertexBuffer(const std::vector<Vertex>& vertices)
    {
        vertexcount_ = static_cast<uint32_t>(vertices.size());
        assert(vertexcount_ >= 3 && "Vertex count must be at least 3\n");

        VkDeviceSize buffsize = sizeof(vertices[0]) * vertexcount_;

        createBuffer(device_, buffsize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     vertexbuff_, vertexbuffmem_);

        void *mapping_data = nullptr;
        vkMapMemory  (device_.get_logic(), vertexbuffmem_, 0, buffsize, 0, &mapping_data);
        memcpy       (mapping_data, vertices.data(), static_cast<size_t>(buffsize));
        vkUnmapMemory(device_.get_logic(), vertexbuffmem_);
    }

    void Model::draw(VkCommandBuffer commandbuffer)
    {
        vkCmdDraw(commandbuffer, vertexcount_, 1, 0, 0);    //  put here some constants
    }

    void Model::bind(VkCommandBuffer commandbuffer)
    {
        VkBuffer buffers[] = {vertexbuff_};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandbuffer, 0, 1, buffers, offsets);  //  and here too
    }

    std::vector<VkVertexInputBindingDescription>     Vertex::get_binding_descriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions{};

        bindingDescriptions.push_back({0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX});

        return bindingDescriptions;
    }
    std::vector<VkVertexInputAttributeDescription> Vertex::get_attribute_descriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back({0, 0,    VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, position)});
        attributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT,    offsetof(Vertex, color)});

        return attributeDescriptions;
    }

}   //  end of the VKModel namespace
