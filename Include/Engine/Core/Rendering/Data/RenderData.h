#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "Engine/Core/Rendering/Data/Vertex.h"
#include "Engine/Core/CoreTypes.h"


  // Данные для одного рендер-объекта
  struct RenderObject
  {
    const FStaticMesh* mesh;
    FMatrix transform;
    FVector color;
  };

  // Данные камеры для рендеринга
  struct CameraData
  {
    FMatrix viewMatrix;
    FMatrix projectionMatrix;
    FVector position;

    CameraData() : viewMatrix(1.0f), projectionMatrix(1.0f), position(0.0f)
    {
    }
  };

  // UBO структуры для шейдеров
  struct SceneUBO
  {
    FMatrix view;
    FMatrix proj;
    FVector cameraPos;

    float padding[13];
  };

  struct ModelUBO
  {
    FMatrix model;
    FVector4 color;
    float padding[8];
  };

  struct LightingUBO
  {
    FVector4 lightPositions[4];
    FVector4 lightColors[4];

    FVector4 ambientColor;
    int lightCount;
    float padding[3];

    LightingUBO()
    {
      lightCount = 0;
      ambientColor = FVector4(0.0f);
      for (int i = 0; i < 4; i++)
      {
        lightPositions[i] = FVector4(0.0f);
        lightColors[i] = FVector4(0.0f);
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

    ModelUBO GetModelUBO(const FMatrix& modelMatrix, const FVector& meshColor) const
    {
      ModelUBO ubo{};
      ubo.model = modelMatrix;
      ubo.color = FVector4(meshColor, 1.0f);
      return ubo;
    }

    // Настройка освещения по умолчанию
    void SetupDefaultLighting()
    {
      lighting.lightCount = 1;
      lighting.lightPositions[0] = FVector4(0.0f, 5.0f, 5.0f, 1.0f);  // Точечный свет
      lighting.lightColors[0] = FVector4(1.0f, 1.0f, 1.0f, 1.0f);     // Белый свет

      lighting.ambientColor = FVector4(0.2f, 0.2f, 0.2f, .3f);  // Слабый ambient
    }
  };
