#ifndef SHADER_HPP_
#define SHADER_HPP_

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <cassert>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Shader {
 public:
  Shader(const Shader& other) = delete;

  Shader(const std::string& vert_path, const std::string& frag_path);

  void SetUniform(const std::string& name, const glm::mat4& projection) const {
    UpdateMatrix(shader_program_, name.c_str(), projection);
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

  void UpdateMatrix(unsigned int shaderProgram, const char* name,
                    const glm::mat4& matrix) const {
    assert(name);

    int mat_loc = glGetUniformLocation(shaderProgram, name);
    glUniformMatrix4fv(mat_loc, kOneMatrix, GL_FALSE, glm::value_ptr(matrix));
  }

  std::string ReadShader(const std::string& file_path) const;

  unsigned int LinkShaders();
};

#endif
