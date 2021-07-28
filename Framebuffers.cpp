#include "Framebuffers.h"


Framebuffers::Framebuffers(VkDevice device, std::vector<VkImageView> swapChainImageViews, VkRenderPass renderPass, VkExtent2D swapChainExtent) {
	this->device = device;
	this->swapChainImageViews = swapChainImageViews;
	this->renderPass = renderPass;
	this->swapChainExtent = swapChainExtent;
}

void Framebuffers::createFramebuffers() {
	swapChainFramebuffers.resize(swapChainImageViews.size());

	//iterate through the views and create frame buffers from them

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		VkImageView attachments[] = {
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass; //give it the render pass here
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = attachments;
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}

	}
}

void Framebuffers::createSingleFramebuffer(VkImageView imageView, VkRenderPass renderPass) {

	VkImageView attachments[] = { imageView };

	VkFramebufferCreateInfo create_framebuffer;
	create_framebuffer.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	create_framebuffer.renderPass = renderPass;
	create_framebuffer.attachmentCount = 1;
	create_framebuffer.pAttachments = attachments;
	create_framebuffer.width = swapChainExtent.width;
	create_framebuffer.height = swapChainExtent.height;
	create_framebuffer.layers = 1;

	if(vkCreateFramebuffer(device, &create_framebuffer, nullptr, &frameBuffer) != VK_SUCCESS){
		throw std::runtime_error("failed to create single framebuffer!");
	}
}