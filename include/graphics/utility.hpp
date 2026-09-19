#ifndef UTILITY_HPP_
#define UTILITY_HPP_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

namespace utility {
    const int kWidth = 800;             // logical
    const int kHeight = 600;            //  size
    const int kMajorVersion = 4;        // result version will -
    const int kMinorVersion = 1;        // be 4.1
    const int kLowerLeftX = 0;
    const int kLowerLeftY = 0;

    enum class ErrorType {
        kCorrect,
        kError
    };
} // namespace utility

void InitOpenGl();

GLFWwindow* CreateWindow(const char* win_name);

void ConfigureViewing(GLFWwindow* win);

utility::ErrorType InitGlad();

void CallbackSettings(GLFWwindow* win);

void framebuffer_size_callback(GLFWwindow* win, int width, int height);

void CleanResources();

#endif
