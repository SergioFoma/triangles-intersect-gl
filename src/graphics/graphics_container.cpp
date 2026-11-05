
#include "graphics/graphics_container.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace {

void SetTextureParameters() {
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

graphics::TextureId LoadCubemap(const config::SkyboxConfig& skybox_con) {
  unsigned int texture_id = 0;
  glGenTextures(graphics::kOneTexture, &texture_id);
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);

  graphics::TextureId texture_id_ptr(reinterpret_cast<unsigned int*>(texture_id),
                                   graphics::TextureIdDeleter{});

  int width = 0, height = 0;
  int file_channels = 3;
  int level = 0, border = 0;

  const std::vector<std::string>& texture_sides = skybox_con.texture_sides;
  int sides_number = skybox_con.sides_number;

  for (int ind = 0; ind < sides_number; ++ind) {
    unsigned char* data = stbi_load(texture_sides[ind].c_str(), &width, &height,
                                   &file_channels, STBI_rgb);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + ind,
                 level, GL_RGB, width, height, border, GL_RGB,
                 GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
  }

  SetTextureParameters();

  if (glGetError()) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kRendering,
                                   "Error of load cubemap!",
                                   "LoadCubemap"};
    throw prog_error::ProgError(std::move(inf));
  }

  return texture_id_ptr;
}

graphics::SkyboxData CreateSkyboxObj(video_buff::GeometryBuffer& skybox_buff,
                                     const std::vector<float>& raw_data,
                                     const config::SkyboxConfig& skybox_con) {
  glm::mat4 model = glm::mat4(1.0F);
  glm::mat4 normal_mat = glm::mat4(1.0F);

  graphics::RenderObject skybox_obj = {
    skybox_buff,
    shader::Shader(skybox_con.shader_con.vert_path,
                   skybox_con.shader_con.frag_path),
    model, normal_mat, graphics::kBuffStartPos, graphics::kSkyboxVer};

  skybox_buff.LoadData(raw_data);

  graphics::TextureId texture_id = LoadCubemap(skybox_con);

  skybox_obj.shader.Use();
  skybox_obj.shader.SetInt("skybox", 0);
  skybox_obj.shader.Disable();

  graphics::SkyboxData skybox_data = {skybox_con, std::move(skybox_obj), std::move(texture_id)};

  int error_code = glGetError();
  if (error_code) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kRendering,
                                   "Error of creating SkyboxObj",
                                   "CreateSkyboxObj"};
    throw prog_error::ProgError(std::move(inf));
  }

  return skybox_data;
}
} // namespace

graphics::RenderObject graphics::GraphicsContainer::CreateTriangleObj(
  video_buff::GeometryBuffer& geom_buff_ptr,
  const config::ShaderConfig& shader_con,
  const shapes::ShapesNumber& shapes_number
) {
  glm::mat4 model = glm::mat4(1.0F);  // init unit matrix

  glm::mat3 normal_mat = glm::mat3(glm::transpose(glm::inverse(model)));

  graphics::RenderObject tr_obj = {
      geom_buff_ptr,
      shader::Shader(shader_con.vert_path, shader_con.frag_path),
      model, normal_mat, kBuffStartPos,
      shapes_number.triangles_number * kTrVertexes};

  int error_code = glGetError();
  if (error_code) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kRendering,
                                   "Error of creating triangle object",
                                   "CreateTrangleObj"};
    throw prog_error::ProgError(std::move(inf));
  }

  return tr_obj;
}

graphics::RenderObject graphics::GraphicsContainer::CreateLineObj(
  video_buff::GeometryBuffer& geom_buff_ptr,
  const config::ShaderConfig& shader_con,
  const shapes::ShapesNumber& shapes_number
) {
  glm::mat4 model = glm::mat4(1.0F);  // init unit matrix

  glm::mat3 normal_mat = glm::mat4(1.0F);

  graphics::RenderObject point_obj = {
      geom_buff_ptr,
      shader::Shader(shader_con.vert_path, shader_con.frag_path),
      model, normal_mat,
      kBuffStartPos + shapes_number.triangles_number * kTrVertexes,
      shapes_number.lines_number * kLineVertexes};

  int error_code = glGetError();
  if (error_code) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kRendering,
                                   "Error of creating line object",
                                   "CreateTrangleObj"};
    throw prog_error::ProgError(std::move(inf));
  }

  return point_obj;
}

graphics::RenderObject graphics::GraphicsContainer::CreatePointObj(
  video_buff::GeometryBuffer& geom_buff_ptr,
  const config::ShaderConfig& shader_con,
  const shapes::ShapesNumber& shapes_number
) {
  glm::mat4 model = glm::mat4(1.0F);  // init unit matrix

  glm::mat3 normal_mat = glm::mat4(1.0F);

  graphics::RenderObject line_obj = {
      geom_buff_ptr,
      shader::Shader(shader_con.vert_path, shader_con.frag_path),
      model, normal_mat,
      kBuffStartPos + shapes_number.triangles_number * kTrVertexes
      + shapes_number.lines_number * kLineVertexes,
      shapes_number.points_number};

  int error_code = glGetError();
  if (error_code) {
    prog_error::ErrorInfo inf = {prog_error::ErrorCode::kRendering,
                                   "Error of creating point object",
                                   "CreateTrangleObj"};
    throw prog_error::ProgError(std::move(inf));
  }

  return line_obj;
}

graphics::SkyboxData graphics::GraphicsContainer::CallSkyboxCreating(
  video_buff::GeometryBuffer& skybox_buff,
  const config::SkyboxConfig& skybox_con
) {
  std::vector<float> skybox_vertices = {
        // positions
        -1.0F,  1.0F, -1.0F,
        -1.0F, -1.0F, -1.0F,
         1.0F, -1.0F, -1.0F,
         1.0F, -1.0F, -1.0F,
         1.0F,  1.0F, -1.0F,
        -1.0F,  1.0F, -1.0F,

        -1.0F, -1.0F,  1.0F,
        -1.0F, -1.0F, -1.0F,
        -1.0F,  1.0F, -1.0F,
        -1.0F,  1.0F, -1.0F,
        -1.0F,  1.0F,  1.0F,
        -1.0F, -1.0F,  1.0F,

         1.0F, -1.0F, -1.0F,
         1.0F, -1.0F,  1.0F,
         1.0F,  1.0F,  1.0F,
         1.0F,  1.0F,  1.0F,
         1.0F,  1.0F, -1.0F,
         1.0F, -1.0F, -1.0F,

        -1.0F, -1.0F,  1.0F,
        -1.0F,  1.0F,  1.0F,
         1.0F,  1.0F,  1.0F,
         1.0F,  1.0F,  1.0F,
         1.0F, -1.0F,  1.0F,
        -1.0F, -1.0F,  1.0F,

        -1.0F,  1.0F, -1.0F,
         1.0F,  1.0F, -1.0F,
         1.0F,  1.0F,  1.0F,
         1.0F,  1.0F,  1.0F,
        -1.0F,  1.0F,  1.0F,
        -1.0F,  1.0F, -1.0F,

        -1.0F, -1.0F, -1.0F,
        -1.0F, -1.0F,  1.0F,
         1.0F, -1.0F, -1.0F,
         1.0F, -1.0F, -1.0F,
        -1.0F, -1.0F,  1.0F,
         1.0F, -1.0F,  1.0F
  };

  return CreateSkyboxObj(skybox_buff, skybox_vertices, skybox_con);
}
