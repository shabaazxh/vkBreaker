#include "Descriptors.h"
#include "Object.h"



Descriptors::Descriptors(const UniformBuffersSpecificaton& uniforms,VkDevice device, 
std::vector<VkImage> swapChainImages,VkDescriptorSetLayout descriptorSetLayout,
VkDescriptorSetLayout shaderDescriptorSetLayout) {
	this->device = device;
	this->swapChainImages = swapChainImages;
	this->descriptorSetLayout = descriptorSetLayout;
	this->uniforms = uniforms;
	this->shaderDescriptorSetLayout = shaderDescriptorSetLayout;
}

void Descriptors::createDescriptorPool() {
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = 20;

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = 40;

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void Descriptors::createDescriptorSets() {
	//create a descriptor set for each swapchain image, all with the same layout
	std::vector<VkDescriptorSetLayout> layout(swapChainImages.size(), descriptorSetLayout); 
	std::vector<VkDescriptorSetLayout> ShaderLayout(swapChainImages.size(), shaderDescriptorSetLayout); 

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
	allocInfo.pSetLayouts = layout.data();

	VkDescriptorSetAllocateInfo ShadeAallocInfo{};
	ShadeAallocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	ShadeAallocInfo.descriptorPool = descriptorPool;
	ShadeAallocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
	ShadeAallocInfo.pSetLayouts = ShaderLayout.data();

	descriptorSets.resize(swapChainImages.size());
	secondDescriptorSets.resize(swapChainImages.size());
	ShaderDescriptorSet.resize(swapChainImages.size());

	if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
	if (vkAllocateDescriptorSets(device, &allocInfo, secondDescriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}
	if (vkAllocateDescriptorSets(device, &ShadeAallocInfo, ShaderDescriptorSet.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniforms.uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorBufferInfo LightBufferInfo{};
		LightBufferInfo.buffer = uniforms.LightBuffers[i];
		LightBufferInfo.offset = 0;
		LightBufferInfo.range = sizeof(LightUniforms);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		VkWriteDescriptorSet LightDescriptorWrite{};
		LightDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		LightDescriptorWrite.dstSet = descriptorSets[i];
		LightDescriptorWrite.dstBinding = 1;
		LightDescriptorWrite.dstArrayElement = 0;
		LightDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		LightDescriptorWrite.descriptorCount = 1;
		LightDescriptorWrite.pBufferInfo = &LightBufferInfo;

		std::array<VkWriteDescriptorSet, 2> writingDescriptors = {descriptorWrite, LightDescriptorWrite};

		vkUpdateDescriptorSets(device, 2, writingDescriptors.data(), 0, nullptr);
	}
	
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniforms.secondBuffer[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorBufferInfo LightBufferInfo{};
		LightBufferInfo.buffer = uniforms.LightBuffers[i];
		LightBufferInfo.offset = 0;
		LightBufferInfo.range = sizeof(LightUniforms);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = secondDescriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		VkWriteDescriptorSet LightDescriptorWrite{};
		LightDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		LightDescriptorWrite.dstSet = secondDescriptorSets[i];
		LightDescriptorWrite.dstBinding = 1;
		LightDescriptorWrite.dstArrayElement = 0;
		LightDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		LightDescriptorWrite.descriptorCount = 1;
		LightDescriptorWrite.pBufferInfo = &LightBufferInfo;

		std::array<VkWriteDescriptorSet, 2> writingDescriptors = {descriptorWrite, LightDescriptorWrite};

		vkUpdateDescriptorSets(device, 2, writingDescriptors.data(), 0, nullptr);
	}

	//Obstacle objects
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniforms.ObstaclesUniform[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(ObstacleUniformBufferObject);

		VkDescriptorBufferInfo LightBufferInfo{};
		LightBufferInfo.buffer = uniforms.LightBuffers[i];
		LightBufferInfo.offset = 0;
		LightBufferInfo.range = sizeof(LightUniforms);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = secondDescriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		VkWriteDescriptorSet LightDescriptorWrite{};
		LightDescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		LightDescriptorWrite.dstSet = secondDescriptorSets[i];
		LightDescriptorWrite.dstBinding = 1;
		LightDescriptorWrite.dstArrayElement = 0;
		LightDescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		LightDescriptorWrite.descriptorCount = 1;
		LightDescriptorWrite.pBufferInfo = &LightBufferInfo;

		std::array<VkWriteDescriptorSet, 2> writingDescriptors = {descriptorWrite, LightDescriptorWrite};

		vkUpdateDescriptorSets(device, 2, writingDescriptors.data(), 0, nullptr);
	}

	//Shader
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniforms.shaderUniform[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(ShaderUniform);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = ShaderDescriptorSet[i];
		descriptorWrite.dstBinding = 3;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		std::array<VkWriteDescriptorSet, 1> writingDescriptors = {descriptorWrite};

		vkUpdateDescriptorSets(device, 1, writingDescriptors.data(), 0, nullptr);
	}

}