#version 450

layout(location = 0) in vec3 vColor;
layout(location = 1) in vec2 vUV;
layout(location = 2) in vec3 vNormal;
layout(location = 3) in vec3 vWorldPos;

layout(location = 0) out vec4 outColor;

void main()
{
    // Просто выводим цвет
    outColor = vec4(vColor, 1.0);
    
    // Или можно использовать UV для текстур (позже)
    // outColor = vec4(vUV, 0.0, 1.0);
    
    // Или нормали для отладки
    // outColor = vec4(vNormal * 0.5 + 0.5, 1.0);
}