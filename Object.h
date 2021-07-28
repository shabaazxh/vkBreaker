#pragma once
#include "lib/glm/glm/glm.hpp"
#include <Vulkan/vulkan.h>
#include <array>
#include <vector>

struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec3 Normals;

	VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescriptions{};
		bindingDescriptions.binding = 0;
		bindingDescriptions.stride = sizeof(Vertex);
		bindingDescriptions.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescriptions;
	}

	std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		attributeDescriptions[2].binding = 0;
		attributeDescriptions[2].location = 2;
		attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[2].offset = offsetof(Vertex, Normals);

		return attributeDescriptions;
	}
};

struct UniformBufferObject {
	glm::mat4 model;
	glm::mat4 view;
	glm::mat4 proj;
	float time;
};

struct ObstacleUniformBufferObject {
	glm::mat4 model[30];
	glm::mat4 view;
	glm::mat4 proj;
	float time;
};

struct LightUniforms {
	glm::vec4 LightColor;
	glm::vec4 ObjectColor;
	glm::vec3 LightPosition;
};

struct ShaderUniform {
	float time;
};

inline glm::vec3 cubePositions[] = {
    glm::vec3(-2.0f,  0.0f, 0.0f), 
    glm::vec3( 1.f,   1.1f, 0.0f), 
    glm::vec3(-1.5f, -1.2f, 0.0f),  
    glm::vec3(-3.0f, -1.0f, 0.0f),  
    glm::vec3( 2.4f, -0.4f, 0.0f),  
    glm::vec3(-1.9f,  1.9f, 0.0f),  
    glm::vec3( 1.3f, -1.0f, 0.0f),  
    glm::vec3( 1.5f,  2.0f, 0.0f), 
    glm::vec3( 2.1f,  1.2f, 0.0f), 
    glm::vec3(-1.3f,  1.0f, 0.0f)  
};

class Object {
public:
	Vertex getInstance() { return instance; }
	std::vector<Vertex> getTriangleData() { return vertices; }
	std::vector<Vertex> getCubeVertexData() { return cubeVert;}

	std::vector<Vertex> getBackgroundData() { return background;}

	std::vector<uint32_t> getIndexData() { return indices; }
private:

