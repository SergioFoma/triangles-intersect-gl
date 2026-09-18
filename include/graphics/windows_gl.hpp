#ifndef WINDOWS_GL_HPP_
#define WINDOWS_GL_HPP_

#include <glad/glad.h>      // for ptr settings on functions
#include <GLFW/glfw3.h>     // lib for create windows, work with stdin and stdout, move cursor

// ========================= ATTENTION ========================
// #include glad lib It should go ahead of other libraries
// ============================================================

void ShowWindow();

void InitOpenGl();

int InitGlad();

void CallbackSettings(GLFWwindow* window);

void RenderingCycle(GLFWwindow* window);

void ProcessInput(GLFWwindow* window);

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

#endif
