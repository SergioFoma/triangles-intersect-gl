#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <string>
#include <utility>
#include <cstdint>

#include "prog_error.hpp"

struct ShaderIdDeleter {
  void operator()(std::uintptr_t* shader_id) const {
    if (shader_id) {
      glDeleteShader(reinterpret_cast<std::uintptr_t>(shader_id));
    }
  }
};

using ShaderId = std::unique_ptr<std::uintptr_t, ShaderIdDeleter>;

struct ProgramIdDeleter {
  void operator()(std::uintptr_t* program_id) const {
    if (program_id) {
      glDeleteProgram(reinterpret_cast<std::uintptr_t>(program_id));
    }
  }
};

using ProgramId = std::unique_ptr<std::uintptr_t, ProgramIdDeleter>;

class Shader {
 public:
  Shader(const Shader& other) = delete;

  Shader(Shader&& other) = default;

  Shader(const std::string& vert_path, const std::string& frag_path);

  void SetMat4(const std::string& name, const glm::mat4& mat) const {
    const char* name_ptr = name.c_str();

    assert(name_ptr);

    std::uintptr_t sh_prog_id = reinterpret_cast<std::uintptr_t>(shader_program_.get());
    int mat_loc = glGetUniformLocation(sh_prog_id, name_ptr);
    glUniformMatrix4fv(mat_loc, kOneMatrix, GL_FALSE, glm::value_ptr(mat));

    if (glGetError()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kShader,
                                   "Error of Mat4 settings",
                                   "Shader::SetMat4"};
      throw prog_error::ProgError(std::move(inf));
    }
  }

  void SetMat3(const std::string& name, const glm::mat3& mat) const {
    const char* name_ptr = name.c_str();

    assert(name_ptr);

    std::uintptr_t sh_prog_id = reinterpret_cast<std::uintptr_t>(shader_program_.get());
    int mat_loc = glGetUniformLocation(sh_prog_id, name_ptr);
    glUniformMatrix3fv(mat_loc, kOneMatrix, GL_FALSE, glm::value_ptr(mat));

    if (glGetError()) {
     prog_error::ErrorInfo inf = {prog_error::ErrorCode::kShader,
                                   "Error of Mat3 settings",
                                   "Shader::SetMat3"};
      throw prog_error::ProgError(std::move(inf));
    }
  }

  void SetVec3(const std::string& name, const glm::vec3& vec) const {
    const char* name_ptr = name.c_str();

    assert(name_ptr);

    std::uintptr_t sh_prog_id = reinterpret_cast<std::uintptr_t>(shader_program_.get());
    int vec_loc = glGetUniformLocation(sh_prog_id, name_ptr);
    glUniform3fv(vec_loc, kOneVec, glm::value_ptr(vec));

    if (glGetError()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kShader,
                                   "Error of Vec3 settings",
                                   "Shader::SetVec3"};
      throw prog_error::ProgError(std::move(inf));
    }
  }

  void SetFloat(const std::string& name, float val) const {
    const char* name_ptr = name.c_str();

    assert(name_ptr);

    std::uintptr_t sh_prog_id = reinterpret_cast<std::uintptr_t>(shader_program_.get());
    int val_loc = glGetUniformLocation(sh_prog_id, name_ptr);
    glUniform1f(val_loc, val);

    if (glGetError()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kShader,
                                   "Error of float settings",
                                   "Shader::SetFloat"};
      throw prog_error::ProgError(std::move(inf));
    }
  }

  void SetInt(const std::string& name, int val) const {
    const char* name_ptr = name.c_str();

    assert(name_ptr);

    std::uintptr_t sh_prog_id = reinterpret_cast<std::uintptr_t>(shader_program_.get());
    int val_loc = glGetUniformLocation(sh_prog_id, name_ptr);
    glUniform1i(val_loc, val);

    if (glGetError()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kShader,
                                   "Error of int setting",
                                   "Shader::SetInt"};
      throw prog_error::ProgError(std::move(inf));
    }
  }

  void Use() const {
    glUseProgram(reinterpret_cast<std::uintptr_t>(shader_program_.get()));
  }

  void Disable() const { glUseProgram(kDisable); }

  std::uintptr_t GetVertexSh() const {
    return reinterpret_cast<std::uintptr_t>(vertex_shader_.get());
  }

  std::uintptr_t GetFragmentSh() const {
    return reinterpret_cast<std::uintptr_t>(fragment_shader_.get());
  }

  std::uintptr_t GetShaderProg() const {
    return reinterpret_cast<std::uintptr_t>(shader_program_.get());
  }

  Shader& operator=(const Shader& other) = delete;

  Shader& operator=(Shader&& other) = default;

  ~Shader() = default;

 private:
  ShaderId vertex_shader_;
  ShaderId fragment_shader_;
  ProgramId shader_program_;
  static constexpr unsigned int kLineCount = 1;
  static constexpr unsigned int kDisable = 0;
  static constexpr unsigned int kOneMatrix = 1;
  static constexpr unsigned int kOneVec = 1;

  std::string ReadShader(const std::string& file_path) const;

  static ShaderId CreateShader(const char* shader_ptr, GLenum shader_type);

  unsigned int LinkShaders();
};

#endif
