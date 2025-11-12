#pragma once
#include <glm/glm.hpp>
#include <vector>

#include "Engine/Core/Rendering/Data/Vertex.h"

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

    float padding[13];
  };

  struct ModelUBO
  {
    glm::mat4 model;

    // Добавляем цвет меша в UBO, чтобы передавать его в шейдер (выравнивание под std140)
    glm::vec4 color;

    float padding[8];
  };

  struct LightingUBO
  {
    glm::vec4 lightPositions[4];
    glm::vec4 lightColors[4];
    // We store intensity in the 'w' component of lightColors and ambientColor
    // to keep the layout compact and avoid a separate array. This matches
    // the shader convention where lightColors[i].w is the intensity and
    // ambientColor.w is the ambient intensity.
    glm::vec4 ambientColor;  // ambientColor.w == ambient intensity
    int lightCount;
    float padding[3];  // pad to 16 byte (std140-friendly)

    LightingUBO()
    {
      lightCount = 0;
      ambientColor = glm::vec4(0.0f);
      for (int i = 0; i < 4; i++)
      {
        lightPositions[i] = glm::vec4(0.0f);
        lightColors[i] = glm::vec4(0.0f);
      }
      padding[0] = 0.0f;
      padding[1] = 0.0f;
      padding[2] = 0.0f;
    }
  };

  // Собранные данные для рендеринга кадра
  struct FrameRenderData
  {
    CameraData camera;
    std::vector<RenderObject> renderObjects;

    LightingUBO lighting;

    // Статическая проверка размера и выравнивания
    // Updated size/offsets to match std140-like layout when intensities are
    // stored in the w component of vec4 colors. Layout is:
    // lightPositions[4] (0..63), lightColors[4] (64..127), ambientColor (128..143),
    // lightCount (144) + padding -> total size 160.
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

    // Методы для получения UBO
    SceneUBO GetSceneUBO() const
    {
      SceneUBO ubo{};
      ubo.view = camera.viewMatrix;
      ubo.proj = camera.projectionMatrix;
      ubo.cameraPos = camera.position;
      return ubo;
    }

    ModelUBO GetModelUBO(const glm::mat4& modelMatrix, const glm::vec3& meshColor) const
    {
      ModelUBO ubo{};
      ubo.model = modelMatrix;
      ubo.color = glm::vec4(meshColor, 1.0f);
      return ubo;
    }

    // Настройка освещения по умолчанию
    void SetupDefaultLighting()
    {
      lighting.lightCount = 1;
      lighting.lightPositions[0] = glm::vec4(0.0f, 5.0f, 5.0f, 1.0f);  // Точечный свет
      lighting.lightColors[0] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);     // Белый свет

      lighting.ambientColor = glm::vec4(0.2f, 0.2f, 0.2f, .3f);  // Слабый ambient
    }
  };
}  // namespace CE