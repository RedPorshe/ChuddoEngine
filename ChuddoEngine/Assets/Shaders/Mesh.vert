#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUV;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragWorldPos;

// Push constants для матриц
layout(push_constant) uniform PushConstants {
    mat4 view;
    mat4 projection;
    mat4 model;
} pc;

void main()
{
    vec4 worldPos = pc.model * vec4(inPosition, 1.0);
    gl_Position = pc.projection * pc.view * worldPos;
    
    fragColor = inColor;
    fragUV = inUV;
    fragNormal = mat3(pc.model) * inNormal; // Преобразуем нормаль
    fragWorldPos = worldPos.xyz;
}