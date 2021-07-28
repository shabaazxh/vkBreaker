#pragma once

#include <stdexcept>
#include <Vulkan/vulkan.h>
#include <vector>

class Framebuffers {
public:
	Framebuffers(VkDevice device,std::vector<VkImageView> swapChainImageViews, VkRenderPass renderPass, VkExtent2D swapChainExtent);
	void createFramebuffers();
	void createSingleFramebuffer(VkImageView imageView, VkRenderPass renderPass);
	std::vector<VkFramebuffer> getSwapChainFramebuffers() { return swapChainFramebuffers; }

	VkFramebuffer getSingleFramebuffer() { return frameBuffer;}
	VkImageView getSingleFramebufferImageView() { return frameBufferImage;}

private:
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkImageView> swapChainImageViews;
	VkRenderPass renderPass;
	VkExtent2D swapChainExtent;
	VkDevice device;

	VkImageView frameBufferImage;
	VkFramebuffer frameBuffer;
};