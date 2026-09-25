#ifndef UTILITY_HPP_
#define UTILITY_HPP_

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace utility {
const int kWidth = 800;       // logical
const int kHeight = 600;      //  size
const int kMajorVersion = 4;  // result version will -
const int kMinorVersion = 1;  // be 4.1
const int kLowerLeftX = 0;
const int kLowerLeftY = 0;

enum class ErrorType { kCorrect, kError };
}  // namespace utility

void InitOpenGl();

GLFWwindow* CreateWindow(const char* win_name);

void ConfigureViewing(GLFWwindow* win);

utility::ErrorType InitGlad();

void CallbackSettings(GLFWwindow* win);

void FrameBufferSizeCallback(GLFWwindow* win, int width, int height);

void CleanResources();

void MouseCallback(GLFWwindow* win, double xpos, double ypos);

#endif
