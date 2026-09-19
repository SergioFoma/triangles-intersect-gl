#ifndef TRIANGLES_RENDERING_HPP_
#define TRIANGLES_RENDERING_HPP_

#include "utility.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>

namespace render {
    const int kDimension = 3;                                   // R^3 (x, y, z)
    const int kVertexes = 3;                                    // Point p_1, p_2, p_3
    const int kColors = 3;                                      // (r, g, b)
    const float kFovy = 45.0f;                                  // Field of View
    const float kAspect = utility::kWidth / utility::kHeight;
    const float kNear = 0.1f ;                                  // zNear
    const float kFar = 100.0f;                                  // zFar
    const float kRotateAngle = -55.0f;                          // for rotate
    const std::string kVertexPath = "./shaders/shader.vert";    // vertex config
    const std::string kFragmentPath = "./shaders/shader.frag";  // fragment config
    const int kBuffCount = 1;
    const int kZeroLocation = 0;                                // location in shaders
    const int kFirstLocation = 1;                               // location in shaders
    const int kOneMatrix = 1;

    enum class ErrorType {
        kCorrect,
        kError
    };
} // namespace render

struct Point {
    float x;
    float y;
    float z;
};

struct Color {
    float r = 0.0;
    float g = 0.0;
    float b = 0.0;
};

struct Triangle {
    Point p_1;
    Point p_2;
    Point p_3;
    Color color;
};

render::ErrorType RenderTriangles(GLFWwindow* win, const std::vector<Triangle>& triangles);

void InitData(std::vector<float>& data, const std::vector<Triangle>& triangles);

void AddPointData(std::vector<float>& data, const Point& point, const Color& color);

unsigned int VaoSettings(std::vector<float>& raw_data);

void RenderCycle(GLFWwindow* win, unsigned int shaderProgram, unsigned int VAO);

void CoordinateTransform(unsigned int shaderProgram);

void UpdateMatrix(unsigned int shaderProgram, const char* name, glm::mat4& matrix);

#endif
