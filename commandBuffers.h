#pragma once

#include <Vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include <memory>
#include "Buffer.h"
#include "Object.h"
#include "Buffer.h"

class CommandBuffers {
public:
	CommandBuffers(VkDevice device, std::vector<VkFramebuffer> swapChainFramebuffers, 
		VkCommandPool commandPool, VkRenderPass renderPass, 
		VkExtent2D swapChainExtent, VkPipeline graphicsPipeline, 
		VkPipeline computePipeline, VkPhysicalDevice physicalDevice, 
		VkBuffer renderingBuffer, VkBuffer indexBuffer,VkPipelineLayout pipelineLayout,
		std::vector<VkDescriptorSet> descriptorSets,
		VkBuffer secondRenderBuffer, 
		VkBuffer secondIndexBuffer,
		VkPipeline static_pipeline, 
		VkPipelineLayout static_pipelineLayout,
		std::vector<VkDescriptorSet> secondDescriptorSet,
		VkBuffer playerVertexBuffer,
		VkBuffer playerIndexBuffer,
		VkPipeline playerPipeline,
		VkPipelineLayout playerPipelineLayout,
		std::vector<VkDescriptorSet> shaderDescriptorSet,
		VkBuffer obstaclesBuffer,
		VkBuffer obstaclesIndexBuffer,
		VkPipeline obstaclesPipeline,
		VkPipelineLayout obstaclesPipelineLayout);

	void createCommandBuffers();

	std::vector<VkCommandBuffer> getCommandBuffers() { return commandBuffers; }

private:
	//std::unique_ptr<Buffer> GameObjectBuffer = std::make_unique<Buffer>(device, physicalDevice);
	std::unique_ptr<Object> GameObjectVertices = std::make_unique<Object>();
	std::unique_ptr<Buffer> bufferAseets = std::make_unique<Buffer>();

	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkDevice device;
	VkCommandPool commandPool;
	VkRenderPass renderPass;
	VkExtent2D swapChainExtent;
	VkPipeline graphicsPipeline;
	VkPipeline computePipeline;
	VkPhysicalDevice physicalDevice;
	VkPipelineLayout pipelineLayout;


	VkPipeline static_pipeline;
	VkPipelineLayout static_pipelineLayout;

	VkPipeline playerPipeline;
	VkPipelineLayout playerPipelineLayout;

	VkPipeline obstaclesPipeline;
	VkPipelineLayout obstaclesPipelineLayout;

	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkDescriptorSet> secondDescriptorSet;
	std::vector<VkDescriptorSet> shaderDescriptorSet;

	VkBuffer renderingBuffer;
	VkBuffer indexBuffer;

	VkBuffer secondRenderBuffer;
	VkBuffer secondIndexBuffer;

	VkBuffer playerVertexBuffer;
	VkBuffer playerIndexBuffer;

	VkBuffer obstaclesBuffer;
	VkBuffer obstaclesIndexBuffer;
	
};