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

        void CreateBuffer(
          std::string name, 
          const void* data, unsigned int size, 
          wgpu::BufferUsage usage
        ) {
          if (data == nullptr) {
            throw std::invalid_argument("Data pointer cannot be nullptr");
          }

          if (this->_buffers.contains(name)) {
            throw std::runtime_error("Buffer " + name + " already exists");
          }

          wgpu::BufferDescriptor bufferDesc;
          bufferDesc.size = size;
          bufferDesc.usage = usage | wgpu::BufferUsage::CopyDst;
          bufferDesc.mappedAtCreation = false;
          wgpu::Buffer buffer = this->_device.CreateBuffer(&bufferDesc);

          this->_buffers[name] = buffer;

          this->_queue.WriteBuffer(buffer, 0, data, bufferDesc.size);
        }

        wgpu::Buffer& GetBuffer(std::string name) {
          if (!this->_buffers.contains(name)) {
            throw std::invalid_argument("Buffer " + name + " does not exist");
          }

          return this->_buffers[name];
        }
   };
}