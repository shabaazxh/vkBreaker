#pragma once


#include <Vulkan/vulkan.h>
#include <stdexcept>

class RenderPass {
public:
	RenderPass(VkDevice device, VkFormat swapChainImageFormat);
	void createRenderPass();

	VkRenderPass getRenderPass() { return renderPass; }

private:
	VkRenderPass renderPass;
	VkFormat swapChainImageFormat;
	VkDevice device;


};