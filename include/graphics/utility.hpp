#ifndef UTILITY_HPP_
#define UTILITY_HPP_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

namespace utility {

struct WindowDeleter {
  void operator()(GLFWwindow* window) const {
    glfwDestroyWindow(window);
  }
};

struct MonitorDeleter {
  void operator()(GLFWmonitor* monitor) const {}
};

struct VideoDeleter {
  void operator()(const GLFWvidmode* vidmode) const {}
};

using WindowPointer = std::unique_ptr<GLFWwindow, WindowDeleter>;

enum class ErrorType { kCorrect, kError };

struct WinConfig {
  int width = 0;
  int height = 0;
  int major_version = 0;
  int minor_version = 0;
};

class Window {
 public:
  Window(const Window* other) = delete;

  Window(Window&& other) = default;

  Window(const char* win_name, const utility::WinConfig& win_config);

  Window& operator=(const Window& other) = delete;

  Window& operator=(Window&& other) = default;

  GLFWwindow* GetPtr() const {
    return window_.get();
  }

  std::pair<int, int> Resize() {
    if (!is_full_screen_) {
      glfwGetWindowPos(GetPtr(), &x_pos_, &y_pos_);
      glfwSetWindowMonitor(GetPtr(), monitor_.get(),
                           kXpos, kYpos,
                           max_width_, max_height_, video_->refreshRate);
      is_full_screen_ = true;
      std::pair<int ,int> size = {max_width_, max_height_};
      return size;
    } else {
      glfwSetWindowMonitor(GetPtr(), NULL,
                           x_pos_, y_pos_,
                           width_, height_, GLFW_DONT_CARE);
      is_full_screen_ = false;
      std::pair<int, int> size = {width_, height_};
      return size;
    }

    return {};
  }

  ~Window() = default;
 private:
  using MonitorPointer = std::unique_ptr<GLFWmonitor, MonitorDeleter>;
  using VideoPointer = std::unique_ptr<const GLFWvidmode, VideoDeleter>;

  WindowPointer window_ = nullptr;
  MonitorPointer monitor_;          // doesn't need clean resources
  VideoPointer video_;              // doesn't need clean resources
  int width_ = 0;
  int height_ = 0;
  int max_width_ = 0;
  int max_height_ = 0;
  int x_pos_ = 0;
  int y_pos_ = 0;
  bool is_full_screen_ = false;

  static constexpr int kXpos = 0;
  static constexpr int kYpos = 0;

  static constexpr int kOn = 1;

  MonitorPointer InitMonitor();

  VideoPointer InitVideo();
};

Window InitGraphics(const utility::WinConfig& win_con);

void InitOpenGl(const utility::WinConfig& win_con);

Window CreateWindow(const char* win_name, const utility::WinConfig& win_con);

void ConfigureViewing(Window& win);

utility::ErrorType InitGlad();

void CleanResources();

namespace detail {
void MouseCallback(GLFWwindow* win, double xpos, double ypos);
} // namespace detail

}  // namespace utility

#endif
