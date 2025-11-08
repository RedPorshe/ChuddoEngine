#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "Rendering/Data/Vertex.h"

namespace CE
{
  // Данные для одного рендер-объекта
  struct RenderObject
  {
    const StaticMesh* mesh;
    glm::mat4 transform;
    glm::vec3 color;
  };

  // Данные камеры для рендеринга
  struct CameraData
  {
    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;
    glm::vec3 position;

    CameraData() : viewMatrix(1.0f), projectionMatrix(1.0f), position(0.0f)
    {
    }
  };

  // UBO структуры для шейдеров
  struct SceneUBO
  {
    glm::mat4 view;
    glm::mat4 proj;
    glm::vec3 cameraPos;
    // Выравнивание до 256 байт
    float padding[13];  // 16 floats * 4 bytes = 64 bytes (mat4=64, vec3=12, padding=52)
  };

  struct ModelUBO
  {
    glm::mat4 model;
    // Выравнивание
    float padding[12];  // mat4=64 bytes
  };

  struct LightingUBO
  {
    glm::vec4 lightPositions[4];  // w=1 для позиций, w=0 для направлений
    glm::vec4 lightColors[4];
    float lightIntensities[4];
    glm::vec4 ambientColor;
    float ambientIntensity;
    int lightCount;
    // Выравнивание
    float padding[14];
  };

  // Собранные данные для рендеринга кадра
  struct FrameRenderData
  {
    CameraData camera;
    std::vector<RenderObject> renderObjects;

    // Данные для шейдеров
    LightingUBO lighting;

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

    // Методы для получения UBO
    SceneUBO GetSceneUBO() const
    {
      SceneUBO ubo{};
      ubo.view = camera.viewMatrix;
      ubo.proj = camera.projectionMatrix;
      ubo.cameraPos = camera.position;
      return ubo;
    }

    ModelUBO GetModelUBO(const glm::mat4& modelMatrix) const
    {
      ModelUBO ubo{};
      ubo.model = modelMatrix;
      return ubo;
    }

    // Настройка освещения по умолчанию
    void SetupDefaultLighting()
    {
      lighting.lightCount = 1;
      lighting.lightPositions[0] = glm::vec4(0.0f, 5.0f, 5.0f, 1.0f);  // Точечный свет
      lighting.lightColors[0] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);     // Белый свет
      lighting.lightIntensities[0] = 1.0f;
      lighting.ambientColor = glm::vec4(0.2f, 0.2f, 0.2f, 1.0f);  // Слабый ambient
      lighting.ambientIntensity = 0.3f;
    }
  };
}  // namespace CE