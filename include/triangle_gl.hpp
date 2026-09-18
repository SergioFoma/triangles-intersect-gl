#ifndef TRIANGLE_GL_HPP_
#define TRIANGLE_GL_HPP_

#include <glad/glad.h>      // for ptr settings on functions
#include <GLFW/glfw3.h>     // lib for create windows, work with stdin and stdout, move cursor

// ========================= ATTENTION ========================
// #include glad lib It should go ahead of other libraries
// ============================================================

struct Point {
    float x = 0.0;
    float y = 0.0;
    float z = 0.0;

    Point(const float x, const float y, const float z)
        : x(x), y(y), z(z) {}
};

struct Triangle {
    Point p_1;
    Point p_2;
    Point p_3;

    Triangle(const Point& p_1, const Point& p_2, const Point& p_3)
        : p_1(p_1), p_2(p_2), p_3(p_3) {}
};

void ShowTriangle(GLFWwindow* window);

void RecordTrianglesData(std::vector<float>& coordinates, std::vector<Triangle>& triangles);

void Rendering(GLFWwindow* window, unsigned int shaderProgram, unsigned int VAO);

#endif
