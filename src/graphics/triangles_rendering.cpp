#include "triangles_rendering.hpp"
#include "parsing_shaders.hpp"

#include <cassert>

namespace {
    float last_mouse_x = 0.0F;
    float last_mouse_y = 0.0F;
    float sensitivity = 0.1F;
    bool first_mouse = true;
} // namespace 

render::ErrorType RenderTriangles(GLFWwindow* win, const std::vector<Triangle>& triangles) {
    assert(win);

    int float_counter = (render::kDimension + render::kColors) * render::kVertexes;
    size_t triangles_number = triangles.size();
    size_t data_cap = (sizeof(float) * float_counter) * triangles_number;
    std::vector<float> raw_data;                                    // convert triangle data to float
    raw_data.reserve(data_cap);

    InitData(raw_data, triangles);

    std::string vertex_str = ReadShader(render::kVertexPath);       // reade vertex shader
    std::string fragment_str = ReadShader(render::kFragmentPath);   // read fragment shader

    const char* vertex_ptr = vertex_str.c_str();
    const char* fragment_ptr = fragment_str.c_str();

    unsigned shader_program = LinkShaders(vertex_ptr, fragment_ptr); // setting shaders

    unsigned int vao = VaoSettings(raw_data);                       // setting AO and VBO

    RenderCycle(win, shader_program, vao, triangles_number);

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
    unsigned int vao = 0;
    glGenVertexArrays(render::kBuffCount, &vao);  // ge unique ID
    glBindVertexArray(vao);                       // VAO activation

    // создаем, привязываем VBO
    unsigned int vbo = 0;
    glGenBuffers(render::kBuffCount, &vbo);      // get unique ID
    glBindBuffer(GL_ARRAY_BUFFER, vbo);          // VBO activation

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

    return vao;
}

void RenderCycle(GLFWwindow* win, unsigned int shaderProgram, unsigned int VAO, size_t triangles_number) {
    assert(win);

    int start_index = 0;
    int vertex_count = triangles_number * render::kVertexes;
    int unbind = 0;
    float back_r = 0.0F, back_g = 0.0F, back_b = 0.0F, alpha = 1.0F;
    
    Camera camera = CameraSettings();

    glfwSetWindowUserPointer(win, &camera);                  // save info about camera in window

    while(glfwGetKey(win, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        glClearColor(back_r, back_g, back_b, alpha);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);      // activate shaders
        glBindVertexArray(VAO);           // bind VAO
        
        ProcessInput(win, camera);

        CoordinateTransform(shaderProgram, camera);

        glDrawArrays(GL_TRIANGLES, start_index, vertex_count);

        glBindVertexArray(unbind);         // unbind VAO

        glfwSwapBuffers(win);
        glfwPollEvents();
    }
}

void ProcessInput(GLFWwindow* win, Camera& camera) {
    assert(win);
    
    float current_time = glfwGetTime();
    camera.delta_time = current_time - camera.last_frame;
    camera.last_frame = current_time;
    float camera_speed = camera.speed * camera.delta_time;

    if (glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS) {
        camera.pos += camera_speed * camera.front;
    }
    if (glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS) {
        camera.pos -= camera_speed * camera.front;
    }
    if (glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS) {
        camera.pos += glm::normalize(glm::cross(camera.up, camera.front)) * camera_speed;
    }
    if (glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS) {
        camera.pos -= glm::normalize(glm::cross(camera.up, camera.front)) * camera_speed;
    }
}

Camera CameraSettings() {
    
    glm::vec3 camera_pos = glm::vec3(0.0F, 0.0F, 3.0F);
    glm::vec3 camera_front = glm::vec3(0.0, 0.0F, -1.0F);
    glm::vec3 up = glm::vec3(0.0F, 1.0F, 0.0F);
    
    float base_speed = 2.5F;
    Camera camera(camera_pos, camera_front, up, base_speed);
     
    return camera;
}

void CoordinateTransform(unsigned int shaderProgram, const Camera& camera) {

    glm::mat4 model = glm::mat4(1.0F);      // init unit matrix
    glm::vec3 rotational_axis = glm::vec3(1.0F, 0.0F, 0.0F);
    model = glm::rotate(model, glm::radians(render::kRotateAngle), rotational_axis);

    //glm::mat4 view = glm::mat4(1.0F);
    //glm::vec3 vector_offset = glm::vec3(0.0F, 0.0F, -3.0F);
    //view = glm::translate(view, vector_offset);
    glm::mat4 view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);

    glm::mat4 projection = glm::perspective(glm::radians(render::kFovy), render::kAspect,
                                        render::kNear, render::kFar);

    UpdateMatrix(shaderProgram, "model", model);
    UpdateMatrix(shaderProgram, "view", view);
    UpdateMatrix(shaderProgram, "projection", projection);
}

void UpdateMatrix(unsigned int shaderProgram, const char* name, glm::mat4& matrix) {
    assert(name);

    int mat_loc = glGetUniformLocation(shaderProgram, name);
    glUniformMatrix4fv(mat_loc, render::kOneMatrix, GL_FALSE, glm::value_ptr(matrix));
}

void MouseCallback(GLFWwindow* win, double xpos, double ypos) {
    assert(win);

    Camera* camera_ptr = static_cast<Camera*>(glfwGetWindowUserPointer(win));

    if (!camera_ptr) {
        return ;
    }
    
    if (first_mouse) {
        last_mouse_x = xpos;
        last_mouse_y = ypos;
        first_mouse = false;
    }

    float delta_x = (xpos - last_mouse_x) * sensitivity;
    float delta_y = (last_mouse_y - ypos) * sensitivity;
    last_mouse_x = xpos;
    last_mouse_y = ypos;
    
    Camera& camera = *camera_ptr;
    camera.yaw += delta_x;
    camera.pitch += delta_y;

    if (camera.pitch > 89.0F) {
        camera.pitch = 89.0F;
    } else if (camera.pitch < -89.0F) {
        camera.pitch = -89.0F;
    }
    float pitch_rad = glm::radians(camera.pitch), yaw_rad = glm::radians(camera.yaw);
    camera.front.x = std::cos(pitch_rad) * std::cos(yaw_rad);
    camera.front.y = std::sin(pitch_rad);
    camera.front.z = std::cos(pitch_rad) * std::sin(yaw_rad);
}

