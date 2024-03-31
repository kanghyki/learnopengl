#ifndef INCLUDED_CAMERA_HPP
#define INCLUDED_CAMERA_HPP

#include "common.hpp"

struct Camera
{
    glm::mat4   getViewMatrix();
    void        reset();

    float       pitch   {0.0f};
    float       yaw     {0.0f};
    glm::vec3   pos     {0.0f, 0.0f, 0.0f};
    glm::vec3   target  {0.0f, 0.0f, -1.0f};
    glm::vec3   up      {0.0f, 1.0f, 0.0f};
};

#endif
