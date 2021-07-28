#pragma once

#define GLM_FORCE_RADIANS
#include <Vulkan/vulkan.h>
#include "Object.h"
#include <memory>
#include <stdexcept>
#include <vector>

#include "lib/glm/glm/glm.hpp"
#include "lib/glm/glm/gtc/matrix_transform.hpp"
#include <iostream>
#include <GLFW/glfw3.h>

#include <chrono>


class Buffer {
public:
	Buffer(VkDevice device, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, VkQueue graphicsQueue, std::vector<VkImage> swapChainImages, VkExtent2D swapChainExtent);
	Buffer() = default;
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, 
		VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	//Vertex buffer
	void createVertexBuffer(size_t buffer_size, std::vector<Vertex> ObjectData);
	VkBuffer getVertexBuffer() { return vertexBuffer; }
	VkDeviceMemory getVertexBufferMemory() { return vertexBufferMemory; }

	//Index buffer
	void createIndexBuffer();
	VkBuffer getIndexBuffer() { return indexBuffer; }
	VkDeviceMemory getIndexBufferMemory() { return indexBufferMemory; }

	//Uniform buffer
	void createUniformBuffer(size_t buffer_size);
	void updateUniformBuffers(uint32_t currentImage, 
	std::vector<VkDeviceMemory> uniformBuffersMemory,
	std::vector<VkDeviceMemory> LightUniformMemory,
	std::vector<VkDeviceMemory> secondUniformBufferMemory, 
	std::vector<VkDeviceMemory> ShaderUniformBufferMemory,
	std::vector<VkDeviceMemory> obstaclesUniformBufferMemory,
	glm::vec3 PlayerMovement, glm::vec3 LightingUpdate, float cameraFOV, glm::vec3 playerSize);

	std::vector<VkBuffer> getUniformBuffers() { return uniformBuffers; }
	std::vector<VkDeviceMemory> getUniformBuffersMemory() { return uniformBuffersMemory; }

	uint32_t findMemoryRequirements(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkExtent2D getSwapChainExtent() { return swapChainExtent; }

	glm::vec3 getPlayerSize() { return playerSize;}
	
	glm::vec3 getObstacleSize() { return obstacleSize;}
	glm::vec3 getObstaclePosition() { return obstaclePosition;}

private:
	VkCommandPool commandPool;
	VkDevice device;
	VkPhysicalDevice physicalDevice;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;

	VkExtent2D swapChainExtent;

	std::vector<VkImage> swapChainImages;

	VkQueue graphicsQueue;
	std::unique_ptr<Object> ObjectInfo = std::make_unique<Object>();


	glm::vec3 playerSize = glm::vec3(0.5);
	glm::vec3 obstacleSize = glm::vec3(0.5);
	glm::vec3 obstaclePosition = glm::vec3(1.0, 0.0, 0.0);

};