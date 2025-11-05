#pragma once

#include <vulkan/vulkan.h>

#include <array>
#include <glm/glm.hpp>

struct Vertex
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec3 color;
  glm::vec2 texCoord;

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