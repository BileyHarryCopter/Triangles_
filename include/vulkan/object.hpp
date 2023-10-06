#pragma once

#include "model.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <memory>

namespace VKObject
{

struct Transform2Dcomponent
{
    glm::vec2 translation{};    //  offest translation
    glm::vec2 scale{1.0f, 1.0f};
    float rotation = 0.0f;

    glm::mat2 mat2() 
    { 
        glm::mat2 rotmat {{ glm::cos(rotation), glm::sin(rotation)},
                          {-glm::sin(rotation), glm::cos(rotation)}};

        glm::mat2 scalemat{{scale.x, 0.0f}, {0.0f, scale.y}};
        return rotmat * scalemat; 
    }
};

class Object
{

    using id_t = unsigned int;


    id_t id_;
    Object(id_t id) : id_{id} {}

public:
    static Object createObject ()
    {
        static id_t current_id = 0;
        return Object{current_id++};
    }
    Object(const Object&) = delete;
    Object& operator=(const Object&) = delete;
    Object(Object&&) = default;
    Object& operator=(Object&&) = default;

    const id_t get_id() const { return id_; }

    std::shared_ptr<VKModel::Model> model_{};
    glm::vec3 color_{};
    Transform2Dcomponent transform2D_{};

};

}