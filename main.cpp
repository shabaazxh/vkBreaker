#define GLFW_INCLUDE_VULKAN
#include <iostream>
#include <Vulkan/vulkan.h>

#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "lib/glm/glm/vec4.hpp"
#include "lib/glm/glm/mat4x4.hpp"


#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"
#include "lib/tinyobjloader/tiny_obj_loader.h"

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>
#include <optional>
#include <set>
#include <cstdint>
#include <algorithm>
#include <memory>

//Class
#include "pipeline.h"
#include "RenderPass.h"
#include "Framebuffers.h"
#include "commandPool.h"
#include "commandBuffers.h"
#include "Renderer.h"
#include "Object.h"
#include "Buffer.h"
#include "DescriptorSetLayout.h"
#include "Descriptors.h"
#include "AABB.h"

// Define shader location (Will need to update these the system path)
const std::string WIN_OS_SHADER_LOC_VERT = "shaders/vert.spv";
const std::string WIN_OS_SHADER_LOC_FRAG = "shaders/frag.spv";

const std::string WIN_OS_SHADER_LOC_VERT_ENEMY = "shaders/Enemy_vert.spv";
const std::string WIN_OS_SHADER_LOC_FRAG_ENEMY = "shaders/Enemy_frag.spv";

const std::string WIN_OS_SHADER_LOC_VERT_STATIC = "shaders/static_vert.spv";
const std::string WIN_OS_SHADER_LOC_FRAG_STATIC = "shaders/static_frag.spv";

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};


class vulkanEngine {
public:
	void run() {
		initWindow();
		initVulkan();
		mainloop();
		cleanup();
	}
private:
	GLFWwindow* window;

	VkInstance instance; 
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice logicalDevice;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages; // <- retrieving the swap chain images
	VkFormat swapChainImageFormat; // <- store the swap chain format for later use
	VkExtent2D swapChainExtent; // <- store the swapchain extent for later use

	std::vector<VkImageView> swapChainImagesViews; // <- an ImageView is just a view into the image describes how to access the image and which part of the image to access

	//Instance class
	std::unique_ptr<Pipeline> pipeline;
	std::unique_ptr<Pipeline> static_pipeline[2];
	std::unique_ptr<Pipeline> obstacles_pipeline;
	std::unique_ptr<RenderPass> renderPass;
	std::unique_ptr<Framebuffers> frameBuffers;
	std::unique_ptr<CommandPool> commandPool;
	std::unique_ptr<CommandBuffers> commandBuffers;
	std::unique_ptr<Renderer> renderer;
	std::unique_ptr<Buffer> buffers;
	std::unique_ptr<Buffer> playerBuffer;
	std::unique_ptr<Buffer> Obstacles;
	std::unique_ptr<Buffer> SecondBufferUniform;
	std::unique_ptr<Buffer> LightUniformBuffer;

	std::unique_ptr<Buffer> secondSquare;
	std::unique_ptr<Object> objectData;
	std::unique_ptr<DescriptorSetLayout> descriptorSetLayout;
	std::unique_ptr<Descriptors> descriptors;

	//Collision
	std::unique_ptr<AABB> Collision = std::make_unique<AABB>();

	glm::vec3 playerPosition = glm::vec3(0,0,0);
	float currentTime = 0.0f; // Keep track of delta time when collision happens
	