	Vertex instance;
	std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}
	};

	std::vector<Vertex> background = {
	{{-1.5f, -1.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
	{{1.5f, -1.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
	{{1.5f, 1.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
	{{-1.5f, 1.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}
	};

	std::vector<Vertex> cubeVert = {
		{{-0.5f, -0.5f, -0.5f}, {.9f, .9f, .9f}, {0.0f,  0.0f, -1.0f}},
		{{0.5f, -0.5f, -0.5f}, {.9f, .9f, .9f}, {0.0f,  0.0f, -1.0f}},
		{{0.5f,  0.5f, -0.5f}, {.9f, .9f, .9f}, {0.0f,  0.0f, -1.0f}},
		{{0.5f,  0.5f, -0.5f}, {.9f, .9f, .9f}, {0.0f,  0.0f, -1.0f}},
		{{-0.5f,  0.5f, -0.5f}, {.9f, .9f, .9f}, {0.0f,  0.0f, -1.0f}},
		{{-0.5f, -0.5f, -0.5f}, {.9f, .9f, .9f}, {0.0f,  0.0f, -1.0f}},

		// right face (yellow)
		{{-0.5f, -0.5f,  0.5f}, {.8f, .8f, .1f}, {0.0f,  0.0f, 1.0f}},
		{{0.5f, -0.5f,  0.5f}, {.8f, .8f, .1f}, {0.0f,  0.0f, 1.0f}},
		{{0.5f,  0.5f,  0.5f}, {.8f, .8f, .1f}, {0.0f,  0.0f, 1.0f}},
		{{0.5f,  0.5f,  0.5f}, {.8f, .8f, .1f}, {0.0f,  0.0f, 1.0f}},
		{{-0.5f, 0.5f,  0.5f}, {.8f, .8f, .1f}, {0.0f,  0.0f, 1.0f}},
		{{-0.5f, -0.5f,  0.5f}, {.8f, .8f, .1f}, {0.0f,  0.0f, 1.0f}},

		// top face (orange, remember y axis points down)
		{{-0.5f,  0.5f,  0.5f}, {.9f, .6f, .1f}, {-1.0f,  0.0f,  0.0f}},
		{{-0.5f,  0.5f, -0.5f}, {.9f, .6f, .1f}, {-1.0f,  0.0f,  0.0f}},
		{{-0.5f, -0.5f, -0.5f}, {.9f, .6f, .1f}, {-1.0f,  0.0f,  0.0f}},
		{{-0.5f, -0.5f, -0.5f}, {.9f, .6f, .1f}, {-1.0f,  0.0f,  0.0f}},
		{{-0.5f, -0.5f,  0.5f}, {.9f, .6f, .1f}, {-1.0f,  0.0f,  0.0f}},
		{{-0.5f,  0.5f,  0.5f}, {.9f, .6f, .1f}, {-1.0f,  0.0f,  0.0f}},

		// bottom face (red)
		{{0.5f,  0.5f,  0.5f}, {.8f, .1f, .1f}, {1.0f,  0.0f,  0.0f}},
		{{0.5f,  0.5f, -0.5f}, {.8f, .1f, .1f}, {1.0f,  0.0f,  0.0f}},
		{{0.5f, -0.5f, -0.5f}, {.8f, .1f, .1f}, {1.0f,  0.0f,  0.0f}},
		{{0.5f, -0.5f, -0.5f}, {.8f, .1f, .1f}, {1.0f,  0.0f,  0.0f}},
		{{0.5f, -0.5f,  0.5f}, {.8f, .1f, .1f}, {1.0f,  0.0f,  0.0f}},
		{{0.5f,  0.5f,  0.5f}, {.8f, .1f, .1f}, {1.0f,  0.0f,  0.0f}},

		// nose face (blue)
		{{-0.5f, -0.5f, -0.5f}, {.1f, .1f, .8f}, { 0.0f, -1.0f,  0.0f}},
		{{0.5f, -0.5f,  -0.5f}, {.1f, .1f, .8f}, {0.0f, -1.0f,  0.0f}},
		{{0.5f, -0.5f,  0.5f}, {.1f, .1f, .8f}, { 0.0f, -1.0f,  0.0f}},
		{{0.5f, -0.5f,  0.5f}, {.1f, .1f, .8f}, { 0.0f, -1.0f,  0.0f}},
		{{-0.5f, -0.5f, 0.5f}, {.1f, .1f, .8f}, { 0.0f, -1.0f,  0.0f}},
		{{-0.5f, -0.5f, -0.5f}, {.1f, .1f, .8f}, { 0.0f, -1.0f,  0.0f}},

		// tail face (green)
		{{-0.5f,  0.5f, -0.5f}, {.1f, .8f, .1f}, {0.0f,  1.0f,  0.0f}},
		{{0.5f,  0.5f, -0.5f}, {.1f, .8f, .1f}, {0.0f,  1.0f,  0.0f}},
		{{0.5f,  0.5f,  0.5f}, {.1f, .8f, .1f}, {0.0f,  1.0f,  0.0f}},
		{{0.5f,  0.5f,  0.5f}, {.1f, .8f, .1f}, {0.0f,  1.0f,  0.0f}},
		{{-0.5f,  0.5f,  0.5f}, {.1f, .8f, .1f}, {0.0f,  1.0f,  0.0f}},
		{{-0.5f,  0.5f, -0.5f}, {.1f, .8f, .1f}, {0.0f,  1.0f,  0.0f}},
		};

	std::vector<uint32_t> indices = {
		0,1,2,2,3,0
	};

};