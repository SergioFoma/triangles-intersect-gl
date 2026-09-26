#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Shader {
 public:
  Shader(const Shader& other) = delete;

  Shader(const std::string& vert_path, const std::string& frag_path);

  void SetMatUniform(const std::string& name, const glm::mat4& mat) const {
    const char* name_ptr = name.c_str();

    assert(name_ptr);

    int mat_loc = glGetUniformLocation(shader_program_, name_ptr);
    glUniformMatrix4fv(mat_loc, kOneMatrix, GL_FALSE, glm::value_ptr(mat));
  }

  void SetVecUniform(const std::string& name, const glm::vec3& vec) const {
      const char* name_ptr = name.c_str();

      assert(name_ptr);

      int vec_loc = glGetUniformLocation(shader_program_, name_ptr);
      glUniform3fv(vec_loc, kOneVec, glm::value_ptr(vec));
  }

  void Use() const { glUseProgram(shader_program_); }

  void Disable() const { glUseProgram(kDisable); }

  unsigned int GetVertexSh() const { return vertex_shader_; }

  unsigned int GetFragmentSh() const { return fragment_shader_; }

  unsigned int GetShaderProg() const { return shader_program_; }

  Shader& operator=(const Shader& other) = delete;

  ~Shader() { glDeleteProgram(shader_program_); }

 private:
  unsigned int vertex_shader_ = 0;
  unsigned int fragment_shader_ = 0;
  unsigned int shader_program_ = 0;
  static constexpr unsigned int kLineCount = 1;
  static constexpr unsigned int kDisable = 0;
  static constexpr unsigned int kOneMatrix = 1;
  static constexpr unsigned int kOneVec = 1;

  void UpdateMatrix(unsigned int shaderProgram, const char* name,
                    const glm::mat4& matrix) const {
    assert(name);
  }

  std::string ReadShader(const std::string& file_path) const;

  unsigned int LinkShaders();
};

#endif
