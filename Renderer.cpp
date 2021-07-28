#include "Renderer.h"


Renderer::Renderer(VkDevice device, VkSwapchainKHR swapChain, std::vector<VkCommandBuffer> commandBuffers, 
VkQueue graphicsQueue, VkQueue presentQueue, std::vector<VkImage> swapChainImages, VkExtent2D swapChainExtent, 
VkPhysicalDevice physicalDevice, VkCommandPool commandPool, 
std::vector<VkDeviceMemory> uniformBufferMemory, 
std::vector<VkDeviceMemory> LightUniformBufferMemory,
std::vector<VkDeviceMemory> secondUniformBufferMemory,
std::vector<VkDeviceMemory> shaderUniformBufferMemory,
std::vector<VkDeviceMemory> obstaclesUniformBufferMemory,
glm::vec3 PlayerMovement,
glm::vec3 LightingUpdate,
float cameraFOV,
glm::vec3 playerSize) {
	this->device = device;
	this->swapChain = swapChain;
	this->commandBuffers = commandBuffers;
	this->graphicsQueue = graphicsQueue;
	this->presentQueue = presentQueue;
	this->swapChainImages = swapChainImages;
	this->swapChainExtent = swapChainExtent;
	this->physicalDevice = physicalDevice;
	this->commandPool = commandPool;
	this->uniformBufferMemory = uniformBufferMemory;
	this->LightUniformBufferMemory = LightUniformBufferMemory;
	this->secondUniformBufferMemory = secondUniformBufferMemory;
	this->shaderUniformBufferMemory = shaderUniformBufferMemory;
	this->obstaclesUniformBufferMemory = obstaclesUniformBufferMemory;
	this->PlayerMovement = PlayerMovement;
	this->LightingUpdate = LightingUpdate;
	this->cameraFOV = cameraFOV;
	this->playerSize = playerSize;
}

void Renderer::drawFrame() {	

	std::unique_ptr<Buffer> bufferAssets = std::make_unique<Buffer>(
		device, physicalDevice, commandPool, 
		graphicsQueue, swapChainImages, 
		swapChainExtent);

	//uint32_t imageIndex;
	vkAcquireNextImageKHR(device, swapChain, UINT64_MAX, imageAvaialbleSemaphore[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}

	//Mark the image as being in use by the frame
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	//HEREEE
	bufferAssets->updateUniformBuffers(imageIndex, uniformBufferMemory, LightUniformBufferMemory, 
	secondUniformBufferMemory, shaderUniformBufferMemory, obstaclesUniformBufferMemory, PlayerMovement, LightingUpdate, cameraFOV, playerSize);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
	VkSemaphore waitSemaphores[] = { imageAvaialbleSemaphore[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex]; //submit command buffer that binds swap chain image just accquired as color attachment

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphore[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	vkQueuePresentKHR(presentQueue, &presentInfo);

	vkQueueWaitIdle(presentQueue); //wait for work to finish before submitting 

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::createSyncObjects() {

	imageAvaialbleSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(swapChainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvaialbleSemaphore[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
		
	}
}


void Renderer::setCollisionSettings(glm::vec3 playerSize, glm::vec3 obstacleSize) {
	this->playerSize = playerSize;
	this->obstacleSize = obstacleSize;
}

void Renderer::processInput(GLFWwindow* window, uint32_t imageIndex){

	float lastFrame = 0.0f;

	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	float movementSpeed = 0.0005f * deltaTime; //0.001

	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
		glfwSetWindowShouldClose(window, true);
	}

	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		PlayerMovement.x += movementSpeed;
		if(PlayerMovement.x >= 3.427){
			PlayerMovement.x = 3.427;
		}
	}

	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		PlayerMovement.x -= movementSpeed;
		if(PlayerMovement.x <= -3.42){
			PlayerMovement.x = -3.42;
		}
	}

	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		PlayerMovement.y -= movementSpeed;
		if(PlayerMovement.y <= -1.876){
			PlayerMovement.y = -1.876;
		}
	}

	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		PlayerMovement.y += movementSpeed;
		if(PlayerMovement.y >= 1.8167){
			PlayerMovement.y = 1.8167;
		}
	}

	// if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
	// 	PlayerMovement.z += movementSpeed;
	// }
	// if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
	// 	PlayerMovement.z -= movementSpeed;
	// }

	//DASH Movement
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
		PlayerMovement.x += 0.02;
	}
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
		PlayerMovement.x -= 0.02;
	}
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
		PlayerMovement.y -= 0.02;
	}
	if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
		PlayerMovement.y += 0.02;
	}


	// for(int i = 0; i < 10; i++){
	// 	if(Collision->intersects(PlayerMovement, playerSize, cubePositions[i], obstacleSize)){
	// 		std::cout << "collide" << std::endl;
	// 		cubePositions[i] = glm::vec3(-100, -100, 0.0f);
	// 		score += 1;
			
	// 	}
	// }
	//std::cout << score << std::endl;
}