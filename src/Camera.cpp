#include "Camera.hpp"

glm::mat4 Camera::getViewMatrix()
{
    front = glm::rotate(glm::mat4(1.0f), glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f)) *
                glm::rotate(glm::mat4(1.0f), glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f)) *
                glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);

    return glm::lookAt(pos, pos + front, up);
}

void Camera::reset()
{
    pitch  = {0.0f};
    yaw    = {0.0f};
    pos    = {0.0f, 0.0f, 3.0f};
    front  = {0.0f, 0.0f, -1.0f};
    up     = {0.0f, 1.0f, 0.0f};
}