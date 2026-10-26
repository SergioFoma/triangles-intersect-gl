#include "shader.hpp"

#include <fstream>
#include <sstream>

#include "prog_error.hpp"

ShaderId Shader::CreateShader(const char* shader_ptr, GLenum shader_type) {
  assert(shader_ptr);

  std::uintptr_t shader_id = glCreateShader(shader_type);

  ShaderId vert_shader{reinterpret_cast<std::uintptr_t*>(shader_id),
                       ShaderIdDeleter{}};

  glShaderSource(shader_id, kLineCount, &shader_ptr, nullptr);
  glCompileShader(shader_id);

  int error_code = 0;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &error_code);

  if (!error_code) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kShader,
                                 "Error of vertex sahder compilation",
                                 "Shader::CreateShader"};
    throw prog_error::ProgError(std::move(inf));
  }

  return vert_shader;
}

Shader::Shader(const std::string& vert_path, const std::string& frag_path) {
  std::string vertex_str = ReadShader(vert_path);
  std::string frag_str = ReadShader(frag_path);

  const char* vertex_ptr = vertex_str.c_str();
  const char* fragment_ptr = frag_str.c_str();

  assert(vertex_ptr && fragment_ptr);

  vertex_shader_    =   CreateShader(vertex_ptr, GL_VERTEX_SHADER);
  fragment_shader_  =   CreateShader(fragment_ptr, GL_FRAGMENT_SHADER);

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
  std::uintptr_t shader_program_id = glCreateProgram();
  ProgramId shader_program_ptr{reinterpret_cast<std::uintptr_t*>(shader_program_id),
                             ProgramIdDeleter{}};
  shader_program_ = std::move(shader_program_ptr);

  std::uintptr_t vertex_sh_id = reinterpret_cast<std::uintptr_t>(vertex_shader_.get());
  std::uintptr_t fragment_sh_id = reinterpret_cast<std::uintptr_t>(fragment_shader_.get());

  glAttachShader(shader_program_id, vertex_sh_id);
  glAttachShader(shader_program_id, fragment_sh_id);
  glLinkProgram(shader_program_id);

  GLint error_code = 0;
  glGetProgramiv(shader_program_id, GL_LINK_STATUS, &error_code);

  if (!error_code || glGetError()) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kShader,
                                   "Error of shader linking",
                                   "Shader::LinkShaders"};
    throw prog_error::ProgError(std::move(inf));
  }
  return shader_program_id;
}
