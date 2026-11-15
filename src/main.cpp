#include "window.h"
#include "appdef.h"
#include "math.h"
#include <iostream>
#include <string_view>
#include <cstdint>
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
    return vec4<f32>(in.color, 1.0);
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
    std::vector<Math::Vertex> vertices = {
      {{0.0f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},   // Top (Red)
      {{-0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // Bottom Left (Green)
      {{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}   // Bottom Right (Blue)
    };

    wgpu::BufferDescriptor bufferDesc{};
    bufferDesc.size = vertices.size() * sizeof(Math::Vertex);
    bufferDesc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
    bufferDesc.mappedAtCreation = false;
    wgpu::Buffer vertexBuffer = ctx.device.CreateBuffer(&bufferDesc);

    // Upload the data to the GPU
    ctx.queue.WriteBuffer(vertexBuffer, 0, vertices.data(), bufferDesc.size);

    // --- 4. CREATE THE SHADER MODULE ---
    wgpu::ShaderSourceWGSL wgslDesc{};
    wgslDesc.code = shaderSource;

    wgpu::ShaderModuleDescriptor shaderDesc{};
    shaderDesc.nextInChain = &wgslDesc;
    wgpu::ShaderModule shaderModule = ctx.device.CreateShaderModule(&shaderDesc);

    // --- 5. DEFINE THE VERTEX BUFFER LAYOUT ---
    wgpu::VertexAttribute attributes[2] = {};
    attributes[0].format = wgpu::VertexFormat::Float32x3; // Position
    attributes[0].offset = 0;
    attributes[0].shaderLocation = 0;

    attributes[1].format = wgpu::VertexFormat::Float32x3; // Color
    attributes[1].offset = sizeof(float) * 3;
    attributes[1].shaderLocation = 1;

    wgpu::VertexBufferLayout vertexLayout{};
    vertexLayout.arrayStride = sizeof(Math::Vertex);
    vertexLayout.stepMode = wgpu::VertexStepMode::Vertex;
    vertexLayout.attributeCount = 2;
    vertexLayout.attributes = attributes;

    // --- 6. CREATE THE RENDER PIPELINE ---
    wgpu::RenderPipelineDescriptor pipelineDesc{};
    pipelineDesc.vertex.module = shaderModule;
    pipelineDesc.vertex.entryPoint = "vs_main";
    pipelineDesc.vertex.bufferCount = 1;
    pipelineDesc.vertex.buffers = &vertexLayout;

    wgpu::ColorTargetState colorTarget{};
    colorTarget.format = ctx.config.format;

    wgpu::FragmentState fragmentState{};
    fragmentState.module = shaderModule;
    fragmentState.entryPoint = "fs_main";
    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTarget;

    pipelineDesc.fragment = &fragmentState; // POINTER!
    pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;

    wgpu::RenderPipeline pipeline = ctx.device.CreateRenderPipeline(&pipelineDesc);

    App::log("Triangle pipeline created, Starting render loop...", App::LogType::INFO);

    // --- 7. THE RENDER LOOP ---
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();

      wgpu::SurfaceTexture surfaceTexture;
      ctx.surface.GetCurrentTexture(&surfaceTexture);

      wgpu::CommandEncoder encoder = ctx.device.CreateCommandEncoder();

      wgpu::RenderPassColorAttachment colorAttachment{};
      colorAttachment.view = surfaceTexture.texture.CreateView();
      colorAttachment.loadOp = wgpu::LoadOp::Clear;
      colorAttachment.storeOp = wgpu::StoreOp::Store;
      colorAttachment.clearValue = {0.05f, 0.05f, 0.1f, 1.0f};

      wgpu::RenderPassDescriptor renderPassDesc{};
      renderPassDesc.colorAttachmentCount = 1;
      renderPassDesc.colorAttachments = &colorAttachment;

      wgpu::RenderPassEncoder renderPass = encoder.BeginRenderPass(&renderPassDesc);
      
      // Bind the buffer and draw!
      renderPass.SetPipeline(pipeline);
      renderPass.SetVertexBuffer(0, vertexBuffer); // Set the buffer!
      renderPass.Draw(3, 1, 0, 0);                 // Draw 3 vertices
      
      renderPass.End();

      wgpu::CommandBuffer command = encoder.Finish();
      ctx.queue.Submit(1, &command);
      ctx.surface.Present();
    }
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  
  App::log("Exited successfully", App::LogType::INFO);
  return 0;
}