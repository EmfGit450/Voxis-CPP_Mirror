#include "appdef.h"
#include "window.h"
#include <iostream>

int main() {
  GLFWwindow* window = nullptr;

  {
    WindowContext ctx = InitWindow(800, 600, "Voxis");
    window = ctx.window; // Save the window pointer so we can close it later

    if (!ctx.device || !window) {
      std::cerr << "Failed to initialize Voxis!" << std::endl;
      return 1;
    }

    // The Game Loop
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();

      wgpu::SurfaceTexture surfaceTexture;
      ctx.surface.GetCurrentTexture(&surfaceTexture);

      wgpu::CommandEncoder encoder = ctx.device.CreateCommandEncoder();

      wgpu::RenderPassColorAttachment colorAttachment{};
      colorAttachment.view = surfaceTexture.texture.CreateView();
      colorAttachment.loadOp = wgpu::LoadOp::Clear;
      colorAttachment.storeOp = wgpu::StoreOp::Store;
      colorAttachment.clearValue = {0.1f, 0.2f, 0.3f, 1.0f};

      wgpu::RenderPassDescriptor renderPassDesc{};
      renderPassDesc.colorAttachmentCount = 1;
      renderPassDesc.colorAttachments = &colorAttachment;

      wgpu::RenderPassEncoder renderPass = encoder.BeginRenderPass(&renderPassDesc);
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