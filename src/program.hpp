#ifndef INCLUDED_PROGRAM_HPP
#define INCLUDED_PROGRAM_HPP

#include "common.hpp"
#include "shader.hpp"

class Program {
  public:
    static std::unique_ptr<Program> Create(const std::vector<std::shared_ptr<Shader>>& shaders);
    static std::unique_ptr<Program> Create(const std::string& vs_filename,
                                           const std::string& fs_filename,
                                           const std::string& gs_filename = "");
    ~Program();

    inline void Use() const { glUseProgram(id_); }

    inline const uint32_t id() const { return id_; }

    uint32_t GetUniformLocation(const std::string& name) const;
    void SetUniform(const std::string& name, int value) const;
    void SetUniform(const std::string& name, float value) const;
    void SetUniform(const std::string& name, const glm::vec2& value) const;
    void SetUniform(const std::string& name, const glm::vec3& value) const;
    void SetUniform(const std::string& name, const glm::vec4& value) const;
    void SetUniform(const std::string& name, const glm::mat4& value) const;
    void SetUniform(const std::string& name, const std::vector<glm::mat4>& value) const;

  private:
    Program();
    bool Link(const std::vector<std::shared_ptr<Shader>>& shaders);

    uint32_t id_{0};
};

#endif
