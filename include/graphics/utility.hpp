#ifndef UTILITY_HPP_
#define UTILITY_HPP_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace utility {

enum class ErrorType { kCorrect, kError };

struct WinConfig {
  int width = 0;
  int height = 0;
  int major_version = 0;
  int minor_version = 0;
};

void InitOpenGl(const utility::WinConfig& win_con);

GLFWwindow* CreateWindow(const char* win_name, const utility::WinConfig& win_con);

void ConfigureViewing(GLFWwindow* win);

utility::ErrorType InitGlad();

void CleanResources();

namespace detail {
void MouseCallback(GLFWwindow* win, double xpos, double ypos);
} // namespace detail

}  // namespace utility

#endif
