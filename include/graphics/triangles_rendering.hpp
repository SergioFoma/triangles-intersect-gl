#ifndef TRIANGLES_RENDERING_HPP_
#define TRIANGLES_RENDERING_HPP_


#include <vector>
#include <string>
#include <memory>
#include <cstdint>

#include "geometry_buffer.hpp"
#include "shader.hpp"
#include "utility.hpp"
#include "configs.hpp"
#include "graphics/graphics_container.hpp"

namespace render {

enum class ErrorType { kCorrect, kError };

ErrorType RenderTriangles(utility::Window& win,
                                  video_buff::BufferManager& buff_manager,
                                  graphics::GraphicsContainer& graphics_container,
                                  const config::UserConfigs& configs,
                                  const std::vector<float>& triangles);
}  // namespace render

#endif
