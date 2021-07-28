#version 450

layout(binding = 0) uniform ObstacleUniformBufferObject {
    mat4 model[30];
    mat4 view;
    mat4 proj;
    float time;
}ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 aNormals;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec3 FragPos;

void main() {
    FragPos = vec3(ubo.model[gl_InstanceIndex] * vec4(inPosition, 1.0));
    gl_Position = ubo.proj * ubo.view * ubo.model[gl_InstanceIndex] * vec4(inPosition, 1.0);
    fragColor = inColor;
    Normal = aNormals;
}