#include "program.hpp"

Program::Program() {}

Program::~Program() {
  if (id_) {
    glDeleteProgram(id_);
  }
}

std::unique_ptr<Program> Program::Create(
    const std::vector<std::shared_ptr<Shader>> &shaders) {
  auto program = std::unique_ptr<Program>(new Program());
  if (!program->Link(shaders)) {
    return nullptr;
  }

  return std::move(program);
}

std::unique_ptr<Program> Program::Create(const std::string &vs_filename,
                                         const std::string &fs_filename,
                                         const std::string &gs_filename) {
  std::shared_ptr<Shader> vs =
      Shader::CreateFromFile(vs_filename, GL_VERTEX_SHADER);
  std::shared_ptr<Shader> fs =
      Shader::CreateFromFile(fs_filename, GL_FRAGMENT_SHADER);
  std::shared_ptr<Shader> gs =
      !gs_filename.empty()
          ? Shader::CreateFromFile(gs_filename, GL_GEOMETRY_SHADER)
          : nullptr;
  if (!vs || !fs || (!gs_filename.empty() && !gs)) {
    return nullptr;
  }
  std::vector<std::shared_ptr<Shader>> shaders({vs, fs});
  if (gs) shaders.push_back(gs);

  return Create(shaders);
}

bool Program::Link(const std::vector<std::shared_ptr<Shader>> &shaders) {
  id_ = glCreateProgram();
  for (auto &shader : shaders) {
    glAttachShader(id_, shader->id());
  }
  glLinkProgram(id_);

  int success = 0;
  glGetProgramiv(id_, GL_LINK_STATUS, &success);
  if (!success) {
    char infoLog[512];
    glGetProgramInfoLog(id_, 512, nullptr, infoLog);
    SPDLOG_ERROR("ERROR::PROGRAM::LINKING_FAILED");
    SPDLOG_ERROR("{}", infoLog);
  }

  return success;
}

uint32_t Program::GetUniformLocation(const std::string &name) const {
  return glGetUniformLocation(id_, name.c_str());
}

void Program::SetUniform(const std::string &name, int value) const {
  uint32_t loc = GetUniformLocation(name);
  glUniform1i(loc, value);
}

void Program::SetUniform(const std::string &name, float value) const {
  uint32_t loc = GetUniformLocation(name);
  glUniform1f(loc, value);
}

void Program::SetUniform(const std::string &name,
                         const glm::vec2 &value) const {
  uint32_t loc = GetUniformLocation(name);
  glUniform2fv(loc, 1, glm::value_ptr(value));
}

void Program::SetUniform(const std::string &name,
                         const glm::vec3 &value) const {
  uint32_t loc = GetUniformLocation(name);
  glUniform3fv(loc, 1, glm::value_ptr(value));
}

void Program::SetUniform(const std::string &name,
                         const glm::vec4 &value) const {
  uint32_t loc = GetUniformLocation(name);
  glUniform4fv(loc, 1, glm::value_ptr(value));
}

void Program::SetUniform(const std::string &name,
                         const glm::mat4 &value) const {
  uint32_t loc = GetUniformLocation(name);
  glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void Program::SetUniform(const std::string &name,
                         const std::vector<glm::mat4> &value) const {
  uint32_t loc = GetUniformLocation(name);
  glUniformMatrix4fv(loc, value.size(), GL_FALSE,
                     glm::value_ptr(*(value.data())));
}
