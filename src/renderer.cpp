#pragma once
#include "window.h"
#include "appdef.h"
#include "math.h"
#include <vector>

namespace App {
  class Renderer {
    private:
      WindowContext& _ctx;

      std::vector<Math::Vertex> vertices;
    
    public:
      Renderer(WindowContext& ctx) : _ctx(ctx) {}

      void InitVertexBuffer() {
        
      }
  };
}