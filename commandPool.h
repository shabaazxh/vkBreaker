#pragma once


#include <stdexcept>
#include <Vulkan/vulkan.h>

#include <optional>
class CommandPool {
public:
	CommandPool(VkDevice device, VkPhysicalDevice physicalDevice, VkSurfaceKHR surface, std::optional<uint32_t> graphicsValue);
	void createCommandPool();

	VkCommandPool getCommandPool() { return commandPool; }

private:
	VkCommandPool commandPool;
	VkPhysicalDevice physicalDevice;
	VkSurfaceKHR surface;
	VkDevice device;
	std::optional<uint32_t> graphicsValue;
};