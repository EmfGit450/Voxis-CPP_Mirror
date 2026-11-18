#pragma once
#include <stdexcept>
#include <webgpu/webgpu_cpp.h>
#include <unordered_map>

namespace Types {
   class BufferManager {
      private:
        wgpu::Device _device;
        wgpu::Queue _queue;

        std::unordered_map<std::string, wgpu::Buffer> _buffers;

      public:
        BufferManager(wgpu::Device& device, wgpu::Queue queue): _device(device), _queue(queue) {}

        void CreateBuffer(const std::string& name, const void* data, unsigned int size, wgpu::BufferUsage usage);

        wgpu::Buffer& GetBuffer(const std::string& name);

        void WriteBuffer(const std::string& name, const void* data, unsigned int size, unsigned int offset);

        void RemoveBuffer(const std::string& name);
   };
}