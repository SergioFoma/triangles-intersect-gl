#include "shader.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>

Shader::Shader(const std::string& vert_path, const std::string& frag_path) {
  std::string vertex_str = ReadShader(vert_path);
  std::string frag_str = ReadShader(frag_path);

  const char* vertex_ptr = vertex_str.c_str();
  const char* fragment_ptr = frag_str.c_str();

  assert(vertex_ptr);
  assert(fragment_ptr);

  int error_code = 0;

  vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader_, kLineCount, &vertex_ptr, NULL);
  glCompileShader(vertex_shader_);
  glGetShaderiv(vertex_shader_, GL_COMPILE_STATUS, &error_code);

  if (!error_code) {
    throw std::runtime_error("Vertex shader compilation error!");
  }

  fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader_, kLineCount, &fragment_ptr, NULL);
  glCompileShader(fragment_shader_);
  glGetShaderiv(fragment_shader_, GL_COMPILE_STATUS, &error_code);

  if (!error_code) {
    throw std::runtime_error("Fragment shader compilation error!");
  }

  LinkShaders();
}

std::string Shader::ReadShader(const std::string& file_path) const {
  std::ifstream shader_config(file_path);

  if (!shader_config) {
    throw std::runtime_error("Shader opening error!");
  }

  std::stringstream ss;
  ss << shader_config.rdbuf();

  std::string str = ss.str();

  return str;
}

unsigned int Shader::LinkShaders() {
  shader_program_ = glCreateProgram();
  glAttachShader(shader_program_, vertex_shader_);
  glAttachShader(shader_program_, fragment_shader_);
  glLinkProgram(shader_program_);

  GLint error_code;
  glGetProgramiv(shader_program_, GL_LINK_STATUS, &error_code);

  if (!error_code) {
    throw std::runtime_error("Shader linking error!");
  }

  glDeleteShader(
      vertex_shader_);  // after linking, the shader is no longer needed - we delete it.
  glDeleteShader(
      fragment_shader_);  // after linking, the shader is no longer needed - we delete it.

  return shader_program_;
}
