#include "commandPool.h"


CommandPool::CommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, std::optional<uint32_t> graphicsValue) {
	this->device = device;
	this->physicalDevice = physicalDevice;
	this->surface = surface;
	this->graphicsValue = graphicsValue;
}

void CommandPool::createCommandPool() {
	// NEEDS TO CHANGE THIS

	//QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = graphicsValue.has_value(); //Will be recording commands for drawing so we have chosen gtaphics family
	poolInfo.flags = 0;

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}



}