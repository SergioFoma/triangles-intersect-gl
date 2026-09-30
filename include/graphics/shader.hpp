#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <stdexcept>

class Shader {
 public:
  Shader(const Shader& other) = delete;

  Shader(Shader&& other)
    : vertex_shader_(other.vertex_shader_),
      fragment_shader_(other.fragment_shader_),
      shader_program_(other.shader_program_) {
      other.shader_program_ = 0;
  };

  Shader(const std::string& vert_path, const std::string& frag_path);

  void SetMat4(const std::string& name, const glm::mat4& mat) const {
    const char* name_ptr = name.c_str();

    assert(name_ptr);

    int mat_loc = glGetUniformLocation(shader_program_, name_ptr);
    glUniformMatrix4fv(mat_loc, kOneMatrix, GL_FALSE, glm::value_ptr(mat));

    if (glGetError()) throw std::runtime_error("SetMat4 has an error!");
  }

  void SetMat3(const std::string& name, const glm::mat3& mat) const {
    const char* name_ptr = name.c_str();

    assert(name_ptr);

    int mat_loc = glGetUniformLocation(shader_program_, name_ptr);
    glUniformMatrix3fv(mat_loc, kOneMatrix, GL_FALSE, glm::value_ptr(mat));

    if (glGetError()) throw std::runtime_error("SetMat3 has an error!");
  }

  void SetVec3(const std::string& name, const glm::vec3& vec) const {
    const char* name_ptr = name.c_str();

    assert(name_ptr);

    int vec_loc = glGetUniformLocation(shader_program_, name_ptr);
    glUniform3fv(vec_loc, kOneVec, glm::value_ptr(vec));

    if (glGetError()) throw std::runtime_error("SetVec3 has an error!");
  }

  void SetFloat(const std::string& name, float val) const {
    const char* name_ptr = name.c_str();

    assert(name_ptr);

    int val_loc = glGetUniformLocation(shader_program_, name_ptr);
    glUniform1f(val_loc, val);

    if (glGetError()) throw std::runtime_error("SetFloat has an error!");
  }

  void SetInt(const std::string& name, int val) const {
    const char* name_ptr = name.c_str();

    assert(name_ptr);

    int val_loc = glGetUniformLocation(shader_program_, name_ptr);
    glUniform1i(val_loc, val);

    if (glGetError()) throw std::runtime_error("SetInt has an error!");
  }

  void SetFloat(const std::string& name, float val) const {
    const char* name_ptr = name.c_str();

    assert(name_ptr);

    int val_loc = glGetUniformLocation(shader_program_, name_ptr);
    glUniform1f(val_loc, val);
  }

  void SetInt(const std::string& name, int val) const {
    const char* name_ptr = name.c_str();

    assert(name_ptr);

    int val_loc = glGetUniformLocation(shader_program_, name_ptr);
    glUniform1i(val_loc, val);
  }

  void Use() const { glUseProgram(shader_program_); }

  void Disable() const { glUseProgram(kDisable); }

  unsigned int GetVertexSh() const { return vertex_shader_; }

  unsigned int GetFragmentSh() const { return fragment_shader_; }

  unsigned int GetShaderProg() const { return shader_program_; }

  Shader& operator=(const Shader& other) = delete;

  Shader& operator=(Shader&& other) {
    if (this == &other) {
      return *this;
    }

    glDeleteProgram(shader_program_);
    shader_program_ = other.shader_program_;
    other.shader_program_ = 0;

    return *this;
  };

  ~Shader() { glDeleteProgram(shader_program_); }

 private:
  unsigned int vertex_shader_ = 0;
  unsigned int fragment_shader_ = 0;
  unsigned int shader_program_ = 0;
  static constexpr unsigned int kLineCount = 1;
  static constexpr unsigned int kDisable = 0;
  static constexpr unsigned int kOneMatrix = 1;
  static constexpr unsigned int kOneVec = 1;

  std::string ReadShader(const std::string& file_path) const;

  unsigned int LinkShaders();
};

#endif
