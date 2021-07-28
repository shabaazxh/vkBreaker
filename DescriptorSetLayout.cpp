#include "DescriptorSetLayout.h"

DescriptorSetLayout::DescriptorSetLayout(VkDevice device) {
	this->device = device;
}

void DescriptorSetLayout::createDescriptorSetLayout() {

	//Describe the use and which stage
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutBinding LightLayoutBinding{};
	LightLayoutBinding.binding = 1;
	LightLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	LightLayoutBinding.descriptorCount = 1;
	LightLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	LightLayoutBinding.pImmutableSamplers = nullptr;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = {uboLayoutBinding, LightLayoutBinding};

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 2;
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}

	//Shader descriptor layout
	VkDescriptorSetLayoutBinding ShaderUniformBinding{};
	ShaderUniformBinding.binding = 3;
	ShaderUniformBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	ShaderUniformBinding.descriptorCount = 1;
	ShaderUniformBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	ShaderUniformBinding.pImmutableSamplers = nullptr;

	std::array<VkDescriptorSetLayoutBinding, 1> LayoutBinding = { ShaderUniformBinding };

	VkDescriptorSetLayoutCreateInfo shaderLayoutInfo{};
	shaderLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	shaderLayoutInfo.bindingCount = 1;
	shaderLayoutInfo.pBindings = LayoutBinding.data();

	if (vkCreateDescriptorSetLayout(device, &shaderLayoutInfo, nullptr, &shaderDescriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}

}
