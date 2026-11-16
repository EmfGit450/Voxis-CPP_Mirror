#include "renderer.h"
#include "window.h"
#include "appdef.h"
#include "objects.h"
#include "buffer.h"
#include <string_view>
#include <vector>

// 1. The WGSL Shader (Now expects data from a vertex buffer!)
const char* shaderSource = R"(
  struct VertexInput {
    @location(0) position: vec3<f32>,
    @location(1) color: vec3<f32>,
  };

  struct VertexOutput {
    @builtin(position) position: vec4<f32>,
    @location(0) color: vec3<f32>,
  };

  @vertex
  fn vs_main(in: VertexInput) -> VertexOutput {
    var out: VertexOutput;
    out.position = vec4<f32>(in.position, 1.0);
    out.color = in.color;
    return out;
  }

  @fragment
  fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
    return vec4<f32>(in.color, 1);
  }
)";

int main() {
  GLFWwindow* window = nullptr;

  {
    WindowContext ctx = InitWindow(800, 600, "Voxis");
    window = ctx.window;

    if (!ctx.device || !window) {
      App::log("Failed to initialize Voxis", App::LogType::ERROR);
      return 1;
    }

    // --- 3. CREATE THE VERTEX BUFFER ---
    std::vector<std::vector<float>> vertices = {
      Types::Vertex{Math::Vector3{-0.5f, 0.5f, 0.0f}, Math::Color3{1.0f, 0.0f, 0.0f}}.raw(),   // Top (Red)
      Types::Vertex{Math::Vector3{-0.5f, -0.5f, 0.0f}, Math::Color3{0.0f, 1.0f, 0.0f}}.raw(), // Bottom Left (Green)
      Types::Vertex{Math::Vector3{0.5f, -0.5f, 0.0f}, Math::Color3{0.0f, 0.0f, 1.0f}}.raw(), // Bottom Right (Blue)
    };

    unsigned int buffSize = vertices.size() * sizeof(Types::Vertex);

    Types::Buffer vertexBuffer(
      ctx.device,
      ctx.queue,
      vertices.data(),
      buffSize,
      wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst
    );

    // Upload the data to the GPU
    ctx.queue.WriteBuffer(vertexBuffer.GetRawBuffer(), 0, vertices.data(), buffSize);

    App::Renderer engine(ctx, shaderSource);

    // --- 7. THE RENDER LOOP ---
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();

      engine.DrawFrame();
    }
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  
  App::log("Exited successfully", App::LogType::INFO);
  return 0;
}