#include <iostream>

#include "windows_gl.hpp"
#include "triangle_gl.hpp"

const int kWidth = 800;             // logical
const int kHeight = 600;            //  size

int main() {

    InitOpenGl();

    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "Triangle", NULL, NULL);
    if (!window) {
        std::cout << "glfwCreateWindow: window is nullptr!\n";
        return 0;
    }

    int phys_width = 0, phys_height = 0;
    glfwGetFramebufferSize(window, &phys_width, &phys_height);

    glfwMakeContextCurrent(window);

    InitGlad();

    glViewport(0, 0, phys_width, phys_height);  // configuring the rendering window

    ShowTriangle(window);

    glfwTerminate();                                        // cleaning resources

    return 0;
}


