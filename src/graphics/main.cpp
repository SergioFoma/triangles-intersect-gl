#include "triangles_rendering.hpp"
#include "utility.hpp"

#include <iostream>

int main() {

  InitOpenGl();

  GLFWwindow* win = CreateWindow("triangle 3D");
  if (!win) {
    std::cout << "CreateWindow: returned null ptr!\n";
    CleanResources();
    return 0;
  }

  glfwMakeContextCurrent(win);

  if (InitGlad() != utility::ErrorType::kCorrect) {
    std::cout << "InitGlad: returned negative value!\n";
    CleanResources();
    return 0;
  }

  ConfigureViewing(win);

  Point p_1 = {-0.5F, -0.5F, 0.0F};
  Point p_2 = {0.5F, -0.5F, 0.0F};
  Point p_3 = {0.0F, 0.5F, 0.0F};
  Color white(1.0F, 1.0F, 1.0F);
  Triangle tr_1(p_1, p_2, p_3, white);

  std::vector<Triangle> triangles = {tr_1};

  render::ErrorType error_code = RenderTriangles(win, triangles);
  if (error_code != render::ErrorType::kCorrect) {
    std::cout << "RenderTriangles: return negative error code!\n";
  }

  CleanResources();

  return 0;
}