	void initWindow() {
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		window = glfwCreateWindow(1920, 1080, "Olrun", nullptr, nullptr);
	}
	void initVulkan() {
		createInstance();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
		renderPass = std::make_unique<RenderPass>(logicalDevice, swapChainImageFormat);
		renderPass->createRenderPass();

		descriptorSetLayout = std::make_unique<DescriptorSetLayout>(logicalDevice);
		descriptorSetLayout->createDescriptorSetLayout();

		pipeline = std::make_unique<Pipeline>(logicalDevice, renderPass->getRenderPass(), swapChainExtent, descriptorSetLayout->getDescriptorSetLayout());
		pipeline->createGraphicsPipeline(WIN_OS_SHADER_LOC_VERT, WIN_OS_SHADER_LOC_FRAG);

		static_pipeline[0] = std::make_unique<Pipeline>(logicalDevice, renderPass->getRenderPass(), swapChainExtent, descriptorSetLayout->getShaderDescriptorSetLayout());
		static_pipeline[0]->createGraphicsPipeline(WIN_OS_SHADER_LOC_VERT_STATIC, WIN_OS_SHADER_LOC_FRAG_STATIC);
		
		static_pipeline[1] = std::make_unique<Pipeline>(logicalDevice, renderPass->getRenderPass(), swapChainExtent, descriptorSetLayout->getDescriptorSetLayout());
		static_pipeline[1]->createGraphicsPipeline(WIN_OS_SHADER_LOC_VERT, WIN_OS_SHADER_LOC_FRAG);

		obstacles_pipeline = std::make_unique<Pipeline>(logicalDevice, renderPass->getRenderPass(), swapChainExtent, descriptorSetLayout->getDescriptorSetLayout());
		obstacles_pipeline->createGraphicsPipeline(WIN_OS_SHADER_LOC_VERT_ENEMY, WIN_OS_SHADER_LOC_FRAG_ENEMY);

		frameBuffers = std::make_unique<Framebuffers>(logicalDevice, swapChainImagesViews, renderPass->getRenderPass(), swapChainExtent);
		frameBuffers->createFramebuffers();

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		commandPool = std::make_unique<CommandPool>(logicalDevice, physicalDevice, surface, indices.graphicsFamily);
		commandPool->createCommandPool();

		objectData = std::make_unique<Object>();

		// NOT BEING USED CUBE
		buffers = std::make_unique<Buffer>(
			logicalDevice, 
			physicalDevice, 
			commandPool->getCommandPool(), 
			graphicsQueue, 
			swapChainImages, 
			swapChainExtent);
		buffers->createVertexBuffer(
			sizeof(objectData->getCubeVertexData()[0]) * objectData->getCubeVertexData().size(),
			objectData->getCubeVertexData());
		buffers->createIndexBuffer();
		buffers->createUniformBuffer(sizeof(UniformBufferObject));

		//LIGHTING BUFFER
		LightUniformBuffer = std::make_unique<Buffer>(
			logicalDevice, 
			physicalDevice, 
			commandPool->getCommandPool(), 
			graphicsQueue, 
			swapChainImages, 
			swapChainExtent);
		LightUniformBuffer->createUniformBuffer(sizeof(LightUniforms));

		SecondBufferUniform = std::make_unique<Buffer>(
			logicalDevice, 
			physicalDevice, 
			commandPool->getCommandPool(), 
			graphicsQueue, 
			swapChainImages, 
			swapChainExtent);
		SecondBufferUniform->createUniformBuffer(sizeof(UniformBufferObject));

		//PLAYER BUFFER
		playerBuffer = std::make_unique<Buffer>(
			logicalDevice, 
			physicalDevice, 
			commandPool->getCommandPool(), 
			graphicsQueue, 
			swapChainImages, 
			swapChainExtent);
		
		//Player buffer
		playerBuffer->createVertexBuffer(sizeof(objectData->getTriangleData()[0]) * objectData->getTriangleData().size(),
		objectData->getTriangleData());
		playerBuffer->createIndexBuffer();
		playerBuffer->createUniformBuffer(sizeof(UniformBufferObject));

		//Other obstacles buffer
		Obstacles = std::make_unique<Buffer>(
			logicalDevice, 
			physicalDevice, 
			commandPool->getCommandPool(), 
			graphicsQueue, 
			swapChainImages, 
			swapChainExtent);
		Obstacles->createVertexBuffer(sizeof(objectData->getTriangleData()[0]) * objectData->getTriangleData().size(),
		objectData->getTriangleData());
		Obstacles->createIndexBuffer();
		Obstacles->createUniformBuffer(sizeof(ObstacleUniformBufferObject));

		//BACKGROUND BUFFER
		secondSquare = std::make_unique<Buffer>(
			logicalDevice, 
			physicalDevice, 
			commandPool->getCommandPool(), 
			graphicsQueue, 
			swapChainImages, 
			swapChainExtent);
		secondSquare->createVertexBuffer(sizeof(objectData->getBackgroundData()[0]) * objectData->getBackgroundData().size(),
			objectData->getBackgroundData());
		secondSquare->createIndexBuffer();
		secondSquare->createUniformBuffer(sizeof(ShaderUniform));

		//Specify uniforms here instead of passing into function all the DAMN time
		UniformBuffersSpecificaton uniformsSpec;
		uniformsSpec.uniformBuffers = buffers->getUniformBuffers();
		uniformsSpec.LightBuffers = LightUniformBuffer->getUniformBuffers();
		uniformsSpec.secondBuffer = SecondBufferUniform->getUniformBuffers();
		uniformsSpec.playerUniform = playerBuffer->getUniformBuffers();
		uniformsSpec.shaderUniform = secondSquare->getUniformBuffers();
		uniformsSpec.ObstaclesUniform = Obstacles->getUniformBuffers();

		descriptors = std::make_unique<Descriptors>(
			uniformsSpec,
			logicalDevice, 
			swapChainImages,  
			descriptorSetLayout->getDescriptorSetLayout(),
			descriptorSetLayout->getShaderDescriptorSetLayout());
			
		descriptors->createDescriptorPool();
		descriptors->createDescriptorSets();

		commandBuffers = std::make_unique<CommandBuffers>(
			logicalDevice, 
			frameBuffers->getSwapChainFramebuffers(), 
			commandPool->getCommandPool(), 
			renderPass->getRenderPass(), 
			swapChainExtent, 
			pipeline->getGraphicsPipeline(), 
			pipeline->getComputePipeline(), 
			physicalDevice, 
			buffers->getVertexBuffer(), 
			buffers->getIndexBuffer(), 
			pipeline->getPipelineLayout(), 
			descriptors->getDescriptorSets(),
			secondSquare->getVertexBuffer(),
			secondSquare->getIndexBuffer(),
			static_pipeline[0]->getGraphicsPipeline(),
			static_pipeline[0]->getPipelineLayout(),
			descriptors->getDescriptorSetSecond(),
			playerBuffer->getVertexBuffer(),
			playerBuffer->getIndexBuffer(),
			static_pipeline[1]->getGraphicsPipeline(),
			static_pipeline[1]->getPipelineLayout(),
			descriptors->getShaderDescriptorSet(),
			Obstacles->getVertexBuffer(),
			Obstacles->getIndexBuffer(),
			obstacles_pipeline->getGraphicsPipeline(),
			obstacles_pipeline->getPipelineLayout());

		commandBuffers->createCommandBuffers();

		glm::vec3 LightingColor = glm::vec3(1.0f, 1.0f, 1.0f);

		renderer = std::make_unique<Renderer>(
			logicalDevice, 
			swapChain, 
			commandBuffers->getCommandBuffers(), 
			graphicsQueue, 
			presentQueue, 
			swapChainImages, 
			swapChainExtent, 
			physicalDevice,
			commandPool->getCommandPool(), 
			buffers->getUniformBuffersMemory(), 
			LightUniformBuffer->getUniformBuffersMemory(),
			SecondBufferUniform->getUniformBuffersMemory(),
			secondSquare->getUniformBuffersMemory(),
			Obstacles->getUniformBuffersMemory(),
			playerPosition,
			LightingColor,
			45.0f,
			buffers->getPlayerSize());

		renderer->createSyncObjects();
		std::cout << renderer->getFOV() << std::endl;

	}
	void mainloop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			renderer->processInput(window, renderer->getImageIndex());
			for(int i = 0; i < 10; i++){
				if(Collision->intersects(renderer->getPlayerMovement(), buffers->getPlayerSize(), cubePositions[i], buffers->getObstacleSize())){
					//renderer->setFOV(30.0f);
					renderer->setLightingColour(glm::vec3(0.0,0.3,1.0));
					renderer->setScore(renderer->getScore() + 1);
					renderer->setPlayerSize(glm::vec3(0.7));
					cubePositions[i] = glm::vec3(-100, -100, 0.0);
					std::cout << renderer->getScore() << std::endl; // score
					currentTime = renderer->getDeltaTime();
				} 
				// The value added here is the duration of the colour change affect
				if(currentTime + 0.2f <= renderer->getDeltaTime()){
					renderer->setLightingColour(glm::vec3(1.0f, 1.0f, 1.0f));
					renderer->setPlayerSize(glm::vec3(0.5));
					//renderer->setFOV(45.0f);
				}
			}
			renderer->drawFrame();
		}
		vkDeviceWaitIdle(logicalDevice);
	}
	void cleanupSwapChain() {

		//Destroy swapchain frame buffers
		for (auto framebuffer : frameBuffers->getSwapChainFramebuffers()) {
			vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
		}

		//Free command buffers 
		vkFreeCommandBuffers(logicalDevice, commandPool->getCommandPool(), static_cast<uint32_t>(commandBuffers->getCommandBuffers
			().size()), commandBuffers->getCommandBuffers().data());

		//Destroy first pipeline & layout
		vkDestroyPipeline(logicalDevice, pipeline->getGraphicsPipeline(), nullptr);
		vkDestroyPipelineLayout(logicalDevice, pipeline->getPipelineLayout(), nullptr);
		vkDestroyRenderPass(logicalDevice, renderPass->getRenderPass(), nullptr);

		//Destroy static (second) pipeline & layout
		vkDestroyPipeline(logicalDevice, static_pipeline[0]->getGraphicsPipeline(), nullptr);
		vkDestroyPipelineLayout(logicalDevice, static_pipeline[0]->getPipelineLayout(), nullptr);

		//Destroy swapchain image views
		for (auto imageView : swapChainImagesViews) {
			vkDestroyImageView(logicalDevice, imageView, nullptr);
		}

		//Destroy the swapchain
		vkDestroySwapchainKHR(logicalDevice, swapChain, nullptr);

		//Destroy uniform buffers 
		for (size_t i = 0; i < swapChainImages.size(); i++) {
			vkDestroyBuffer(logicalDevice, buffers->getUniformBuffers()[i], nullptr);
			vkFreeMemory(logicalDevice, buffers->getUniformBuffersMemory()[i], nullptr);
		}

		vkDestroyDescriptorPool(logicalDevice, descriptors->getDescriptorPool(), nullptr);
	}

	void cleanup() {
		cleanupSwapChain();

		//Destroy second pipeline index buffer
		vkDestroyBuffer(logicalDevice, secondSquare->getIndexBuffer(), nullptr);
		vkFreeMemory(logicalDevice, secondSquare->getIndexBufferMemory(), nullptr);

		//Destroy index buffer
		vkDestroyBuffer(logicalDevice, buffers->getIndexBuffer(), nullptr);
		vkFreeMemory(logicalDevice, buffers->getIndexBufferMemory(), nullptr);

		//Destroy descriptor set layout
		vkDestroyDescriptorSetLayout(logicalDevice, descriptorSetLayout->getDescriptorSetLayout(), nullptr);

		//Destroy second pipeline vertex buffer & memory
		vkDestroyBuffer(logicalDevice, secondSquare->getVertexBuffer(), nullptr);
		vkFreeMemory(logicalDevice, secondSquare->getVertexBufferMemory(), nullptr);

		//Destroy first vertex & memory buffer
		vkDestroyBuffer(logicalDevice, buffers->getVertexBuffer(), nullptr);
		vkFreeMemory(logicalDevice, buffers->getVertexBufferMemory(), nullptr);

		//Destroy semaphores
		for (size_t i = 0; i < renderer->getMaxFrames(); i++) {
			vkDestroySemaphore(logicalDevice, renderer->getFinishedSemaphore()[i], nullptr);
			vkDestroySemaphore(logicalDevice, renderer->getAvailableSemaphore()[i], nullptr);
			vkDestroyFence(logicalDevice, renderer->getFences()[i], nullptr);
		}

		//Destroy the command pool
		vkDestroyCommandPool(logicalDevice, commandPool->getCommandPool(), nullptr);

		//Destroy the logical device
		vkDestroyDevice(logicalDevice, nullptr);
		//Destroy surface to window
		vkDestroySurfaceKHR(instance, surface, nullptr);
		//Destroy Vulkan instance
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);
		glfwTerminate();

	}

	void recreateSwapChain() {
		vkDeviceWaitIdle(logicalDevice);

		createSwapChain();
		createImageViews();
		renderPass->createRenderPass();
		pipeline->createGraphicsPipeline("C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/vert.spv", "C:/Users/Shahb/source/repos/VulkanTesting/VulkanTesting/shaders/frag.spv");
		frameBuffers->createFramebuffers();
		buffers->createUniformBuffer(sizeof(UniformBufferObject));
		descriptors->createDescriptorPool();
		commandBuffers->createCommandBuffers();
	}

	void createInstance() {
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Vulkan Engine";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledLayerCount = 0;

		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create vulkan instance!");
		}

	}

	void createSurface() {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	void pickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				physicalDevice = device;
				break;
			}
		}
		if (physicalDevice == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}
	}

	void createLogicalDevice() {
		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily.value(), indices.presentFamily.value() };


		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};

		/* Creating the logical device */
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();
		createInfo.pEnabledFeatures = 0;

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

		vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
	}

	void createSwapChain() {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		//amount of images in the swap chain
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		//make sure not to exceed the max number of images 
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1; //always one unless it's stereoscopic 3d application
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		//how to handle swap chain images that will be used across multiple families

		QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; //image is owned by one queue family at a time
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; //images can be used across multiple families 
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(logicalDevice, swapChain, &imageCount, swapChainImages.data());

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}

	void createImageViews() {
		//resize to fit all the image views we will be creating -> make inline with swapchanimages amount
		swapChainImagesViews.resize(swapChainImages.size());

		for (size_t i = 0; i < swapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapChainImagesViews[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create image views!");
			}
		}
	}

	bool isDeviceSuitable(VkPhysicalDevice device) {
		QueueFamilyIndices indices = findQueueFamilies(device);


		bool extensionSupport = checkDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionSupport) {
			SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		return indices.isComplete() && extensionSupport && swapChainAdequate;
	}

	bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr ,&extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	std::vector<const char*> getRequiredExtensions() {
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		return extensions;
	}

	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
				indices.graphicsFamily = i;
			}
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
			if (presentSupport) {
				indices.presentFamily = i;
			}

			if (indices.isComplete()) {
				break;
			}
			i++;
		}

		return indices;
	}

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details;
	}

	//Surface format for the swapchain -> getting the right colours
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}
		return availableFormats[0]; //else just take the first one available 
	}

	//Swap chain present modes
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	//Matching swap chain extent to the resolution of the swap chain images -> same res as window
	//need to set VkSurfaceCapabilitiesKHR currentExtent member 
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilites) {
		if (capabilites.currentExtent.width != UINT32_MAX) {
			return capabilites.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilites.minImageExtent.width, 
				capabilites.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilites.minImageExtent.height,
				capabilites.maxImageExtent.height);

			return actualExtent;
		}

	}
};

int main() {
	vulkanEngine engine;

	try {
		engine.run();
	}
	catch (const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
