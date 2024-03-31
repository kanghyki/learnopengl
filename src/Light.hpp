#ifndef INCLUDED_LIGHT_HPP
#define INCLUDED_LIGHT_HPP

#include "common.hpp"

struct Light
{
    glm::vec3 position  { glm::vec3(3.0f, 3.0f, 3.0f) };
    glm::vec3 ambient   { glm::vec3(0.0f, 0.0f, 0.0f) };
    glm::vec3 diffuse   { glm::vec3(1.0f, 1.0f, 1.0f) };
    glm::vec3 specular  { glm::vec3(1.0f, 1.0f, 1.0f) };
};

struct Material
{
    glm::vec3 ambient   { glm::vec3(0.6f, 1.0f, 1.0f) };
    glm::vec3 diffuse   { glm::vec3(0.7f, 1.0f, 1.0f) };
    glm::vec3 specular  { glm::vec3(0.8f, 1.0f, 1.0f) };
    float shininess     { 30.0f };
};

#endif