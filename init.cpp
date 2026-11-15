#include <webgpu/webgpu_cpp.h>
#include <webgpu/webgpu_glfw.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdint>
#include <string_view>

int main() {
  // 1. Initialize GLFW and create a window
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return 1;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // We are using WebGPU, not OpenGL
  GLFWwindow* window = glfwCreateWindow(800, 600, "Voxis", nullptr, nullptr);
  if (!window) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return 1;
  }

  // --- START WEBGPU SCOPE ---
  // Everything WebGPU-related is inside this block. 
  // When the block ends, all GPU resources are safely destroyed 
  // BEFORE GLFW is terminated.
  {
    // 2. Create WebGPU Instance with TimedWaitAny support
    static constexpr auto kTimedWaitAny = wgpu::InstanceFeatureName::TimedWaitAny;
    wgpu::InstanceDescriptor instanceDesc{
      .requiredFeatureCount = 1,
      .requiredFeatures = &kTimedWaitAny
    };
    wgpu::Instance instance = wgpu::CreateInstance(&instanceDesc);
    
    if (!instance) {
      std::cerr << "Failed to create WebGPU instance" << std::endl;
      return 1;
    }

    // 3. Create the Surface from the GLFW window
    wgpu::Surface surface = wgpu::glfw::CreateSurfaceForWindow(instance, window);
    if (!surface) {
      std::cerr << "Failed to create WebGPU surface" << std::endl;
      return 1;
    }

    // 4. Request the Adapter (The physical GPU)
    wgpu::Adapter adapter;
    wgpu::RequestAdapterOptions adapterOpts{};
    adapterOpts.compatibleSurface = surface;

    wgpu::Future adapterFuture = instance.RequestAdapter(
      &adapterOpts,
      wgpu::CallbackMode::WaitAnyOnly,
      [&adapter](wgpu::RequestAdapterStatus status, wgpu::Adapter a, wgpu::StringView message) {
        if (status == wgpu::RequestAdapterStatus::Success) {
          adapter = a;
        } else {
          std::cerr << "Adapter failed: " << std::string_view(message.data, message.length) << std::endl;
        }
      }
    );
    instance.WaitAny(adapterFuture, UINT64_MAX);

    if (!adapter) {
      std::cerr << "Failed to get adapter!" << std::endl;
      return 1;
    }

    // 5. Request the Device (The logical connection to the GPU)
    wgpu::Device device;
    wgpu::Future deviceFuture = adapter.RequestDevice(
      nullptr,
      wgpu::CallbackMode::WaitAnyOnly,
      [&device](wgpu::RequestDeviceStatus status, wgpu::Device d, wgpu::StringView message) {
        if (status == wgpu::RequestDeviceStatus::Success) {
          device = d;
        } else {
          std::cerr << "Device failed: " << std::string_view(message.data, message.length) << std::endl;
        }
      }
    );
    instance.WaitAny(deviceFuture, UINT64_MAX);

    if (!device) {
      std::cerr << "Failed to get device!" << std::endl;
      return 1;
    }

    // 6. Configure the Surface
    wgpu::SurfaceConfiguration config{};
    config.device = device;
    config.width = 800;
    config.height = 600;
    config.presentMode = wgpu::PresentMode::Fifo;

    wgpu::SurfaceCapabilities caps;
    if (surface.GetCapabilities(adapter, &caps) != wgpu::Status::Success) {
      std::cerr << "Failed to get surface capabilities!" << std::endl;
      return 1;
    }
    config.format = caps.formats[0];

    surface.Configure(&config);

    std::cout << "Successfully configured WebGPU! Starting render loop..." << std::endl;

    // 7. The Render Loop
    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();

      wgpu::SurfaceTexture surfaceTexture;
      surface.GetCurrentTexture(&surfaceTexture);

      wgpu::CommandEncoder encoder = device.CreateCommandEncoder();

      wgpu::RenderPassColorAttachment colorAttachment{};
      colorAttachment.view = surfaceTexture.texture.CreateView();
      colorAttachment.loadOp = wgpu::LoadOp::Clear;
      colorAttachment.storeOp = wgpu::StoreOp::Store;
      colorAttachment.clearValue = {0.1f, 0.2f, 0.3f, 1.0f}; // Dark blue/grey

      wgpu::RenderPassDescriptor renderPassDesc{};
      renderPassDesc.colorAttachmentCount = 1;
      renderPassDesc.colorAttachments = &colorAttachment;

      wgpu::RenderPassEncoder renderPass = encoder.BeginRenderPass(&renderPassDesc);
      renderPass.End();

      wgpu::CommandBuffer command = encoder.Finish();
      device.GetQueue().Submit(1, &command);

      surface.Present();
    }
  } 
  // --- END WEBGPU SCOPE ---
  // The instance, surface, adapter, and device are destroyed right here.

  // 8. Cleanup GLFW safely
  glfwDestroyWindow(window);
  glfwTerminate();
  
  std::cout << "Exited cleanly." << std::endl;
  return 0;
}