#ifndef INCLUDED_COMMON_HPP
#define INCLUDED_COMMON_HPP

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
// clang-format on

#include <stdint.h>

#include <fstream>
#include <functional>
#include <memory>
#include <optional>
#include <sstream>
#include <string>

std::optional<std::string> LoadTextFile(const std::string &filename);
std::vector<std::string> Split(const std::string &s, const std::string &sep);

#endif
