#ifndef GEOMETRY_BUFFER_HPP_
#define GEOMETRY_BUFFER_HPP_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <utility>
#include <memory>
#include <cstdint>
#include <vector>

#include "prog_error.hpp"

struct VertexBufferDeleter {
  static constexpr int kBuffCount = 1;

  void operator()(unsigned int* buffer_id) const {
      if (!buffer_id) {
        return ;
      }

      std::uintptr_t id = reinterpret_cast<std::uintptr_t>(buffer_id);
      unsigned int trimmed_id = static_cast<unsigned int>(id);
      glDeleteBuffers(kBuffCount, &trimmed_id);
  }
};

using VertexBuffer = std::unique_ptr<unsigned int, VertexBufferDeleter>;

struct VertexArrayDeleter {
  static constexpr int kBuffCount = 1;

  void operator()(unsigned int* array_id) const {
    if (!array_id) {
      return ;
    }

    std::uintptr_t id = reinterpret_cast<std::uintptr_t>(array_id);
    unsigned int trimmed_id = static_cast<unsigned int>(id);
    glDeleteVertexArrays(kBuffCount, &trimmed_id);
  }
};

using VertexArray = std::unique_ptr<unsigned int, VertexArrayDeleter>;

class GeometryBuffer {
 public:
  GeometryBuffer(const GeometryBuffer& other) = delete;

  GeometryBuffer(GeometryBuffer&& other) =  default;

  GeometryBuffer(const std::vector<float>& raw_data,
                          unsigned int vertex_dim, unsigned int color_dim,
                          unsigned int normal_dim)
    : vertex_dim_(vertex_dim), color_dim_(color_dim), normal_dim_(normal_dim) {

    unsigned int vertex_array_id = 0, vertex_buffer_id = 0;
    glGenVertexArrays(kBuffCount, &vertex_array_id);
    glGenBuffers(kBuffCount, &vertex_buffer_id);

    VertexArray vertex_array_ptr{
      reinterpret_cast<unsigned int*>(vertex_array_id),
      VertexArrayDeleter{}};
    vertex_array_ = std::move(vertex_array_ptr);

    VertexBuffer vertex_buffer_ptr{
      reinterpret_cast<unsigned int*>(vertex_buffer_id),
      VertexBufferDeleter{}};
    vertex_buffer_ = std::move(vertex_buffer_ptr);

    Bind();
    glBufferData(GL_ARRAY_BUFFER, raw_data.size() * sizeof(float),
                 raw_data.data(), GL_STATIC_DRAW);
    Unbind();

    if (glGetError()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kGeomBuff,
                                   "Error of creating geom buffer",
                                   "GeometryBuffer::GeometryBuffer"};
      throw prog_error::ProgError(std::move(inf));
    }
  }

  void Bind() const {
    unsigned int vertex_array_id = GetArrayId();
    unsigned int vertex_buffer_id = GetBufferId();

    glBindVertexArray(vertex_array_id);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_id);

    if (glGetError()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kGeomBuff,
                                   "Error of binding",
                                   "GeometryBuffer::Bind"};
      throw prog_error::ProgError(std::move(inf));
    }
  }

  void Unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, kUnbind);
    glBindVertexArray(kUnbind);

    if (glGetError()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kGeomBuff,
                                   "Error of undinding",
                                   "GeometryBuffer::Unbind"};
      throw prog_error::ProgError(std::move(inf));
    }
  }

  void Draw(GLenum mode, size_t start_pos, size_t vertex_count) const {
    glDrawArrays(mode, start_pos, vertex_count);

    if (glGetError()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kGeomBuff,
                                   "Error of drawing",
                                   "GeometryBuffer::Draw"};
      throw prog_error::ProgError(std::move(inf));
    }
  }

  void SetCoordinates(int location) const {
    SetAttribute(location, vertex_dim_, reinterpret_cast<void*>(0));
  }

  void SetColors(int location) const {
    SetAttribute(location, color_dim_,
                 reinterpret_cast<void*>(vertex_dim_ * sizeof(float)));
  }

  void SetNormal(int location) const {
    unsigned int offset = vertex_dim_ + color_dim_;
    SetAttribute(location, normal_dim_,
                 reinterpret_cast<void*>(offset * sizeof(float)));
  }

  unsigned int GetVao() const {
    return GetArrayId();
  }

  unsigned int GetVbo() const {
    return GetBufferId();
  }

  GeometryBuffer& operator=(const GeometryBuffer& other) = delete;

  GeometryBuffer& operator=(GeometryBuffer&& other) = default;

  ~GeometryBuffer() = default;

 private:
  unsigned int vertex_dim_ = 0;  // for one vertex
  unsigned int color_dim_ = 0;   // for one vertex
  unsigned int normal_dim_ = 0;  // for one vertex
  VertexArray vertex_array_;
  VertexBuffer vertex_buffer_;
  static constexpr unsigned int kUnbind = 0;
  static constexpr unsigned int kBuffCount = 1;
  static constexpr unsigned int kStartIndex = 0;

  void SetAttribute(unsigned int location, unsigned int att_dim,
                    void* start_pos) const {
    unsigned int mul_coeff = vertex_dim_ + color_dim_ + normal_dim_;

    glVertexAttribPointer(location, att_dim, GL_FLOAT, GL_FALSE,
                          mul_coeff * sizeof(float), start_pos);
    glEnableVertexAttribArray(location);

    if (glGetError()) {
      prog_error::ErrorInfo inf = {prog_error::ErrorCode::kGeomBuff,
                                   "Error of setting attributes",
                                   "GeometryBuffer::SetAttribute"};
      throw prog_error::ProgError(std::move(inf));
    }
  }

  unsigned int GetBufferId() const {
    std::uintptr_t vertex_buffer_id =
      reinterpret_cast<std::uintptr_t>(vertex_buffer_.get());
    unsigned int buffer_id = static_cast<unsigned int>(vertex_buffer_id);

    return buffer_id;
  }

  unsigned int GetArrayId() const {
    std::uintptr_t vertex_array_id =
      reinterpret_cast<std::uintptr_t>(vertex_array_.get());
    unsigned int array_id = static_cast<unsigned int>(vertex_array_id);

    return array_id;
  }
};

#endif
