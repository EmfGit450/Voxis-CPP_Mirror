#include "window.h"
#include "appdef.h"
#include "objects.h"
#include "renderer.h"
#include <string_view>

namespace App {
  Renderer::Renderer(WindowContext& ctx, std::string_view shaderSource) : _ctx(ctx) {
    wgpu::ShaderSourceWGSL wgslDesc{};
    wgslDesc.code = shaderSource;

    wgpu::ShaderModuleDescriptor shaderDesc{};
    shaderDesc.nextInChain = &wgslDesc;
    wgpu::ShaderModule shaderModule = ctx.device.CreateShaderModule(&shaderDesc);

    //A vertex has a vec3 of position and a vec3 of color
    wgpu::VertexAttribute attributes[2] = {};
    attributes[0].format = wgpu::VertexFormat::Float32x3; // Position
    attributes[0].offset = 0;
    attributes[0].shaderLocation = 0;

    attributes[1].format = wgpu::VertexFormat::Float32x3; // Color
    attributes[1].offset = sizeof(float) * 3;
    attributes[1].shaderLocation = 1;

    wgpu::VertexBufferLayout vertexLayout{};
    vertexLayout.arrayStride = sizeof(Types::Vertex);
    vertexLayout.stepMode = wgpu::VertexStepMode::Vertex;
    vertexLayout.attributeCount = 2;
    vertexLayout.attributes = attributes;

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

    pipelineDesc.fragment = &fragmentState;
    pipelineDesc.primitive.topology = wgpu::PrimitiveTopology::TriangleList;

    this->_pipeline = ctx.device.CreateRenderPipeline(&pipelineDesc);

    App::log("Created render pipeline", App::LogType::INFO);
  }

  void Renderer::DrawFrame() {
    wgpu::SurfaceTexture surfaceTexture;
    this->_ctx.surface.GetCurrentTexture(&surfaceTexture);

    wgpu::CommandEncoder encoder = this->_ctx.device.CreateCommandEncoder();

    wgpu::RenderPassColorAttachment colorAttachment{};
    colorAttachment.view = surfaceTexture.texture.CreateView();
    colorAttachment.loadOp = wgpu::LoadOp::Clear;
    colorAttachment.storeOp = wgpu::StoreOp::Store;
    colorAttachment.clearValue = {0.05f, 0.05f, 0.1f, 1.0f};

    wgpu::RenderPassDescriptor renderPassDesc{};
    renderPassDesc.colorAttachmentCount = 1;
    renderPassDesc.colorAttachments = &colorAttachment;

    wgpu::RenderPassEncoder renderPass = encoder.BeginRenderPass(&renderPassDesc);

    renderPass.SetPipeline(this->_pipeline);

    for (wgpu::Buffer& buffer : this->vertexBuffers) {
      renderPass.SetVertexBuffer(0, buffer); // Set the buffer!
      renderPass.Draw(buffer.GetSize() / sizeof(Types::Vertex), 1, 0, 0);  
    }

    renderPass.End();

    wgpu::CommandBuffer command = encoder.Finish();
    this->_ctx.queue.Submit(1, &command);
    this->_ctx.surface.Present();
  }
};