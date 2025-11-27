#pragma once
#include <vulkan/vulkan.h>

#include <array>
#include <glm/glm.hpp>
#include <vector>
#include "Engine/Utils/Math/AllMath.h"

namespace CE
{
  struct Vertex
  {
    Math::Vector3f position;
    Math::Vector3f  normal;
    Math::Vector3f  color;
    Math::Vector2f  texCoord;
    Vertex()
    {
    }
    Vertex(const Math::Vector3f& pos, const Math::Vector3f& norm = Math::Vector3f(0.0f), const Math::Vector2f& uv = Math::Vector2f(0.0f))
        : position(pos), normal(norm), texCoord(uv)
    {
    }
    Vertex(const Math::Vector3f& pos, const Math::Vector3f& norm = Math::Vector3f(0.0f),
           const Math::Vector3f& col = Math::Vector3f(1.0f), const Math::Vector2f& uv = Math::Vector2f(0.0f))
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
  struct StaticMesh
  {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    // Матрица трансформации для рендеринга
    Math::Matrix4f transform{1.0f};

    // Материал (пока базовый)
    Math::Vector3f color{1.0f, 1.0f, 1.0f};

    StaticMesh() = default;
    StaticMesh(const std::vector<Vertex>& verts, const std::vector<uint32_t>& inds)
        : vertices(verts), indices(inds)
    {
    }
  };
}  // namespace CE