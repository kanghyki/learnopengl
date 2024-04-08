#ifndef INCLUDED_SHADER_HPP
#define INCLUDED_SHADER_HPP

#include "common.hpp"

class Shader {
 public:
  static std::shared_ptr<Shader> CreateFromFile(const std::string &filename,
                                                GLenum shader_type);
  ~Shader();

  inline const uint32_t id() const { return id_; }

 private:
  Shader();
  bool LoadFile(const std::string &filename, GLenum shader_type);

  uint32_t id_{0};
};

#endif