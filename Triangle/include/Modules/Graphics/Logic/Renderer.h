#pragma once
#include <vector>
#include <chrono>
#include <memory>
#include <windows.h>
#include <vulkan/vulkan.hpp>
#include <glm/detail/type_vec2.hpp>

#include "Modules/Graphics/Logic/Swapchain.h"
#include "Modules/Graphics/VulkanWrappers/VulkanContext.h"

namespace Graphics
{
	struct WindowsContext
	{
		HINSTANCE hInstance;
		HWND window;
		std::string name;
		uint32_t width;
		uint32_t height;
	};

	class Renderer
	{
	public:
		Renderer(WindowsContext context);

		void prepare();
		void submit(vk::CommandBuffer& buffer) const;
		void present() const;

		std::shared_ptr<VulkanContext> context;
		std::shared_ptr<VulkanSwapChain> swapchain;
		std::shared_ptr<vk::Queue> queue;

		vk::Format depthFormat;
		vk::Format colorformat = vk::Format::eB8G8R8A8Unorm;

		vk::CommandPool                cmdPool;
		vk::RenderPass                 renderPass;

		vk::PipelineCache              pipelineCache;
		std::vector<vk::Framebuffer>   frameBuffers;

		uint32_t getCurrentBuffer() const;

	private:
		struct
		{
			vk::Image        image;
			vk::DeviceMemory mem;
			vk::ImageView    view;
		} depthStencil;

		const glm::vec2 SCREEN_DIMENSIONS;

		void submitPrePresentBarrier(vk::Image image) const;
		void submitPostPresentBarrier(vk::Image image) const;

		void prepareDepthStencil(uint32_t width, uint32_t height);
		void prepareRenderPass();
		void preparePipelineCache();
		void prepareFramebuffers(uint32_t width, uint32_t height);
		uint32_t getMemoryPropertyIndex(vk::MemoryPropertyFlags flag, vk::MemoryRequirements requirements) const;

		uint32_t currentBuffer = 0;

		vk::Semaphore          presentComplete;
		vk::Semaphore          renderComplete;

		vk::CommandBuffer      postPresentCmdBuffer;
		vk::CommandBuffer      prePresentCmdBuffer;
	};
}
