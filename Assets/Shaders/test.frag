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

// ModelUBO удален из fragment шейдера - он нужен только в vertex
// layout(binding = 1) uniform ModelUBO {
//     mat4 model;
// } model;

layout(binding = 2) uniform LightingUBO {
    vec4 lightPositions[4];
    vec4 lightColors[4];
    float lightIntensities[4];
    vec4 ambientColor;
    float ambientIntensity;
    int lightCount;
} lighting;

void main() {

   
    vec3 zerocolor = vec3(0.0);
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(scene.cameraPos - fragPos);
   
   
   if (lighting.ambientColor.rgb != zerocolor)
   {
    vec3 ambient = lighting.ambientColor.rgb * lighting.ambientIntensity;
    
    vec3 result = vec3(0.0);
    
    if (lighting.lightCount > 1)
    {
    for(int i = 0; i < lighting.lightCount; i++) 
        {
       
        vec3 lightDir = normalize(lighting.lightPositions[i].xyz - fragPos);
        
       
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = diff * lighting.lightColors[i].rgb * lighting.lightIntensities[i];
        
        result += diffuse;
       }
    }
    else
    if (lighting.lightCount == 1)
    {
        vec3 lightaDir = normalize(lighting.lightPositions[0].xyz - fragPos);
         float difff = max(dot(normal,lightaDir), 0.0);
         vec3 diffuses = difff * lighting.lightColors[0].rgb * lighting.lightIntensities[0];
         result += diffuses;
    }
    
    // Финальный цвет = ambient + diffuse от всех источников
    vec3 finalColor = (ambient + result) * fragColor;
   
   if (finalColor == zerocolor) 
   {
    outColor = vec4(1.0,1.0,1.0, 1.0);
   }
   else
   {
    outColor = vec4(finalColor, 1.0);
   }
   }
   else
   {
outColor = vec4(1.0,0.0,0.0, 1.0); //red if abientcolor == zerocolor
   }
    
    
}