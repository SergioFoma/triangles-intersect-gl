#include "utility.hpp"
#include "triangles_rendering.hpp"

#include <iostream>

int main() {

    InitOpenGl();

    GLFWwindow* win = CreateWindow("triangle 3D");
    if (!win) {
        std::cout << "CreateWindow: returned null ptr!\n";
        CleanResources();                                        // cleaning resources
        return 0;
    }

    glfwMakeContextCurrent(win);

    if (InitGlad() != utility::ErrorType::kCorrect) {
        std::cout << "InitGlad: returned negative value!\n";
        CleanResources();                                        // cleaning resources
        return 0;
    }

    ConfigureViewing(win);

    Point p_1 = {-0.5f , -0.5f  , 0.0f};
    Point p_2 = {0.5f  , -0.5f  , 0.0f};
    Point p_3 = {0.0f  ,  0.5f  , 0.0f};
    Color white(1.0f, 1.0f, 1.0f);
    Triangle tr(p_1, p_2, p_3, white);

    std::vector<Triangle> triangles = {tr};

    render::ErrorType error_code = RenderTriangles(win, triangles);
    if (error_code != render::ErrorType::kCorrect) {
        std::cout << "RenderTriangles: return negative error code!\n";
    }

    CleanResources();                                        // cleaning resources

    return 0;
}


