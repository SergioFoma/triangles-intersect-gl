#include <iostream>

#include "windows_gl.hpp"

const int kMajorVersion = 4;              // result version will -
const int kMinorVersion = 1;              // be 4.1
const int kWidth = 800;
const int kHeight = 600;
const int kLowerLeftX = 0;
const int kLowerLeftY = 0;

void ShowWindow() {
    InitOpenGl();

    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "FirstWindow", NULL, NULL);
    if (!window) {
        std::cout << "glfwCreateWindow: window is nullptr!\n";
        return ;
    }

    glfwMakeContextCurrent(window);

    InitGlad();

    glViewport(kLowerLeftX, kLowerLeftY, kWidth, kHeight);  // configuring the rendering window

    CallbackSettings(window);

    RenderingCycle(window);

    glfwTerminate();                                        // cleaning resources
}

void InitOpenGl() {
    /*
        GLFW_OPENGL_CORE_PROFILE - usu only modern function
        GL_TRUE                  - delete all old function
    */

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, kMajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, kMinorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

int InitGlad() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "InitFlag: failed to initialize GLAD\n";
        return -1;
    }

    return 0;
}

void CallbackSettings(GLFWwindow* window) {
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}


void RenderingCycle(GLFWwindow* window) {
    /*
        glfwWindowShouldClose - return true, if the GLFW window was closed
        glfwSwapBuffers       - swaps the front and rear window buffers
        glfwPollEvents        - checks, if any events are triggered
    */
    while(!glfwWindowShouldClose(window)) {
        ProcessInput(window);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(kLowerLeftX, kLowerLeftY, width, height);
}

void ProcessInput(GLFWwindow* window) {
    /*
        GLFW_PRESS - currently pressed
        GLFW_RELEASE - released
    */

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }
}
