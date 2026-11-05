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

struct Camera {
  glm::mat4 view = glm::mat4(1.0F);
  glm::mat4 projection = glm::mat4(1.0F);
  glm::vec3 pos;
  glm::vec3 front;
  glm::vec3 up;
  float speed = 0.05F;
  float delta_time = 0.0F;
  float last_frame = 0.0F;
  float yaw = -90.0F;
  float pitch = 0.0F;
  float sensitivity = 0.0F;
  float fovy = 0.0F;
  float aspect = 0.0F;
  float near_plane = 0.0F;
  float far_plane = 0.0F;

  explicit Camera(const config::CameraConfig& camera)
      : pos(camera.pos), front(camera.front), up(camera.up),
        speed(camera.speed), sensitivity(camera.mouse_sensitivity),
        fovy(camera.fovy), aspect(camera.aspect),
        near_plane(camera.near_plane), far_plane(camera.far_plane) {
    last_frame = glfwGetTime();

    projection = glm::perspective(glm::radians(fovy), aspect, near_plane, far_plane);
  }

  void UpdateCameraMatrix() {
    view = glm::lookAt(pos, pos + front, up);
  }

  void SetPerspectiveMatrix(float new_aspect) {
    projection = glm::perspective(glm::radians(fovy), new_aspect, near_plane, far_plane);
  }
};

struct RenderObject {
  video_buff::GeometryBuffer& geom_buff;
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
  config::SkyboxConfig config;
  RenderObject obj;
  TextureId texture_id;

  SkyboxData(config::SkyboxConfig config, RenderObject obj, TextureId id)
    : config(std::move(config)), obj(std::move(obj)),
      texture_id(std::move(id)) {}

  SkyboxData(const SkyboxData& other) = delete;

  SkyboxData(SkyboxData&& other) = default;

  unsigned int GetTextureId() const {
    std::uintptr_t uint_id = reinterpret_cast<std::uintptr_t>(texture_id.get());
    unsigned int unint_id = static_cast<unsigned int>(uint_id);

    return unint_id;
  }

  SkyboxData& operator=(const SkyboxData& other) = delete;

  SkyboxData& operator=(SkyboxData&& other) = delete;

  ~SkyboxData() = default;
};

class GraphicsContainer {
 public:
  GraphicsContainer(const shapes::ShapesNumber& shapes_number,          // how to get at the start of program ?
                    const config::UserConfigs& user_configs,
                    video_buff::BufferManager& buffer_manager)
    : shapes_obj_{
        CreateTriangleObj(buffer_manager.GetVideoBuff(),
                                user_configs.tr_shader_con,
                                shapes_number),
        CreateLineObj(buffer_manager.GetVideoBuff(),
                              user_configs.line_shader_con,
                              shapes_number),
        CreatePointObj(buffer_manager.GetVideoBuff(),
                                user_configs.point_shader_con,
                                shapes_number)
      },
      skybox_data_{CallSkyboxCreating(buffer_manager.GetSkyboxBuff(),
                                      user_configs.skybox_con)},
      camera_{user_configs.camera_con} {}

    //shapes_obj_ = {std::move(tr_obj), std::move(point_obj), std::move(line_obj)};

    //skybox_data_ = CallSkyboxCreating(buffer_manager.GetSkyboxBuff(),
    //                                  user_configs.skybox_con);

  ShapesObject& GetShapesObj() {
    return shapes_obj_;
  }

  Camera& GetCamera() {
    return camera_;
  }

  SkyboxData& GetSkybox() {
    return skybox_data_;
  }

 private:
  ShapesObject shapes_obj_;
  SkyboxData skybox_data_;
  Camera camera_;

  RenderObject CreateTriangleObj(video_buff::GeometryBuffer& geom_buff_ptr,
                                 const config::ShaderConfig& shader_con,
                                 const shapes::ShapesNumber& shapes_number);

  RenderObject CreateLineObj(video_buff::GeometryBuffer& geom_buff_ptr,
                             const config::ShaderConfig& shader_con,
                             const shapes::ShapesNumber& shapes_number);

  RenderObject CreatePointObj(video_buff::GeometryBuffer& geom_buff_ptr,
                              const config::ShaderConfig& shader_con,
                              const shapes::ShapesNumber& shapes_number);

  SkyboxData CallSkyboxCreating(video_buff::GeometryBuffer& skybox_buff,
                                const config::SkyboxConfig& skybox_con);
};
} // namespace graphics




#endif
