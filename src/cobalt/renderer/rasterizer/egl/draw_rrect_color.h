// Copyright 2017 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef COBALT_RENDERER_RASTERIZER_EGL_DRAW_RRECT_COLOR_H_
#define COBALT_RENDERER_RASTERIZER_EGL_DRAW_RRECT_COLOR_H_

#include "cobalt/math/rect_f.h"
#include "cobalt/render_tree/color_rgba.h"
#include "cobalt/render_tree/rounded_corners.h"
#include "cobalt/renderer/rasterizer/egl/draw_object.h"

namespace cobalt {
namespace renderer {
namespace rasterizer {
namespace egl {

// Handles drawing a colored rounded rect.
class DrawRRectColor : public DrawObject {
 public:
  DrawRRectColor(GraphicsState* graphics_state,
                 const BaseState& base_state,
                 const math::RectF& rect,
                 const render_tree::RoundedCorners& corners,
                 const render_tree::ColorRGBA& color);

  void ExecuteUpdateVertexBuffer(GraphicsState* graphics_state,
      ShaderProgramManager* program_manager) OVERRIDE;
  void ExecuteRasterize(GraphicsState* graphics_state,
      ShaderProgramManager* program_manager) OVERRIDE;
  base::TypeId GetTypeId() const OVERRIDE;

 private:
  math::RectF rect_;
  render_tree::RoundedCorners corners_;
  uint32_t color_;

  uint8_t* vertex_buffer_;
};

}  // namespace egl
}  // namespace rasterizer
}  // namespace renderer
}  // namespace cobalt

#endif  // COBALT_RENDERER_RASTERIZER_EGL_DRAW_RRECT_COLOR_H_
