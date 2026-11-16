#pragma once
#include "window.h"
#include "buffer.h"
#include <vector>
#include <string_view>

namespace App {
  class Renderer {
    private:
      WindowContext& _ctx;

      wgpu::RenderPipeline _pipeline;

      std::vector<wgpu::Buffer> vertexBuffers;

    public:

      Renderer(WindowContext& ctx, std::string_view shaderSource);

      void DrawFrame();
  };
}