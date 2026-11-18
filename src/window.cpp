#include "appdef.h"
#include "webgpu/webgpu_cpp.h"
#include "window.h"
#include <webgpu/webgpu_glfw.h>
#include <string_view>
#include <string>

WindowContext InitWindow(int width, int height, const char* title) {
  WindowContext ctx;

  if (!glfwInit()) {
    App::log("Failed to initialize GLFW", App::LogType::ERROR);
    return ctx;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  ctx.window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!ctx.window) {
    App::log("Failed to create GLFW window", App::LogType::ERROR);
    return ctx;
  }

  static constexpr auto kTimedWaitAny = wgpu::InstanceFeatureName::TimedWaitAny;
  wgpu::InstanceDescriptor instanceDesc{
    .requiredFeatureCount = 1,
    .requiredFeatures = &kTimedWaitAny
  };
  ctx.instance = wgpu::CreateInstance(&instanceDesc);
  
  ctx.surface = wgpu::glfw::CreateSurfaceForWindow(ctx.instance, ctx.window);

  wgpu::RequestAdapterOptions adapterOpts{};
  adapterOpts.compatibleSurface = ctx.surface;
  wgpu::Future adapterFuture = ctx.instance.RequestAdapter(
    &adapterOpts,
    wgpu::CallbackMode::WaitAnyOnly,
    [&ctx](wgpu::RequestAdapterStatus status, wgpu::Adapter a, wgpu::StringView message) {
      if (status == wgpu::RequestAdapterStatus::Success) {
        ctx.adapter = a;
      } else {
        // App::log takes a string_view. Since we need to concat, we make a std::string first.
        std::string errMsg = "Adapter failed: " + std::string(message.data, message.length);
        App::log(errMsg, App::LogType::ERROR);
      }
    }
  );
  ctx.instance.WaitAny(adapterFuture, UINT64_MAX);

  wgpu::Future deviceFuture = ctx.adapter.RequestDevice(
    nullptr,
    wgpu::CallbackMode::WaitAnyOnly,
    [&ctx](wgpu::RequestDeviceStatus status, wgpu::Device d, wgpu::StringView message) {
      if (status == wgpu::RequestDeviceStatus::Success) {
        ctx.device = d;
      } else {
        std::string errMsg = "Device failed: " + std::string(message.data, message.length);
        App::log(errMsg, App::LogType::ERROR);
      }
    }
  );
  ctx.instance.WaitAny(deviceFuture, UINT64_MAX);
  ctx.queue = ctx.device.GetQueue();

  ctx.config.device = ctx.device;
  ctx.config.width = width;
  ctx.config.height = height;
  ctx.config.presentMode = wgpu::PresentMode::Mailbox;

  wgpu::SurfaceCapabilities caps;
  ctx.surface.GetCapabilities(ctx.adapter, &caps);
  ctx.config.format = caps.formats[0];

  ctx.surface.Configure(&ctx.config);

  App::log("Successfully configured Dawn", App::LogType::INFO);
  return ctx;
}

void setPresentMode(WindowContext& ctx, wgpu::PresentMode presentMode) {
  ctx.config.presentMode = presentMode;
}