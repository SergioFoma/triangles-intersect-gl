#include "triangles_rendering.hpp"
#include "parsing_shaders.hpp"

#include <assert.h>

render::ErrorType RenderTriangles(GLFWwindow* win, const std::vector<Triangle>& triangles) {
    assert(win);

    int float_counter = render::kDimension + render::kVertexes + render::kColors;
    size_t data_cap = (sizeof(float) * float_counter) * triangles.size();
    std::vector<float> raw_data;                                    // convert triangle data to float
    raw_data.reserve(data_cap);

    InitData(raw_data, triangles);

    std::string vertex_str = ReadShader(render::kVertexPath);       // reade vertex shader
    std::string fragment_str = ReadShader(render::kFragmentPath);   // read fragment shader

    const char* vertex_ptr = vertex_str.c_str();
    const char* fragment_ptr = fragment_str.c_str();

    unsigned shaderProgram = LinkShaders(vertex_ptr, fragment_ptr); // setting shaders

    unsigned int VAO = VaoSettings(raw_data);                       // setting AO and VBO

    RenderCycle(win, shaderProgram, VAO);

    return render::ErrorType::kCorrect;
}

void InitData(std::vector<float>& data, const std::vector<Triangle>& triangles) {

    for (const auto& tr: triangles) {
        const Color& color = tr.color;
        AddPointData(data, tr.p_1, color);
        AddPointData(data, tr.p_2, color);
        AddPointData(data, tr.p_3, color);
    }
}

void AddPointData(std::vector<float>& data, const Point& point, const Color& color) {
    data.push_back(point.x);
    data.push_back(point.y);
    data.push_back(point.z);
    data.push_back(color.r);
    data.push_back(color.g);
    data.push_back(color.b);
}

unsigned int VaoSettings(std::vector<float>& raw_data) {
    // create VAO
    unsigned int VAO = 0;
    glGenVertexArrays(render::kBuffCount, &VAO);  // ge unique ID
    glBindVertexArray(VAO);                     // VAO activation

    // создаем, привязываем VBO
    unsigned int VBO = 0;
    glGenBuffers(render::kBuffCount, &VBO);      // get unique ID
    glBindBuffer(GL_ARRAY_BUFFER, VBO);         // VBO activation

    // GL_ARRAY_BUFFER - the target buffer type, which indicates that he is used as an array of vertices

    // copy data from coordinates.data to GPU video memory
    glBufferData(GL_ARRAY_BUFFER, raw_data.size() * sizeof(float), raw_data.data(), GL_STATIC_DRAW);

    // settings for parsing data
    // GL_FALSE - don't need normalize data
    // multiplication coefficient = 2 * render::kDimension =
    // = 2 * 3 = 6, because we keep: x_1 y_1 z_1 r_1 g_1 b_1 x_2
    int mul_coeff = 2 * render::kDimension;
    glVertexAttribPointer(render::kZeroLocation, render::kDimension, GL_FLOAT, GL_FALSE,
                          mul_coeff * sizeof(float), (void*)0);
    glEnableVertexAttribArray(render::kZeroLocation);       // activate the zero slot
    glVertexAttribPointer(render::kFirstLocation, render::kDimension, GL_FLOAT, GL_FALSE,
                          mul_coeff * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(render::kFirstLocation);     // active the first slot

    glBindVertexArray(0);                   // unbind VAO

    return VAO;
}

void RenderCycle(GLFWwindow* win, unsigned int shaderProgram, unsigned int VAO) {
    int start_index = 0;
    int vertex_count = 3;
    int unbind = 0;

    while(!glfwWindowShouldClose(win)) {
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);      // activate shaders
        glBindVertexArray(VAO);           // bind VAO

        CoordinateTransform(shaderProgram);

        glDrawArrays(GL_TRIANGLES, start_index, vertex_count);

        glBindVertexArray(unbind);         // unbind VAO

        glfwSwapBuffers(win);
        glfwPollEvents();
    }
}

void CoordinateTransform(unsigned int shaderProgram) {

    glm::mat4 model = glm::mat4(1.0f);      // init unit matrix
    glm::vec3 rotational_axis = glm::vec3(1.0f, 0.0f, 0.0f);
    model = glm::rotate(model, glm::radians(render::kRotateAngle), rotational_axis);

    glm::mat4 view = glm::mat4(1.0f);
    glm::vec3 vector_offset = glm::vec3(0.0f, 0.0f, -3.0f);
    view = glm::translate(view, vector_offset);

    glm::mat4 projection = glm::perspective(glm::radians(render::kFovy), render::kAspect,
                                        render::kNear, render::kFar);

    UpdateMatrix(shaderProgram, "model", model);
    UpdateMatrix(shaderProgram, "view", view);
    UpdateMatrix(shaderProgram, "projection", projection);
}

void UpdateMatrix(unsigned int shaderProgram, const char* name, glm::mat4& matrix) {

    int matLoc = glGetUniformLocation(shaderProgram, name);
    glUniformMatrix4fv(matLoc, render::kOneMatrix, GL_FALSE, glm::value_ptr(matrix));
}
