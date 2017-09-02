#ifndef GRAPHICS_PIPELINE_H
#define GRAPHICS_PIPELINE_H

#include <vulkan/vulkan.hpp>

namespace lpe
{
	class GraphicsPipeline
	{
		vk::PhysicalDevice physicalDevice;
		vk::Device device;

		vk::RenderPass renderPass;
		vk::DescriptorSetLayout descriptorSetLayout;
		vk::PipelineLayout pipelineLayout;
		vk::Pipeline graphicsPipeline;

	protected:
		vk::Format FindDepthFormat();
		vk::ShaderModule CreateShaderModule(const std::vector<char>& code);

		void CreateRenderPass(vk::Format swapChainImageFormat);
		void CreateDescriptorSetLayout();
		void CreateGraphicsPipeline(vk::Extent2D swapChainExtent);

	public:
		GraphicsPipeline() = default;
		~GraphicsPipeline();

		void Create(vk::PhysicalDevice physicalDevice, const vk::Device& device, vk::Format swapChainImageFormat, vk::Extent2D swapChainExtent);
	};
}

#endif