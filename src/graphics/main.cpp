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

  if (InitGlad() != utility::ErrorType::kCorrect) {
    std::cout << "InitGlad: returned negative value!\n";
    CleanResources();
    return 0;
  }

  ConfigureViewing(win);

  Point p_1_1 = {-0.5F, -0.5F, 0.0F};
  Point p_1_2 = {0.5F, -0.5F, 0.0F};
  Point p_1_3 = {0.0F, 0.5F, 0.0F};
  Color color_1 = {1.0F, 0.5F, 0.31F};

  Point p_2_1 = {0.4F, 0.1F, -0.1F};
  Point p_2_2 = {0.9F, 0.4F, -0.3F};
  Point p_2_3 = {0.7F, 0.5F, -0.1F};
  Color color_2 = {0.5F, 0.5F, 0.5F};

  Triangle tr_1(p_1_1, p_1_2, p_1_3, color_1);
  Triangle tr_2(p_2_1, p_2_2, p_2_3, color_2);

  std::vector<Triangle> triangles = {tr_1, tr_2};

  render::ErrorType error_code = RenderTriangles(win, triangles);
  if (error_code != render::ErrorType::kCorrect) {
    std::cout << "RenderTriangles: return negative error code!\n";
  }

  CleanResources();

  return 0;
}
