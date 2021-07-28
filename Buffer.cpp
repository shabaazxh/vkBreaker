#include "Buffer.h"

Buffer::Buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::vector<VkImage> swapChainImages, VkExtent2D swapChainExtent) {
	this->device = device;
	this->physicalDevice = physicalDevice;
	this->commandPool = commandPool;
	this->graphicsQueue = graphicsQueue;
	this->swapChainImages = swapChainImages;
	this->swapChainExtent = swapChainExtent;
}

void Buffer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
	VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	//Getting memory requirements for the buffer
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryRequirements(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate the buffer memory!");
	}

	// If memoery allocation is success->associate the memory to the buffer
	vkBindBufferMemory(device, buffer, bufferMemory, 0);

}

void Buffer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	//Memory transfer opertations are executed using command buffers 
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	//Recording the command buffer
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; //Use command buffer once

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

void Buffer::createVertexBuffer(size_t buffer_size, std::vector<Vertex> ObjectData) {
	VkDeviceSize bufferSize = buffer_size;

	//On the CPU 
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	//Copy vertex data into vertex buffer
	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, ObjectData.data(), (size_t) bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | 
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		vertexBuffer, vertexBufferMemory);

	copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);

}

void Buffer::createIndexBuffer() {
	VkDeviceSize bufferSize = sizeof(ObjectInfo->getIndexData()[0]) * ObjectInfo->getIndexData().size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, ObjectInfo->getIndexData().data(), (size_t)bufferSize);
	vkUnmapMemory(device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(device,stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);

}

void Buffer::createUniformBuffer(size_t buffer_size) {
	VkDeviceSize bufferSize = buffer_size;

	uniformBuffers.resize(swapChainImages.size());
	uniformBuffersMemory.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniformBuffers[i], uniformBuffersMemory[i]);
	}
}

void Buffer::updateUniformBuffers(uint32_t currentImage, std::vector<VkDeviceMemory> uniformBuffersMemory,
std::vector<VkDeviceMemory> LightUniformMemory, std::vector<VkDeviceMemory> secondUniformBufferMemory,
std::vector<VkDeviceMemory> ShaderUniformBufferMemory,
std::vector<VkDeviceMemory> obstaclesUniformBufferMemory,
glm::vec3 PlayerMovement, glm::vec3 LightingUpdate, float cameraFOV, glm::vec3 playerSize) {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	LightUniforms lighting;
	lighting.LightPosition = glm::vec3(1.2f, 1.0f, 2.0f);
	lighting.LightColor = glm::vec4(LightingUpdate, 1.0);
	lighting.ObjectColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	ShaderUniform shaderUniform{};
	shaderUniform.time = (float)glfwGetTime();
	
	UniformBufferObject ubo{};
	ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(PlayerMovement));
	ubo.model = glm::scale(ubo.model, glm::vec3(playerSize));
	//ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.proj = glm::perspective(glm::radians(cameraFOV), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 10.0f);
	//ubo.proj[1][1] *= -1;

	void* data;
	vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(device, uniformBuffersMemory[currentImage]);

	vkMapMemory(device, LightUniformMemory[currentImage], 0, sizeof(lighting), 0, &data);
	memcpy(data, &lighting, sizeof(lighting));
	vkUnmapMemory(device, LightUniformMemory[currentImage]);

	ubo.model = glm::translate(ubo.model, glm::vec3(-2.0f, 0.0f, 0.0f));
	vkMapMemory(device, secondUniformBufferMemory[currentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(device, secondUniformBufferMemory[currentImage]);

	vkMapMemory(device, ShaderUniformBufferMemory[currentImage], 0, sizeof(shaderUniform), 0, &data);
	memcpy(data, &shaderUniform, sizeof(shaderUniform));
	vkUnmapMemory(device, ShaderUniformBufferMemory[currentImage]);

	// ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(obstaclePosition));
	// ubo.model = glm::scale(ubo.model, glm::vec3(obstacleSize));

	ObstacleUniformBufferObject oUBO{};
	for(int i = 0; i < 30; i++){
		oUBO.time = (float)glfwGetTime();
		oUBO.model[i] = glm::translate(glm::mat4(1.0f), glm::vec3(cubePositions[i]));
		oUBO.model[i] = glm::rotate(oUBO.model[i], time * glm::radians(45.0f),glm::vec3(0.0, 0.0, 1.0));
		oUBO.model[i] = glm::scale(oUBO.model[i], glm::vec3(obstacleSize));
	}
	oUBO.view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	oUBO.proj = glm::perspective(glm::radians(cameraFOV), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 10.0f);

	vkMapMemory(device, obstaclesUniformBufferMemory[currentImage], 0, sizeof(oUBO), 0, &data);
	memcpy(data, &oUBO, sizeof(oUBO));
	vkUnmapMemory(device, obstaclesUniformBufferMemory[currentImage]);
}

uint32_t Buffer::findMemoryRequirements(uint32_t typeFilter, VkMemoryPropertyFlags properties) {

	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	/* Make sure memory type is suitable and also check if memory allows is host visible using .propertyFlags */
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties){
			return i;
		}
	}
}