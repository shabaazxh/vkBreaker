#version 450

layout(binding = 0) uniform ObstacleUniformBufferObject {
    mat4 model[30];
    mat4 view;
    mat4 proj;
    float time;
}ubo;

layout(binding = 1)  uniform LightUniform {
    vec3 LightColor;
    vec3 ObjectColor; 
    vec3 LightPosition;
}LightUBO;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 FragPos;

layout(location = 0) out vec4 outColor;


void main() {
    float ambientStrength = 0.9;
    vec3 ambient = ambientStrength * LightUBO.LightColor;

    //diffuse lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(LightUBO.LightPosition - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * LightUBO.LightColor;

    vec3 fragColor = vec3(ubo.time * 0.7,0.7, 1.0);

    vec3 result = (ambient + diffuse) * LightUBO.ObjectColor;
    outColor = vec4(fragColor,1.0);
}