#pragma once
#include <vulkan/vulkan.h>

#include <array>
#include <glm/glm.hpp>
#include <vector>
#include "Engine/Core/CoreTypes.h"


  struct Vertex
  {
    FVector position;
    FVector  normal;
    FVector  color;
    FVector2D  texCoord;
    Vertex()
    {
    }
    Vertex(const FVector& pos, const FVector& norm = FVector(0.0f), const FVector2D& uv = FVector2D(0.0f))
        : position(pos), normal(norm), texCoord(uv)
    {
    }
    Vertex(const FVector& pos, const FVector& norm = FVector(0.0f),
           const FVector& col = FVector(1.0f), const FVector2D& uv = FVector2D(0.0f))
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

    bool operator<(const Vertex& other) const
    {
      if (position != other.position) return position < other.position;
      if (normal != other.normal) return normal < other.normal;
      if (color != other.color) return color < other.color;
      return texCoord < other.texCoord;
    }
  };

  
  struct FStaticMesh
  {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    
    FMatrix transform{1.0f};

   
    FVector color{1.0f, 1.0f, 1.0f};

    FStaticMesh() = default;
    FStaticMesh(const std::vector<Vertex>& verts, const std::vector<uint32_t>& inds)
        : vertices(verts), indices(inds)
    {
    }
  };
