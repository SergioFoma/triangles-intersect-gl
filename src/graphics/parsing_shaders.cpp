#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <sstream>
#include <fstream>
#include <assert.h>

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

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, line_count, &vertex_ptr, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, line_count, &fragment_ptr, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);       // no need, because we've already linked before.
    glDeleteShader(fragmentShader);     // no need, because we've already linked before.

    return shaderProgram;

}


