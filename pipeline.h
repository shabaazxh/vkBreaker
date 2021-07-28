#pragma once

#include <vector>
#include "shader.h"
#include <Vulkan/vulkan.h>
#include "Object.h"
#include <memory>

class Pipeline {
public:
	Pipeline(VkDevice device, VkRenderPass renderPass,VkExtent2D swapChainExtent, VkDescriptorSetLayout descriptorSetLayout);
	void createGraphicsPipeline(std::string vertFile, std::string fragFile);
	VkPipelineLayout getPipelineLayout() { return pipelineLayout; }
	VkPipeline getGraphicsPipeline() { return graphicsPipeline; }

	void createComputePipeline(std::string computeFile);

	VkPipeline getComputePipeline() { return computePipeline; }
private:
	VkDevice device;
	VkExtent2D swapChainExtent;
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;
	VkPipeline graphicsPipeline;
	VkPipeline computePipeline;
	VkDescriptorSetLayout descriptorSetLayout;

	std::unique_ptr<Object> GameObject = std::make_unique<Object>();
};