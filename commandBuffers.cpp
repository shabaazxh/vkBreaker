#include "commandBuffers.h"
#include <iostream>

CommandBuffers::CommandBuffers(VkDevice device, std::vector<VkFramebuffer> 
swapChainFramebuffers, VkCommandPool commandPool, 
VkRenderPass renderPass, VkExtent2D swapChainExtent, 
VkPipeline graphicsPipeline, VkPipeline computePipeline, 
VkPhysicalDevice physicalDevice, VkBuffer renderingBuffer, 
VkBuffer indexBuffer ,VkPipelineLayout pipelineLayout, 
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
VkPipelineLayout obstaclesPipelineLayout) {
	this->device = device;
	this->swapChainFramebuffers = swapChainFramebuffers;
	this->commandPool = commandPool;
	this->renderPass = renderPass;
	this->swapChainExtent = swapChainExtent;
	this->graphicsPipeline = graphicsPipeline;
	this->computePipeline = computePipeline;
	this->renderingBuffer = renderingBuffer;
	this->pipelineLayout = pipelineLayout;
	this->descriptorSets = descriptorSets;
	this->indexBuffer = indexBuffer;
	this->secondRenderBuffer = secondRenderBuffer;
	this->secondIndexBuffer = secondIndexBuffer;
	this->static_pipeline = static_pipeline;
	this->static_pipelineLayout = static_pipelineLayout;
	this->secondDescriptorSet = secondDescriptorSet;
	this->playerVertexBuffer = playerVertexBuffer;
	this->playerIndexBuffer = playerIndexBuffer;
	this->playerPipeline = playerPipeline;
	this->playerPipelineLayout = playerPipelineLayout;
	this->shaderDescriptorSet = shaderDescriptorSet;
	this->obstaclesBuffer = obstaclesBuffer;
	this->obstaclesIndexBuffer = obstaclesIndexBuffer;
	this->obstaclesPipeline = obstaclesPipeline;
	this->obstaclesPipelineLayout = obstaclesPipelineLayout;
}


void CommandBuffers::createCommandBuffers() {
	commandBuffers.resize(swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}


		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = swapChainExtent;

		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkBuffer vertexBuffers[] = { renderingBuffer };
		VkDeviceSize offsets[] = { 0 };

		VkBuffer secondVertexBuffer[] = { secondRenderBuffer };
		VkDeviceSize offSetsSecond[] = {0};

		VkBuffer playerBuffer[] = { playerVertexBuffer};
		VkDeviceSize offSetPlayer[] = {0};

		VkBuffer obstaclesVertexBuffer[] = { obstaclesBuffer };
		VkDeviceSize offSetObstacles[] = {0};

		//Draw second
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, static_pipeline);
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, static_pipelineLayout, 0, 1, &shaderDescriptorSet[i], 0, nullptr);
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, secondVertexBuffer, offSetsSecond);
		vkCmdBindIndexBuffer(commandBuffers[i], secondIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(GameObjectVertices->getIndexData().size()), 1, 0, 0, 0);

		//Player buffer
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, playerPipeline);
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, playerPipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, playerBuffer, offSetPlayer);
		vkCmdBindIndexBuffer(commandBuffers[i], playerIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		//vkCmdDraw(commandBuffers[i], static_cast<uint32_t>(GameObjectVertices->getCubeVertexData().size()), 1, 0, 0);
		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(GameObjectVertices->getIndexData().size()), 1, 0,0,0);

		//Obstacles object
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, obstaclesPipeline);
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, obstaclesPipelineLayout, 0, 1, &secondDescriptorSet[i], 0, nullptr);
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, obstaclesVertexBuffer, offSetObstacles);
		vkCmdBindIndexBuffer(commandBuffers[i], obstaclesIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		//vkCmdDraw(commandBuffers[i], static_cast<uint32_t>(GameObjectVertices->getCubeVertexData().size()), 1, 0, 0);
		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(GameObjectVertices->getIndexData().size()), 10, 0,0,0);

		//Floor object
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, playerPipeline);
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, playerPipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, playerBuffer, offSetPlayer);
		vkCmdBindIndexBuffer(commandBuffers[i], playerIndexBuffer, 0, VK_INDEX_TYPE_UINT32);
		//vkCmdDraw(commandBuffers[i], static_cast<uint32_t>(GameObjectVertices->getCubeVertexData().size()), 1, 0, 0);
		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(GameObjectVertices->getIndexData().size()), 1, 0,0,0);

		// Cube buffer
		// vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		// vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &secondDescriptorSet[i], 0, nullptr);
		// vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);
		// //vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		// vkCmdDraw(commandBuffers[i], static_cast<uint32_t>(GameObjectVertices->getCubeVertexData().size()), 1, 0, 0);
		// //vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(GameObjectVertices->getIndexData().size()), 1, 0,0,0);

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

	}

}

/* COMPUTE PIPELINE */
		//vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, computePipeline);
		//vkCmdDispatch(commandBuffers[i], 256,1,1);