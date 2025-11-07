#pragma once
#include <vulkan/vulkan.h>

#include <array>
#include <glm/glm.hpp>
#include <vector>

namespace CE
{
  struct Vertex
  {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 color;
    glm::vec2 texCoord;
    Vertex()
    {
    }
    Vertex(const glm::vec3& pos, const glm::vec3& norm = glm::vec3(0.0f), const glm::vec2& uv = glm::vec2(0.0f))
        : position(pos), normal(norm), texCoord(uv)
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
    glm::mat4 transform{1.0f};

    // Материал (пока базовый)
    glm::vec3 color{1.0f, 1.0f, 1.0f};

    StaticMesh() = default;
    StaticMesh(const std::vector<Vertex>& verts, const std::vector<uint32_t>& inds)
        : vertices(verts), indices(inds)
    {
    }
  };
}  // namespace CE