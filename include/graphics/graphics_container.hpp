#ifndef GRAPHICS_CONTAINER_HPP_
#define GRAPHICS_CONTAINER_HPP_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "graphics/utility.hpp"
#include "graphics/shader.hpp"
#include "graphics/geometry_buffer.hpp"
#include "configs.hpp"
#include "shapes.hpp"

namespace graphics {

constexpr int kTrVertexes = 3;           // Point p_1, p_2, p_3
constexpr int kLineVertexes = 2;         // Point p_1, p_2
constexpr int kSkyboxVer = 36;           // skybox vertexes number
constexpr unsigned int kBuffStartPos = 0;
constexpr unsigned int kOneTexture = 1;

struct Color {
  float r = 0.0F;
  float g = 0.0F;
  float b = 0.0F;
};

class Camera {
 public:
  explicit Camera(const config::CameraConfig& camera)
      : pos_(camera.pos), front_(camera.front), up_(camera.up),
        speed_(camera.speed), sensitivity_(camera.mouse_sensitivity),
        fovy_(camera.fovy), aspect_(camera.aspect),
        near_plane_(camera.near_plane), far_plane_(camera.far_plane) {
    last_frame_ = glfwGetTime();

    projection_ = glm::perspective(glm::radians(fovy_), aspect_, near_plane_, far_plane_);
  }

  void SetPerspectiveMatrix(float new_aspect) {
    projection_ = glm::perspective(glm::radians(fovy_), new_aspect, near_plane_, far_plane_);
  }

  void Move(const utility::Window& win, float speed_coeff) {
    float current_time = glfwGetTime();
    delta_time_ = current_time - last_frame_;
    last_frame_ = current_time;
    float camera_speed = speed_coeff * speed_ * delta_time_;

    if (glfwGetKey(win.GetPtr(), GLFW_KEY_W) == GLFW_PRESS) {
        pos_ += camera_speed * front_;
      }
    if (glfwGetKey(win.GetPtr(), GLFW_KEY_S) == GLFW_PRESS) {
      pos_ -= camera_speed * front_;
    }
    if (glfwGetKey(win.GetPtr(), GLFW_KEY_A) == GLFW_PRESS) {
      pos_ +=
          glm::normalize(glm::cross(up_, front_)) * camera_speed;
    }
    if (glfwGetKey(win.GetPtr(), GLFW_KEY_D) == GLFW_PRESS) {
      pos_ -=
          glm::normalize(glm::cross(up_, front_)) * camera_speed;
    }

    view_ = glm::lookAt(pos_, pos_ + front_, up_);
  }

  void UpdateAngles(float delta_x, float delta_y) {
    yaw_ += delta_x;
    pitch_ += delta_y;

    if (pitch_ > 89.0F) {
      pitch_ = 89.0F;
    } else if (pitch_ < -89.0F) {
      pitch_ = -89.0F;
    }

    float pitch_rad = glm::radians(pitch_);
    float yaw_rad = glm::radians(yaw_);
    front_.x = std::cos(pitch_rad) * std::cos(yaw_rad);
    front_.y = std::sin(pitch_rad);
    front_.z = std::cos(pitch_rad) * std::sin(yaw_rad);
  }

  const glm::mat4& GetView() const {
    return view_;
  }
  const glm::mat4& GetProjection() const {
    return projection_;
  }
  const glm::vec3& GetPos() const {
    return pos_;
  }
  const glm::vec3& GetFront() const {
    return front_;
  }
  const glm::vec3& GetUp() const {
    return up_;
  }
  float GetSensitivity() const {
    return sensitivity_;
  }

 private:
  glm::mat4 view_ = glm::mat4(1.0F);
  glm::mat4 projection_ = glm::mat4(1.0F);
  glm::vec3 pos_;
  glm::vec3 front_;
  glm::vec3 up_;
  float speed_ = 0.05F;
  float delta_time_ = 0.0F;
  float last_frame_ = 0.0F;
  float yaw_ = -90.0F;
  float pitch_ = 0.0F;
  float sensitivity_ = 0.0F;
  float fovy_ = 0.0F;
  float aspect_ = 0.0F;
  float near_plane_ = 0.0F;
  float far_plane_ = 0.0F;
};

struct RenderObject {
  shader::Shader shader;
  glm::mat4 model;
  glm::mat3 normal_mat;
  size_t start_pos = 0;
  size_t count = 0;
};

struct ShapesObject {
  RenderObject tr_obj;
  RenderObject point_obj;
  RenderObject line_obj;
};

struct TextureIdDeleter {
  int one_texture = 1;

  void operator()(unsigned int* texture_id) const {
    if (!texture_id) {
      return ;
    }

    std::uintptr_t uint_id = reinterpret_cast<std::uintptr_t>(texture_id);
    unsigned int unint_id = static_cast<unsigned int>(uint_id);
    glDeleteTextures(one_texture, &unint_id);
  }
};

using TextureId = std::unique_ptr<unsigned int, TextureIdDeleter>;

struct SkyboxData {
  RenderObject obj;
  TextureId texture_id;

  SkyboxData(RenderObject obj, TextureId id)
    : obj(std::move(obj)),
      texture_id(std::move(id)) {}

  SkyboxData(const SkyboxData& other) = delete;

  SkyboxData(SkyboxData&& other) = default;

  unsigned int GetTextureId() const {
    std::uintptr_t uint_id = reinterpret_cast<std::uintptr_t>(texture_id.get());
    unsigned int unint_id = static_cast<unsigned int>(uint_id);

    return unint_id;
  }

  SkyboxData& operator=(const SkyboxData& other) = delete;

  SkyboxData& operator=(SkyboxData&& other) = default;

  ~SkyboxData() = default;
};

class GraphicsContainer {
 public:
  GraphicsContainer(const shapes::ShapesNumber& shapes_number,
                    const config::UserConfigs& user_configs,
                    video_buff::BufferManager& buffer_manager)
    : shapes_obj_{
        CreateTriangleObj(user_configs.GetTrShCon(),
                          shapes_number),
        CreatePointObj(user_configs.GetPointShCon(),
                                shapes_number),
        CreateLineObj(user_configs.GetLineShCon(),
                      shapes_number)
      },
      skybox_data_{CallSkyboxCreating(buffer_manager,
                                      user_configs.GetSkyboxCon())},
      camera_{user_configs.GetCameraCon()} {}

  const ShapesObject& GetShapesObj() const {
    return shapes_obj_;
  }

  Camera& GetCamera() {
    return camera_;
  }

  const SkyboxData& GetSkybox() const {
    return skybox_data_;
  }

 private:
  ShapesObject shapes_obj_;
  SkyboxData skybox_data_;
  Camera camera_;

  RenderObject CreateTriangleObj(const config::ShaderConfig& shader_con,
                                 const shapes::ShapesNumber& shapes_number);

  RenderObject CreateLineObj(const config::ShaderConfig& shader_con,
                             const shapes::ShapesNumber& shapes_number);

  RenderObject CreatePointObj(const config::ShaderConfig& shader_con,
                              const shapes::ShapesNumber& shapes_number);

  SkyboxData CallSkyboxCreating(video_buff::BufferManager& buffer_manager,
                                const config::SkyboxConfig& skybox_con);
};
} // namespace graphics




#endif
