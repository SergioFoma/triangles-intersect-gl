#include <iostream>

#include "windows_gl.hpp"
#include "triangle_gl.hpp"

const int kWidth = 800;
const int kHeight = 600;

int main() {

    InitOpenGl();

    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "Triangle", NULL, NULL);
    if (!window) {
        std::cout << "glfwCreateWindow: window is nullptr!\n";
        return 0;
    }

    glfwMakeContextCurrent(window);

    InitGlad();

    glViewport(0, 0, kWidth, kHeight);  // configuring the rendering window

    ShowTriangle(window);

    glfwTerminate();                                        // cleaning resources

    return 0;
}


