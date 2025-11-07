#version 450
#extension GL_ARB_separate_shader_objects : enable

// ВХОДНЫЕ ДАННЫЕ ИЗ ВЕРШИННОГО ШЕЙДЕРА
layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

// UNIFORM БУФЕРЫ
layout(binding = 0) uniform SceneUBO {
    mat4 view;
    mat4 proj;
    vec3 cameraPos;
} scene;

layout(binding = 2) uniform LightingUBO {
    vec4 lightPositions[4];
    vec4 lightColors[4];
    float lightIntensities[4];
    vec4 ambientColor;
    float ambientIntensity;
    int lightCount;
} lighting;

void main() {
    // Нормализуем нормаль и направление взгляда
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(scene.cameraPos - fragPos);
    
    // Ambient компонент
    vec3 ambient = lighting.ambientColor.rgb * lighting.ambientIntensity;
    
    vec3 result = vec3(0.0);
    
    // Обрабатываем все источники света
    for(int i = 0; i < lighting.lightCount; i++) {
        // Вектор от поверхности к свету
        vec3 lightDir = normalize(lighting.lightPositions[i].xyz - fragPos);
        
        // Diffuse компонент (Lambert)
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * lighting.lightColors[i].rgb * lighting.lightIntensities[i];
        
        result += diffuse;
    }
    
    // Финальный цвет = ambient + diffuse от всех источников
    vec3 finalColor = (ambient + result) * fragColor;
    outColor = vec4(finalColor, 1.0);
}