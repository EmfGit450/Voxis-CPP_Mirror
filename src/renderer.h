#pragma once
#include "window.h"
#include <string_view>

namespace App {
  class Renderer {
    private:
      WindowContext& _ctx;

      wgpu::RenderPipeline _pipeline;

    public:

      Renderer(WindowContext& ctx, std::string_view shaderSource);

      void DrawFrame();
  };
}