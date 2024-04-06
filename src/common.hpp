#ifndef INCLUDED_COMMON_HPP
#define INCLUDED_COMMON_HPP

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// clang-format on

#include <stdint.h>

#include <fstream>
#include <functional>
#include <memory>
#include <optional>
#include <sstream>
#include <string>

std::optional<std::string> loadTextFile(const std::string &filename);
std::vector<std::string> split(const std::string &s, const std::string &sep);

#endif
