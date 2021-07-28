#pragma once
#include <Vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include <memory>
#include "Buffer.h"

class DescriptorSetLayout {
public:
	DescriptorSetLayout(VkDevice device);

	void createDescriptorSetLayout();

	VkDescriptorSetLayout getDescriptorSetLayout() { return descriptorSetLayout; }
	VkDescriptorSetLayout getShaderDescriptorSetLayout() { return shaderDescriptorSetLayout;}

private:
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSetLayout shaderDescriptorSetLayout;
	std::vector<VkImage> swapChainImages;
	VkDevice device;
};