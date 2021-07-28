cmake_minimum_required(VERSION 3.13)
project(graphicsproject)

set(CMAKE_CXX_STANDARD 17)
find_package(Vulkan REQUIRED)
add_subdirectory(lib/glfw-3.3.4)
add_subdirectory(lib/tinyobjloader)

add_executable(graphicsproject
main.cpp 
Buffer.cpp 
commandBuffers.cpp 
 commandPool.cpp 
Descriptors.cpp 
 DescriptorSetLayout.cpp 
Framebuffers.cpp 
Object.cpp 
pipeline.cpp 
Renderer.cpp 
RenderPass.cpp 
shader.cpp
AABB.h
)
target_link_libraries(graphicsproject glfw ${GLFW_LIBRARIES} Vulkan::Vulkan)
