#include "buffer_manager.h"
#include "appdef.h"
#include <stdexcept>
#include <string>
#include <webgpu/webgpu_cpp.h>
#include <unordered_map>

namespace Types {
  void BufferManager::CreateBuffer(const std::string& name, unsigned int size, wgpu::BufferUsage usage, const void* data = nullptr) {
    if (size == 0) {
      throw std::invalid_argument("Size is zero");
    }

    if (this->_buffers.contains(name)) {
      throw std::runtime_error("Buffer " + name + " already exists");
    }

    wgpu::BufferDescriptor bufferDesc;
    bufferDesc.size = size;
    bufferDesc.usage = usage | wgpu::BufferUsage::CopyDst;
    bufferDesc.mappedAtCreation = false;
    wgpu::Buffer buffer = this->_device.CreateBuffer(&bufferDesc);

    App::log("Created Buffer " + name, App::LogType::INFO);

    this->_buffers[name] = buffer;

    if (data != nullptr) {
      this->_queue.WriteBuffer(buffer, 0, data, bufferDesc.size);

      App::log("Wrote " + std::to_string(size) + " bytes to Buffer " + name, App::LogType::INFO);
    }
  }

  wgpu::Buffer& BufferManager::GetBuffer(const std::string& name) {
    if (!this->_buffers.contains(name)) {
      throw std::invalid_argument("Buffer " + name + " does not exist");
    }

    return this->_buffers[name];
  }

  void BufferManager::WriteBuffer(const std::string& name, const void* data, unsigned int size, unsigned int offset) {
    if (size == 0) {
      throw std::invalid_argument("Size is zero");
    }

    if (data == nullptr) {
      throw std::invalid_argument("Data pointer is nullptr");
    }

    if (!this->_buffers.contains(name)) {
      throw std::invalid_argument("Buffer " + name + " does not exist");
    }

    const wgpu::Buffer& buffer = this->_buffers[name];

    if (offset + size > buffer.GetSize()) {
      throw std::runtime_error("Data cannot fit in buffer");
    }

    this->_queue.WriteBuffer(buffer, offset, data, size);

    App::log("Wrote " + std::to_string(size) + " bytes to Buffer " + name, App::LogType::INFO);
  }

  void BufferManager::RemoveBuffer(const std::string& name) {
    if (!this->_buffers.contains(name)) {
      throw std::invalid_argument("Buffer " + name + " does not exist");
    }

    this->_buffers[name].Destroy();
    
    App::log("Destroyed Buffer " + name, App::LogType::INFO);

    this->_buffers.erase(name);
  };
}