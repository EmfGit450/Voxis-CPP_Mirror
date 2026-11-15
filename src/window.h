#pragma once
#include <webgpu/webgpu_cpp.h>
#include <GLFW/glfw3.h>

struct WindowContext {
  GLFWwindow* window = nullptr;
  wgpu::Instance instance = nullptr;
  wgpu::Adapter adapter = nullptr;
  wgpu::Device device = nullptr;
  wgpu::Surface surface = nullptr;
  wgpu::Queue queue = nullptr;
  wgpu::SurfaceConfiguration config = {};
};

WindowContext InitWindow(int width, int height, const char* title);

void CleanupWindow(WindowContext& ctx);