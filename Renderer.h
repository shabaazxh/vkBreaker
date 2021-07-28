#pragma once

#include <Vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include "Buffer.h"
#include <memory>
#include <iostream>
#include "AABB.h"

class Renderer {
public:
	Renderer(VkDevice device, VkSwapchainKHR swapChain, 
		std::vector<VkCommandBuffer> commandBuffers, 
		VkQueue graphicsQueue, VkQueue presentQueue, 
		std::vector<VkImage> swapChainImages, VkExtent2D swapChainExtent, VkPhysicalDevice physicalDevice, VkCommandPool commandPool, 
		std::vector<VkDeviceMemory> uniformBufferMemory,
		std::vector<VkDeviceMemory> LightUniformBufferMemory,
		std::vector<VkDeviceMemory> secondUniformBufferMemory, 
		std::vector<VkDeviceMemory> shaderUniformBufferMemory,
		std::vector<VkDeviceMemory> obstaclesUniformBufferMemory,
		glm::vec3 PlayerMovement, glm::vec3 LightingUpdate, float cameraFOV,
		glm::vec3 playerSize);
	void createSyncObjects();
	void drawFrame();

	std::vector<VkSemaphore> getAvailableSemaphore() { return imageAvaialbleSemaphore; }
	std::vector<VkSemaphore> getFinishedSemaphore() { return renderFinishedSemaphore; }

	std::vector<VkFence> getFences() { return inFlightFences; }

	int getMaxFrames() { return MAX_FRAMES_IN_FLIGHT; }

	void processInput(GLFWwindow* window, uint32_t imageIndex);

	uint32_t getImageIndex() { return imageIndex;}

	glm::vec3 getPlayerMovement() { return PlayerMovement;}
	glm::vec3 getLightingUpdate() { return LightingUpdate;}
	void setLightingColour(glm::vec3 color) { LightingUpdate = color;}

	void setCollisionSettings(glm::vec3 playerSize, glm::vec3 obstacleSize);

	int getScore() { return score;}
	void setScore(int num) { score = num;}

	float getDeltaTime() { return deltaTime;}

	float getFOV() { return cameraFOV;}
	void setFOV(float setFOV) { cameraFOV = setFOV;}

	void setPlayerSize(glm::vec3 playerSizeAdjust) { playerSize = playerSizeAdjust;}

private:
	std::vector<VkSemaphore> imageAvaialbleSemaphore;
	std::vector<VkSemaphore> renderFinishedSemaphore;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	std::vector<VkDeviceMemory> uniformBufferMemory;
	std::vector<VkDeviceMemory> LightUniformBufferMemory;
	std::vector<VkDeviceMemory> secondUniformBufferMemory;
	std::vector<VkDeviceMemory> shaderUniformBufferMemory;
	std::vector<VkDeviceMemory> obstaclesUniformBufferMemory;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkDevice device;
	std::vector<VkCommandBuffer> commandBuffers;
	VkQueue graphicsQueue;
	VkQueue presentQueue;
	VkExtent2D swapChainExtent;
	VkPhysicalDevice physicalDevice;
	VkCommandPool commandPool;

	const int MAX_FRAMES_IN_FLIGHT = 2;

	glm::vec3 PlayerMovement;
	glm::vec3 playerSize;
	glm::vec3 obstacleSize;

	glm::vec3 LightingUpdate;

	//To use right pair of semaphores, need to keep track of current frame
	size_t currentFrame = 0;
	uint32_t imageIndex;
	int score = 0;

	//Game time
	float deltaTime;
	float cameraFOV;
};