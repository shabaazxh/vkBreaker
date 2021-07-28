#pragma once

#include <Vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

struct UniformBuffersSpecificaton {
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkBuffer> LightBuffers;
	std::vector<VkBuffer> secondBuffer;
	std::vector<VkBuffer> playerUniform;
	std::vector<VkBuffer> shaderUniform;
	std::vector<VkBuffer> ObstaclesUniform;
};

class Descriptors {
public:
	Descriptors(const UniformBuffersSpecificaton& uniforms,VkDevice device, 
	std::vector<VkImage> swapChainImages, VkDescriptorSetLayout descriptorSetLayout,
	VkDescriptorSetLayout shaderDescriptorSetLayout);

	void createDescriptorPool();
	void createDescriptorSets();

	VkDescriptorPool getDescriptorPool() { return descriptorPool; }
	std::vector<VkDescriptorSet> getDescriptorSets() { return descriptorSets; }

	std::vector<VkDescriptorSet> getDescriptorSetSecond() { return secondDescriptorSets;}
	std::vector<VkDescriptorSet> getShaderDescriptorSet() { return ShaderDescriptorSet;}
private:

	UniformBuffersSpecificaton uniforms;

	VkDevice device;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSetLayout shaderDescriptorSetLayout;

	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkDescriptorSet> secondDescriptorSets;
	std::vector<VkDescriptorSet> ShaderDescriptorSet;
	std::vector<VkDescriptorSet> obstaclesDescriptorSet;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkBuffer> LightBuffers;

	std::vector<VkImage> swapChainImages;
	VkDescriptorPool descriptorPool;
};