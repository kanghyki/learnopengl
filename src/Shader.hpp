#ifndef INCLUDED_SHADER_HPP
#define INCLUDED_SHADER_HPP

#include "common.hpp"

class Shader {
public:
  ~Shader();
  static std::shared_ptr<Shader> createFromFile(const std::string &fileName,
                                                GLenum shaderType);
  uint32_t getId() const;

private:
  Shader();
  bool loadFile(const std::string &fileName, GLenum shaderType);

  uint32_t mId{0};
};

#endif