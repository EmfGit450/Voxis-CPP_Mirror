#pragma once
#include <webgpu/webgpu_cpp.h>

namespace Types {
  class Buffer {
    private:
      wgpu::Buffer _buffer;
      unsigned int _size;
    
    public:
      Buffer(wgpu::Device device, wgpu::Queue queue, const void* data, unsigned int size, wgpu::BufferUsage usage);

      Buffer(const Buffer& buffer) = delete;
      Buffer& operator=(const Buffer&) = delete;

      Buffer(Buffer&&) noexcept = default;
      Buffer& operator=(Buffer&&) noexcept = default;

      const wgpu::Buffer& GetRawBuffer() const { return _buffer; }
      unsigned int GetSize() const { return _size; }
  };
}