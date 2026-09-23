#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <sstream>
#include <fstream>
#include <cassert>

#include "parsing_shaders.hpp"

std::string ReadShader(const std::string& file_path) {
    std::ifstream shader_config(file_path);

    std::stringstream ss;
    ss << shader_config.rdbuf();

    std::string str = ss.str();

    return str;
}

unsigned int LinkShaders(const char* vertex_ptr, const char* fragment_ptr) {
    assert(vertex_ptr);
    assert(fragment_ptr);

    int line_count = 1;

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, line_count, &vertex_ptr, NULL);
    glCompileShader(vertex_shader);

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, line_count, &fragment_ptr, NULL);
    glCompileShader(fragment_shader);

    unsigned int shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glDeleteShader(vertex_shader);       // no need, because we've already linked before.
    glDeleteShader(fragment_shader);     // no need, because we've already linked before.

    return shader_program;

}


