#ifndef PARSING_SHADERS_HPP_
#define PARSING_SHADERS_HPP_

#include <string>

std::string ReadShader(const std::string& file_path);

unsigned int LinkShaders(const char* vertex_ptr, const char* fragment_ptr);

#endif
