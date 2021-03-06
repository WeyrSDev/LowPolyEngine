#include "../include/UniformBuffer.h"
#include "../include/ModelsRenderer.h"
#include <glm/gtc/matrix_transform.hpp>

lpe::UniformBuffer::UniformBuffer(const UniformBuffer& other)
{
  this->device.reset(other.device.get());
  this->physicalDevice = other.physicalDevice;
  this->ubo = other.ubo;
  this->viewBuffer = other.viewBuffer;
  this->instanceBuffer = other.instanceBuffer;
}

lpe::UniformBuffer::UniformBuffer(UniformBuffer&& other)
{
  this->device.reset(other.device.get());
  other.device.release();

  this->physicalDevice = other.physicalDevice;
  this->ubo = other.ubo;
  this->viewBuffer = std::move(other.viewBuffer);
  this->instanceBuffer = std::move(other.instanceBuffer);
}

lpe::UniformBuffer& lpe::UniformBuffer::operator=(const UniformBuffer& other)
{
  this->device.reset(other.device.get());
  this->physicalDevice = other.physicalDevice;
  this->ubo = other.ubo;
  this->viewBuffer = other.viewBuffer;
  this->instanceBuffer = other.instanceBuffer;

  return *this;
}

lpe::UniformBuffer& lpe::UniformBuffer::operator=(UniformBuffer&& other)
{
  this->device.reset(other.device.get());
  other.device.release();

  this->physicalDevice = other.physicalDevice;
  this->ubo = other.ubo;
  this->viewBuffer = std::move(other.viewBuffer);
  this->instanceBuffer = std::move(other.instanceBuffer);

  return *this;
}

lpe::UniformBuffer::UniformBuffer(vk::PhysicalDevice physicalDevice,
                                  vk::Device* device,
                                  ModelsRenderer& modelsRenderer,
                                  const Camera& camera, 
                                  const Commands& commands)
  : physicalDevice(physicalDevice)
{
  this->device.reset(device);

  viewBuffer = {physicalDevice, device, sizeof(ubo)};
  instanceBuffer = { physicalDevice, device };
	
  
  Update(camera, modelsRenderer, commands);
}

lpe::UniformBuffer::~UniformBuffer()
{
  if(device)
  {
    device.release();
  }
}

void lpe::UniformBuffer::Update(const Camera& camera, ModelsRenderer& renderer, const Commands& commands)
{
  ubo.view = camera.GetView();
  ubo.projection = camera.GetPerspective();
  ubo.projection[1][1] *= -1;

  viewBuffer.CopyToBufferMemory(&ubo, sizeof(ubo));

  std::vector<InstanceData> instanceData = renderer.GetInstanceData();

  if (instanceData.empty())
    return;

	if (instanceData.size() * sizeof(InstanceData) != instanceBuffer.GetSize())
	{
		vk::DeviceSize size = instanceData.size() * sizeof(InstanceData);

		// totally dump and inefficient
    instanceBuffer.CreateStaged(commands, size, instanceData.data(), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
	}
  else
  {
    auto cmdBuffer = commands.BeginSingleTimeCommands();
    instanceBuffer.CopyStaged(cmdBuffer, instanceData.data());
    commands.EndSingleTimeCommands(cmdBuffer);
  }
}

std::vector<vk::DescriptorBufferInfo> lpe::UniformBuffer::GetDescriptors()
{
  return { viewBuffer.GetDescriptor(), instanceBuffer.GetDescriptor() };
}

void lpe::UniformBuffer::SetLightPosition(glm::vec3 light)
{
  ubo.lightPos = light;
}

vk::Buffer lpe::UniformBuffer::GetInstanceBuffer()
{
	return instanceBuffer.GetBuffer();
}