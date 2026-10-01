#include "utility.hpp"

#include <cassert>
#include <iostream>
#include "geometry_buffer.hpp"

namespace {
const int kLowerLeftX = 0;
const int kLowerLeftY = 0;
} // namespace


void InitOpenGl(const utility::WinConfig& win_con) {
  /*
        GLFW_OPENGL_CORE_PROFILE - usu only modern function
        GL_TRUE                  - delete all old function
  */

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, win_con.major_version);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, win_con.minor_version);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

GLFWwindow* CreateWindow(const char* win_name, const utility::WinConfig& win_con) {

  GLFWwindow* win =
      glfwCreateWindow(win_con.width, win_con.height, win_name, nullptr, nullptr);
  if (!win) {
    std::cout << "glfwCreateWindow: window is nullptr!\n";
  }
    
  glfwMakeContextCurrent(win);

  return win;
}

void ConfigureViewing(GLFWwindow* win) {
  assert(win);

  int phys_width = 0, phys_height = 0;
  glfwGetFramebufferSize(win, &phys_width, &phys_height);

  glViewport(0, 0, phys_width,
             phys_height);  // configuring the rendering window

  glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glEnable(GL_DEPTH_TEST);

  CallbackSettings(win);
}

utility::ErrorType InitGlad() {
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "InitGlag: failed to initialize GLAD\n";
    return utility::ErrorType::kError;
  }

  return utility::ErrorType::kCorrect;
}

void CallbackSettings(GLFWwindow* win) {
  assert(win);

  glfwSetFramebufferSizeCallback(win, FrameBufferSizeCallback);

  glfwSetCursorPosCallback(win, MouseCallback);
}

void FrameBufferSizeCallback(GLFWwindow* win, int width, int height) {
  assert(win);

  glViewport(kLowerLeftX, kLowerLeftY, width, height);
}

void CleanResources() {
  glfwTerminate();  // cleaning resources
}
