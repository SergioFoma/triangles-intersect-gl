#ifndef GEOMETRY_BUFFER_HPP_
#define GEOMETRY_BUFFER_HPP_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <stdexcept>

#include "prog_error.hpp"

class GeometryBuffer {
 public:
  GeometryBuffer(const GeometryBuffer& other) = delete;

  GeometryBuffer(GeometryBuffer&& other)
    : vertex_dim_(other.vertex_dim_),
      color_dim_(other.color_dim_),
      normal_dim_(other.normal_dim_),
      vertex_count_(other.vertex_count_) {

      vao_ = other.vao_;
      vbo_ = other.vbo_;

      other.vao_ = 0;
      other.vbo_ = 0;
  };

  explicit GeometryBuffer(const std::vector<float>& raw_data,
                          unsigned int vertex_dim, unsigned int color_dim,
                          unsigned int normal_dim, unsigned int vertex_count)
      : vertex_dim_(vertex_dim),
        color_dim_(color_dim),
        normal_dim_(normal_dim),
        vertex_count_(vertex_count) {

    glGenVertexArrays(kBuffCount, &vao_);
    glGenBuffers(kBuffCount, &vbo_);

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
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);

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

  void Draw(GLenum mode) const {
    glDrawArrays(mode, kStartIndex, vertex_count_);

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

  unsigned int GetVao() const { return vao_; }

  unsigned int GetVbo() const { return vbo_; }

  GeometryBuffer& operator=(const GeometryBuffer& other) = delete;

  GeometryBuffer& operator=(GeometryBuffer&& other) {
    if (this == &other) {
      return *this;
    }

    glDeleteBuffers(kBuffCount, &vbo_);
    glDeleteVertexArrays(kBuffCount, &vao_);

    vertex_dim_ = other.vertex_dim_;
    color_dim_ = other.color_dim_;
    normal_dim_ = other.normal_dim_;
    vertex_count_ = other.vertex_count_;

    vao_ = other.vao_;
    vbo_ = other.vbo_;

    other.vao_ = 0;
    other.vbo_ = 0;

    return *this;
  };

  ~GeometryBuffer() {
    glDeleteBuffers(kBuffCount, &vbo_);
    glDeleteVertexArrays(kBuffCount, &vao_);
  }

 private:
  unsigned int vertex_dim_ = 0;  // for one vertex
  unsigned int color_dim_ = 0;   // for one vertex
  unsigned int normal_dim_ = 0;  // for one vertex
  unsigned int vao_ = 0;
  unsigned int vbo_ = 0;
  unsigned int vertex_count_ = 0;
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
};

#endif
