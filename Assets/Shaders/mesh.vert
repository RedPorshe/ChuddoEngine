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
    vec4 color;
} model;

void main() {
    // Преобразование позиции в мировые координаты
    vec4 worldPosition = model.model * vec4(inPosition, 1.0);
    gl_Position = scene.proj * scene.view * worldPosition;
    
    // Передаем цвет меша из ModelUBO (устанавливается в коде при отрисовке)
    fragColor = model.color.rgb;
    
    // Преобразование нормалей в мировое пространство
    mat3 normalMatrix = mat3(transpose(inverse(model.model)));
    fragNormal = normalize(normalMatrix * inNormal);
    
    // Позиция в мировых координатах для расчета освещения
    fragPos = worldPosition.xyz;
}