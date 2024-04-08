#include "shader.hpp"

Shader::Shader() {}

Shader::~Shader() {
  if (id_) {
    glDeleteShader(id_);
  }
}

std::shared_ptr<Shader> Shader::CreateFromFile(const std::string &filename,
                                               GLenum shader_type) {
  auto shader = std::unique_ptr<Shader>(new Shader());
  if (!shader->LoadFile(filename, shader_type)) {
    return nullptr;
  }

  return std::move(shader);
}

bool Shader::LoadFile(const std::string &filename, GLenum shader_type) {
  auto result = LoadTextFile(filename);
  if (!result.has_value()) {
    return false;
  }

  std::string &code = result.value();
  const char *pSource = code.c_str();
  int32_t codeLength = code.length();

  id_ = glCreateShader(shader_type);
  glShaderSource(id_, 1, &pSource, &codeLength);
  glCompileShader(id_);

  int success = 0;
  glGetShaderiv(id_, GL_COMPILE_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetShaderInfoLog(id_, 512, nullptr, infoLog);
    SPDLOG_ERROR("ERROR::SHADER::COMPILATION_FAILED: {}", filename);
    SPDLOG_ERROR("{}", infoLog);
  }

  return success;
}