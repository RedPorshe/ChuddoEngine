#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragNormal;
layout(location = 2) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

layout(binding = 0) uniform SceneUBO {
    mat4 view;
    mat4 proj;
    vec3 cameraPos;
} scene;

layout(binding = 2) uniform LightingUBO {
    vec4 lightPositions[4];
    vec4 lightColors[4];  
    vec4 ambientColor;    
    int lightCount;
} lighting;

float calculateAttenuation(float distance) {
    return 1.0 / (1.0 + 0.022 * distance + 0.0019 * distance * distance);
}
vec3 calculateDiffuse(vec3 normal, vec3 lightDir, vec3 lightColor, float lightIntensity, float distance) {
    float diff = max(dot(normal, lightDir), 0.0);
    float attenuation = calculateAttenuation(distance);
    return lightColor * diff * lightIntensity * attenuation;
}


void main() {
     vec3 albedo = fragColor;
    vec3 normal = normalize(fragNormal);
    
    // Ambient освещение
    vec3 ambient = lighting.ambientColor.rgb * lighting.ambientColor.w;
    
    // Fallback ambient если все нули
    if (lighting.lightCount == 0 && length(ambient) < 0.01) {
        ambient = vec3(0.1); // Минимальный ambient
    }

    vec3 result = ambient;
    
    // Обработка всех источников света
    for(int i = 0; i < lighting.lightCount && i < 4; i++) {
        vec3 lightVec = lighting.lightPositions[i].xyz - fragPos;
        float distance = length(lightVec);
        vec3 lightDir = normalize(lightVec);
        
        vec3 diffuse = calculateDiffuse(normal, lightDir, 
                                      lighting.lightColors[i].rgb, 
                                      lighting.lightColors[i].w, 
                                      distance);
        result += diffuse;
    }
    
    // Rim lighting (подсветка краев)
    vec3 viewDir = normalize(scene.cameraPos - fragPos);
    float rim = pow(1.0 - max(dot(viewDir, normal), 0.0), 2.0);
    vec3 rimLight = vec3(0.3) * rim;
    result += rimLight;

    // Финальный цвет
    vec3 finalColor = result * albedo;
    
    // Добавляем базовый цвет для видимости даже при слабом освещении
    finalColor += 0.15 * albedo;
    
    // Тоновая компрессия и гамма коррекция
    finalColor = finalColor / (finalColor + vec3(1.0));
    finalColor = pow(finalColor, vec3(0.4545));
    
    outColor = vec4(finalColor, 1.0);
}