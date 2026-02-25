#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUV;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragWorldPos;

layout(location = 0) out vec4 outColor;

// Простое освещение (временное)
layout(push_constant) uniform PushConstants {
    mat4 view;
    mat4 projection;
    mat4 model;
} pc;

void main()
{
    // Простой свет сверху
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.5));
    float diff = max(dot(fragNormal, lightDir), 0.3);
    
    outColor = vec4(fragColor * diff, 1.0);
}