#include "utility.hpp"

#include <assert.h>
#include <iostream>

void InitOpenGl() {
    /*
        GLFW_OPENGL_CORE_PROFILE - usu only modern function
        GL_TRUE                  - delete all old function
    */

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, utility::kMajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, utility::kMinorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

GLFWwindow* CreateWindow(const char* win_name) {

    GLFWwindow* win = glfwCreateWindow(utility::kWidth, utility::kHeight, win_name, NULL, NULL);
    if (!win) {
        std::cout << "glfwCreateWindow: window is nullptr!\n";
    }

    return win;
}

void ConfigureViewing(GLFWwindow* win) {
    assert(win);

    int phys_width = 0, phys_height = 0;
    glfwGetFramebufferSize(win, &phys_width, &phys_height);

    glViewport(0, 0, phys_width, phys_height);  // configuring the rendering window

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

   glfwSetFramebufferSizeCallback(win, framebuffer_size_callback);
}

void framebuffer_size_callback(GLFWwindow* win, int width, int height) {
    assert(win);

    glViewport(utility::kLowerLeftX, utility::kLowerLeftY, width, height);
}

void CleanResources() {
    glfwTerminate();             // cleaning resources
}
