#version 450

// Входные атрибуты
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inUV;

// Push constants для матриц и параметров террейна
layout(push_constant) uniform PushConstants {
    mat4 view;
    mat4 projection;
    mat4 model;
    // Дополнительные параметры террейна
    vec4 terrainParams; // x = tiling factor, y = height scale, z = fog density, w = unused
} push;

// Выходные данные для фрагментного шейдера
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragUV;
layout(location = 2) out vec3 fragNormal;
layout(location = 3) out vec3 fragWorldPos;
layout(location = 4) out float fragHeight;

void main() {
    // Мировая позиция
    vec4 worldPos = push.model * vec4(inPosition, 1.0);
    fragWorldPos = worldPos.xyz;
    
    // Высота для цветовых градиентов
    fragHeight = inPosition.y;
    
      mat3 normalMatrix = transpose(inverse(mat3(push.model)));
    fragNormal = normalize(normalMatrix * inNormal);
    
    // UV координаты с tiling
    fragUV = inUV * push.terrainParams.x;
    
    // Цвет (можно использовать inColor или вычислять на основе высоты)
    fragColor = inColor;
    
    // Финальная позиция
    gl_Position = push.projection * push.view * worldPos;
}