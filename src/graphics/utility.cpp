#include "utility.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>

#include "prog_error.hpp"

namespace {
const int kLowerLeftX = 0;
const int kLowerLeftY = 0;

void FrameBufferSizeCallback(GLFWwindow* win, int width, int height) {
  glViewport(kLowerLeftX, kLowerLeftY, width, height);

  int error_code = glGetError();
  if (error_code) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kUtility,
                                   "Error of frame buffer callback",
                                   "FrameBufferSizeCallback"};
    throw prog_error::ProgError(std::move(inf));
  }
}

void CallbackSettings(utility::Window& win) {

  glfwSetFramebufferSizeCallback(win.GetPtr(), FrameBufferSizeCallback);

  glfwSetCursorPosCallback(win.GetPtr(), utility::detail::MouseCallback);

  int error_code = glGetError();
  if (error_code) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kUtility,
                                   "Error of callback settings",
                                   "CallbackSettings"};
    throw prog_error::ProgError(std::move(inf));
  }
}
} // namespace

utility::Window::Window(const char* win_name, const WinConfig& win_config)
   : width_(win_config.width), height_(win_config.height) {

   glfwSwapInterval(kOn);

   monitor_ = InitMonitor();

   video_ = InitVideo();

  GLFWwindow* window = glfwCreateWindow(width_, height_, win_name, nullptr, nullptr);
  if (!window) {
    utility::CleanResources();
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kUtility,
                                  "Error of creating window",
                                  "Utility::Window"};
    throw prog_error::ProgError(std::move(inf));
  }
  WindowPointer window_ptr{window, WindowDeleter{}};
  window_ = std::move(window_ptr);
  max_width_ = video_.get()->width;
  max_height_ = video_.get()->height;
}

utility::Window utility::InitGraphics(const utility::WinConfig& win_con) {
  utility::InitOpenGl(win_con);

  utility::Window win = utility::CreateWindow("triangle 3D", win_con);

  if (utility::InitGlad() != utility::ErrorType::kCorrect) {
    utility::CleanResources();
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kUtility,
                                   "Error of init glad",
                                   "InitGraphics"};
    throw prog_error::ProgError(std::move(inf));
  }

  utility::ConfigureViewing(win);

  return win;
}

utility::Window::MonitorPointer utility::Window::InitMonitor() {

  GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
  if (!primaryMonitor) {
    utility::CleanResources();
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kUtility,
                                  "Error of geting primary monitor",
                                  "Utility::Window"};
    throw prog_error::ProgError(std::move(inf));
  }
  MonitorPointer monitor_ptr{primaryMonitor, MonitorDeleter{}};

  return monitor_ptr;
}

utility::Window::VideoPointer utility::Window::InitVideo() {
  const GLFWvidmode* videoMode = glfwGetVideoMode(monitor_.get());
  if (!videoMode) {
    utility::CleanResources();
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kUtility,
                                "Error of geting video mode",
                                "Utility::Window"};
    throw prog_error::ProgError(std::move(inf));
  }
  VideoPointer video_ptr{videoMode, VideoDeleter{}};

  return video_ptr;
}

void utility::InitOpenGl(const utility::WinConfig& win_con) {
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

utility::Window utility::CreateWindow(const char* win_name, const utility::WinConfig& win_con) {

  Window win(win_name, win_con);

  glfwMakeContextCurrent(win.GetPtr());

  return win;
}

void utility::ConfigureViewing(utility::Window& win) {

  int phys_width = 0, phys_height = 0;
  glfwGetFramebufferSize(win.GetPtr(), &phys_width, &phys_height);

  glViewport(0, 0, phys_width,
             phys_height);  // configuring the rendering window

  glfwSetInputMode(win.GetPtr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  glEnable(GL_DEPTH_TEST);

  CallbackSettings(win);
}

utility::ErrorType utility::InitGlad() {
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "InitGlag: failed to initialize GLAD\n";
    return utility::ErrorType::kError;
  }

  return utility::ErrorType::kCorrect;
}

void utility::CleanResources() {
  glfwTerminate();  // cleaning resources
}
