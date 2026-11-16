#pragma once
#include "math.h"
#include <vector>

namespace Types {
  struct Vertex {
    Math::Vector3 position;
    Math::Color3 color;
  };

  struct Mesh {
    std::vector<Vertex> vertices;
  };

  
  class Buffer {
    
  }
}