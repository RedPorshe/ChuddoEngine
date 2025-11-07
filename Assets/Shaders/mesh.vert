#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragNormal;
layout(location = 2) out vec3 fragPos;

layout(binding = 0) uniform SceneUBO {
    mat4 view;
    mat4 proj;
    vec3 cameraPos;
} scene;

layout(binding = 1) uniform ModelUBO {
    mat4 model;
} model;

void main() {
    vec4 worldPos = model.model * vec4(inPosition, 1.0);
    gl_Position = scene.proj * scene.view * worldPos;
    
    fragColor = inColor;
    fragNormal = mat3(model.model) * inNormal;
    fragPos = worldPos.xyz;
}