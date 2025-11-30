#pragma once
#include <vulkan/vulkan.h>

#include <array>
#include <glm/glm.hpp>
#include <vector>
#include "Engine/Utils/Math/AllMath.h"


  struct Vertex
  {
    CEMath::Vector3f position;
    CEMath::Vector3f  normal;
    CEMath::Vector3f  color;
    CEMath::Vector2f  texCoord;
    Vertex()
    {
    }
    Vertex(const CEMath::Vector3f& pos, const CEMath::Vector3f& norm = CEMath::Vector3f(0.0f), const CEMath::Vector2f& uv = CEMath::Vector2f(0.0f))
        : position(pos), normal(norm), texCoord(uv)
    {
    }
    Vertex(const CEMath::Vector3f& pos, const CEMath::Vector3f& norm = CEMath::Vector3f(0.0f),
           const CEMath::Vector3f& col = CEMath::Vector3f(1.0f), const CEMath::Vector2f& uv = CEMath::Vector2f(0.0f))
        : position(pos), normal(norm), color(col), texCoord(uv)
    {
    }

    static VkVertexInputBindingDescription GetBindingDescription();

    static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions();
    bool operator==(const Vertex& other) const
    {
      return position == other.position &&
             normal == other.normal &&
             color == other.color &&
             texCoord == other.texCoord;
    }
  };

  // Простой меш - массив вершин и индексов
  struct FStaticMesh
  {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Матрица трансформации для рендеринга
    CEMath::Matrix4f transform{1.0f};

    // Материал (пока базовый)
    CEMath::Vector3f color{1.0f, 1.0f, 1.0f};

    FStaticMesh() = default;
    FStaticMesh(const std::vector<Vertex>& verts, const std::vector<uint32_t>& inds)
        : vertices(verts), indices(inds)
    {
    }
  };
