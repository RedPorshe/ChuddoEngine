#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragUV;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragWorldPos;
layout(location = 4) in vec3 fragViewDir;

layout(push_constant) uniform PushConstants {
    mat4 view;
    mat4 projection;
    mat4 model;
} push;

layout(location = 0) out vec4 outColor;

// Параметры освещения
const vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3)); // Направление света
const vec3 lightColor = vec3(1.0, 1.0, 1.0); // Белый свет
const float lightIntensity = 1.2;

const vec3 ambientColor = vec3(0.2, 0.2, 0.3);
const float ambientIntensity = 0.3;

// Параметры материала
const float specularStrength = 0.5;
const float shininess = 32.0;

void main() {
    // Нормализуем входные векторы
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(fragViewDir);
    
    // Ambient (фоновое освещение)
    vec3 ambient = ambientColor * ambientIntensity;
    
    // Diffuse (диффузное освещение)
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * lightColor * lightIntensity;
    
    // Specular (блики) - упрощённо
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = spec * specularStrength * lightColor;
    
    // Цвет объекта
    vec3 objectColor = fragColor;
    
    // Комбинируем всё вместе
    vec3 finalColor = objectColor * (ambient + diffuse) + specular;
    
    // Простая гамма-коррекция
    finalColor = pow(finalColor, vec3(1.0/2.2));
    
    outColor = vec4(finalColor, 1.0);
}