#ifndef GEOMETRY_BUFFER_HPP_
#define GEOMETRY_BUFFER_HPP_

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <vector>

class GeometryBuffer {
 public:
  GeometryBuffer(const GeometryBuffer& other) = delete;

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
  }

  void Bind() const {
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  }

  void Unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, kUnbind);
    glBindVertexArray(kUnbind);
  }

  void Draw(GLenum mode) const {
    glDrawArrays(mode, kStartIndex, vertex_count_);
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
  }
};

#endif
