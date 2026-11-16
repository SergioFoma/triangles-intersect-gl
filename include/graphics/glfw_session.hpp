#ifndef GLFW_SESSION_HPP_
#define GLFW_SESSION_HPP_

#include <GLFW/glfw3.h>

#include "configs.hpp"
#include "utility.hpp"

namespace glfw {

class GlfwSession {
 public:
  explicit GlfwSession(const config::WinConfig& win_con) {
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

  ~GlfwSession() {
    glfwTerminate();  // cleaning resources
  }
};
} // namespace glfw

#endif
