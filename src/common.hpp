#ifndef INCLUDED_COMMON_HPP
#define INCLUDED_COMMON_HPP

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdint.h>
#include <string>
#include <memory>
#include <optional>
#include <fstream>
#include <sstream>

std::optional<std::string>  loadTextFile(const std::string& filename);

#endif
