#include <webgpu/webgpu_cpp.h>
#include <GLFW/glfw3.h>
#include <webgpu/webgpu_glfw.h> // Dawn's GLFW helper header
#include <iostream>

int main() {
    // 1. Initialize GLFW and create a window
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return 1;
    }

    // Tell GLFW not to create an OpenGL context, as we're using WebGPU
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Voxis - WebGPU", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return 1;
    }

    // 2. Create a WebGPU instance
    wgpu::Instance instance = wgpu::CreateInstance();
    if (!instance) {
        std::cerr << "Failed to create WebGPU instance" << std::endl;
        return 1;
    }

    // 3. Create the WebGPU surface from the GLFW window
    //    This function is likely provided by dawn::webgpu_glfw
    wgpu::Surface surface = wgpu::glfw::CreateSurfaceForWindow(instance, window);
    if (!surface) {
        std::cerr << "Failed to create WebGPU surface" << std::endl;
        return 1;
    }

    // --- From here, your WebGPU rendering setup (adapter, device, pipeline) begins ---

    std::cout << "Successfully created GLFW window and WebGPU surface!" << std::endl;

    wgpu::Adapter adapter;
    wgpu::Device device;
    // (You'll need to request an adapter and device here first)
    
    // Configure the surface (swap chain)
    wgpu::SurfaceConfiguration config{};
    config.device = device;

    wgpu::SurfaceCapabilities caps;
    wgpu::Status status = surface.GetCapabilities(adapter, &caps);
    
    if (status != wgpu::Status::Success) {
        std::cerr << "Failed to get surface capabilities!" << std::endl;
        return 1;
    }
    
    config.format = caps.formats[0]; // Usually BGRA8Unorm or similar
    config.width = 800;
    config.height = 600;
    config.presentMode = wgpu::PresentMode::Fifo;

    surface.Configure(&config);

    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();

      // // 1. Get current texture
      // wgpu::SurfaceTexture surfaceTexture;
      // surface.GetCurrentTexture(&surfaceTexture);

      // // 2. Create command encoder
      // wgpu::CommandEncoder encoder = device.CreateCommandEncoder();

      // // 3. Render Pass setup
      // wgpu::RenderPassColorAttachment colorAttachment{};
      // colorAttachment.view = surfaceTexture.texture.CreateView();
      // colorAttachment.loadOp = wgpu::LoadOp::Clear;
      // colorAttachment.storeOp = wgpu::StoreOp::Store;
      // colorAttachment.clearValue = {0.1f, 0.2f, 0.3f, 1.0f}; // Dark grey-blue

      // wgpu::RenderPassDescriptor renderPassDesc{};
      // renderPassDesc.colorAttachmentCount = 1;
      // renderPassDesc.colorAttachments = &colorAttachment;

      // wgpu::RenderPassEncoder renderPass = encoder.BeginRenderPass(&renderPassDesc); // PascalCase
      // renderPass.End(); // PascalCase

      // // 4. Submit to queue
      // wgpu::CommandBuffer command = encoder.Finish(); // PascalCase
      // device.GetQueue().Submit(1, &command); // PascalCase

      // // 5. Present
      // surface.Present(); // PascalCase
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}