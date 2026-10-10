#include "shader.hpp"

#include <fstream>
#include <sstream>

#include "prog_error.hpp"

Shader::Shader(const std::string& vert_path, const std::string& frag_path) {
  std::string vertex_str = ReadShader(vert_path);
  std::string frag_str = ReadShader(frag_path);

  const char* vertex_ptr = vertex_str.c_str();
  const char* fragment_ptr = frag_str.c_str();

  assert(vertex_ptr);
  assert(fragment_ptr);

  int error_code = 0;

  vertex_shader_ = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader_, kLineCount, &vertex_ptr, nullptr);
  glCompileShader(vertex_shader_);
  glGetShaderiv(vertex_shader_, GL_COMPILE_STATUS, &error_code);

  if (!error_code) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kShader,
                                   "Error of vertex sahder compilation",
                                   "Shader::Shader"};
    throw prog_error::ProgError(std::move(inf));
  }

  fragment_shader_ = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader_, kLineCount, &fragment_ptr, nullptr);
  glCompileShader(fragment_shader_);
  glGetShaderiv(fragment_shader_, GL_COMPILE_STATUS, &error_code);

  if (!error_code) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kGeomBuff,
                                   "Error of fragment shader compilation!",
                                   "Shader::Shader"};
    throw prog_error::ProgError(std::move(inf));
  }

  LinkShaders();
}

std::string Shader::ReadShader(const std::string& file_path) const {
  std::ifstream shader_config(file_path);

  if (!shader_config) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kShader,
                                  "Error of opening shader",
                                  "Shader::ReadShader"};
    throw prog_error::ProgError(std::move(inf));
  }

  std::stringstream ss;
  ss << shader_config.rdbuf();

  std::string str = ss.str();

  int error_code = glGetError();
  if (error_code) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kShader,
                                  "Error of reading shader",
                                  "Shader::ReadShader"};
    throw prog_error::ProgError(std::move(inf));
  }

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
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kShader,
                                   "Error of shader linking",
                                   "Shader::LinkShaders"};
    throw prog_error::ProgError(std::move(inf));
  }

  glDeleteShader(
      vertex_shader_);  // after linking, the shader is no longer needed - we delete it.
  glDeleteShader(
      fragment_shader_);  // after linking, the shader is no longer needed - we delete it.
  vertex_shader_ = 0;
  fragment_shader_ = 0;

  error_code = glGetError();
  if (error_code) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kShader,
                                   "Error of linking shaders",
                                   "Shader::LinkShaders"};
      throw prog_error::ProgError(std::move(inf));
  }

  return shader_program_;
}
