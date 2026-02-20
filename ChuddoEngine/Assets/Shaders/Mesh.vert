#version 450

// Входные атрибуты - должны соответствовать Vertex структуре
layout(location = 0) in vec3 inPosition;  // из Position
layout(location = 1) in vec3 inNormal;    // из Normal
layout(location = 2) in vec4 inColor;     // из Color
layout(location = 3) in vec2 inUV;         // из UV

// Выходные данные для фрагментного шейдера
layout(location = 0) out vec3 vColor;
layout(location = 1) out vec2 vUV;
layout(location = 2) out vec3 vNormal;
layout(location = 3) out vec3 vWorldPos;

// Uniforms для трансформаций (добавьте если нужны)
//layout(binding = 0) uniform UniformBufferObject {
  //  mat4 model;
   // mat4 view;
   // mat4 proj;
//} ubo;

void main()
{
    // Простая передача позиции (без трансформаций)
    gl_Position = vec4(inPosition, 1.0);
    
    // Передаём данные во фрагментный шейдер
    vColor = inColor.rgb;  // или inColor если нужна альфа
    vUV = inUV;
    vNormal = inNormal;
    vWorldPos = inPosition;
}