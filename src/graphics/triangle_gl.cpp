#include <iostream>
#include <vector>

#include "triangle_gl.hpp"

const int kBufCount = 1;
const int kLineCount = 1;
const int kLayoutId = 0;
const int kDimension = 3;                                   // (x, y, z)

const char* vertexShaderSource = "#version 410 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}";

const char* fragmentShaderSource = "#version 410 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}";

void ShowTriangle(GLFWwindow* window) {

    Point p_1 = {-0.5f , -0.5f  , 0.0f};
    Point p_2 = {0.5f  , -0.5f  , 0.0f};
    Point p_3 = {0.0f  ,  0.5f  , 0.0f};

    Triangle tr(p_1, p_2, p_3);
    std::vector<Triangle> triangles = {tr};
    std::vector<float> coordinates;
    RecordTrianglesData(coordinates, triangles);

    // 2. create and compile shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, kLineCount, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, kLineCount, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // .. --> Vertex Shader --> Fragment Shader --> ...
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);       // no need, because we've already linked before.
    glDeleteShader(fragmentShader);     // no need, because we've already linked before.

    // VBO - raw vertex data directly in GPU video memory (buffer)
    // VAO - how the data from the VBO is mapped to the vertex attributes in the shader.

    // create VAO
    unsigned int VAO = 0;
    glGenVertexArrays(kBufCount, &VAO);         // ge unique ID
    glBindVertexArray(VAO);                     // VAO activation

    // создаем, привязываем VBO
    unsigned int VBO = 0;
    glGenBuffers(kBufCount, &VBO);              // get unique ID
    glBindBuffer(GL_ARRAY_BUFFER, VBO);         // VBO activation

    // GL_ARRAY_BUFFER - the target buffer type, which indicates that he is used as an array of vertices

    // copy data from coordinates.data to GPU video memory
    glBufferData(GL_ARRAY_BUFFER, coordinates.size() * sizeof(float), coordinates.data(), GL_STATIC_DRAW);

    // settings for parsing data
    // GL_FALSE - don't need normalize data
    glVertexAttribPointer(kLayoutId, kDimension, GL_FLOAT, GL_FALSE, kDimension * sizeof(float), (void*)0);
    glEnableVertexAttribArray(kLayoutId);  // activate the zero slot

    glBindVertexArray(0);                   // unbind VAO

    Rendering(window, shaderProgram, VAO);
}


void RecordTrianglesData(std::vector<float>& coordinates, std::vector<Triangle>& triangles) {

    for (auto& tr: triangles) {
        Point& p_1 = tr.p_1;
        Point& p_2 = tr.p_2;
        Point& p_3 = tr.p_3;

        coordinates.push_back(p_1.x);
        coordinates.push_back(p_1.y);
        coordinates.push_back(p_1.z);
        coordinates.push_back(p_2.x);
        coordinates.push_back(p_2.y);
        coordinates.push_back(p_2.z);
        coordinates.push_back(p_3.x);
        coordinates.push_back(p_3.y);
        coordinates.push_back(p_3.z);
    }
}

void Rendering(GLFWwindow* window, unsigned int shaderProgram, unsigned int VAO) {
    while(!glfwWindowShouldClose(window)) {

        //  отрисовка
        glUseProgram(shaderProgram);  // Активируем шейдеры
        glBindVertexArray(VAO);       // Восстанавливаем настройки

        // Рисуем: тип примитива, с какого индекса начать, сколько вершин нарисовать
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Отвязываем обратно
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
