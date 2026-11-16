#include "appdef.h"
#include "buffer.h"
#include <webgpu/webgpu_cpp.h>
#include <stdexcept>

namespace Types {
  Buffer::Buffer(wgpu::Device device, wgpu::Queue queue, const void* data, unsigned int size, wgpu::BufferUsage usage) {
    if (data == nullptr) {
      throw std::invalid_argument("Data pointer cannot be nullptr");
    }

    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.size = size;
    bufferDesc.usage = usage | wgpu::BufferUsage::CopyDst;
    bufferDesc.mappedAtCreation = false;
    this->_buffer = device.CreateBuffer(&bufferDesc);
    this->_size = size;

    queue.WriteBuffer(this->_buffer, 0, data, bufferDesc.size);
  }

  VertexBuffer::VertexBuffer(wgpu::Device device, wgpu::Queue queue, const void* data, unsigned int size) {
    if (data == nullptr) {
      throw std::invalid_argument("Data pointer cannot be nullptr");
    }

    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.size = size;
    bufferDesc.usage = wgpu::BufferUsage::Vertex | wgpu::BufferUsage::CopyDst;
    bufferDesc.mappedAtCreation = false;
    this->_buffer = device.CreateBuffer(&bufferDesc);
    this->_size = size;
    this->_vertex_count = size / (sizeof(float) * 6);

    /*
      While struct Vertex may have a different size, the data provided to 
    */

    queue.WriteBuffer(this->_buffer, 0, data, bufferDesc.size);
  }
}