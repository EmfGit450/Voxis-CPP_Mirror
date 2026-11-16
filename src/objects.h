#pragma once
#include "math.h"
#include <vector>

namespace Types {
  struct Vertex {
    Math::Vector3 position;
    Math::Color3 color;

    std::vector<float> raw() {
      return std::vector<float>{
        position.x, position.y, position.z,
        color.r, color.g, color.b
      };
    }
  };

  struct Mesh {
    std::vector<Vertex> vertices;
    int vertex_count;
  };
}