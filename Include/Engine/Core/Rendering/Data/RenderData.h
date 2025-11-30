#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "Engine/Core/Rendering/Data/Vertex.h"
#include "Engine/Utils/Math/AllMath.h"


  // Данные для одного рендер-объекта
  struct RenderObject
  {
    const FStaticMesh* mesh;
    CEMath::Matrix4f transform;
    CEMath::Vector3f color;
  };

  // Данные камеры для рендеринга
  struct CameraData
  {
    CEMath::Matrix4f viewMatrix;
    CEMath::Matrix4f projectionMatrix;
    CEMath::Vector3f position;

    CameraData() : viewMatrix(1.0f), projectionMatrix(1.0f), position(0.0f)
    {
    }
  };

  // UBO структуры для шейдеров
  struct SceneUBO
  {
    CEMath::Matrix4f view;
    CEMath::Matrix4f proj;
    CEMath::Vector3f cameraPos;

    float padding[13];
  };

  struct ModelUBO
  {
    CEMath::Matrix4f model;    
    CEMath::Vector4f color;
    float padding[8];
  };

  struct LightingUBO
  {
    CEMath::Vector4f lightPositions[4];
    CEMath::Vector4f lightColors[4];
    
    CEMath::Vector4f ambientColor;  
    int lightCount;
    float padding[3];  

    LightingUBO()
    {
      lightCount = 0;
      ambientColor = CEMath::Vector4f(0.0f);
      for (int i = 0; i < 4; i++)
      {
        lightPositions[i] = CEMath::Vector4f(0.0f);
        lightColors[i] = CEMath::Vector4f(0.0f);
      }
      padding[0] = 0.0f;
      padding[1] = 0.0f;
      padding[2] = 0.0f;
    }
  };


  struct FrameRenderData
  {
    CameraData camera;
    std::vector<RenderObject> renderObjects;

    LightingUBO lighting;

    static_assert(sizeof(LightingUBO) == 160, "LightingUBO size should be 160 bytes");
    static_assert(offsetof(LightingUBO, lightPositions) == 0, "lightPositions offset mismatch");
    static_assert(offsetof(LightingUBO, lightColors) == 64, "lightColors offset mismatch");
    static_assert(offsetof(LightingUBO, ambientColor) == 128, "ambientColor offset mismatch");
    static_assert(offsetof(LightingUBO, lightCount) == 144, "lightCount offset mismatch");

    void Clear()
    {
      camera = CameraData();
      renderObjects.clear();
      lighting = LightingUBO();
    }

    void AddRenderObject(const RenderObject& object)
    {
      renderObjects.push_back(object);
    }

    void SetCameraData(const CameraData& camData)
    {
      camera = camData;
    }

    
    SceneUBO GetSceneUBO() const
    {
      SceneUBO ubo{};
      ubo.view = camera.viewMatrix;
      ubo.proj = camera.projectionMatrix;
      ubo.cameraPos = camera.position;
      return ubo;
    }

    ModelUBO GetModelUBO(const CEMath::Matrix4f& modelMatrix, const CEMath::Vector3f& meshColor) const
    {
      ModelUBO ubo{};
      ubo.model = modelMatrix;
      ubo.color = CEMath::Vector4f(meshColor, 1.0f);
      return ubo;
    }

    // Настройка освещения по умолчанию
    void SetupDefaultLighting()
    {
      lighting.lightCount = 1;
      lighting.lightPositions[0] = CEMath::Vector4f(0.0f, 5.0f, 5.0f, 1.0f);  // Точечный свет
      lighting.lightColors[0] = CEMath::Vector4f(1.0f, 1.0f, 1.0f, 1.0f);     // Белый свет

      lighting.ambientColor = CEMath::Vector4f(0.2f, 0.2f, 0.2f, .3f);  // Слабый ambient
    }
  };
