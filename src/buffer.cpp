#include "buffer.h"
#include <webgpu/webgpu_cpp.h>

namespace Types {
  Buffer::Buffer(wgpu::Device device, wgpu::Queue queue, const void* data, unsigned int size, wgpu::BufferUsage usage) {
    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.size = size;
    bufferDesc.usage = usage | wgpu::BufferUsage::CopyDst;
    bufferDesc.mappedAtCreation = false;
    this->_buffer = device.CreateBuffer(&bufferDesc);
    this->_size = size;

    if (data != nullptr) {
      queue.WriteBuffer(this->_buffer, 0, data, bufferDesc.size);
    }
  }
}