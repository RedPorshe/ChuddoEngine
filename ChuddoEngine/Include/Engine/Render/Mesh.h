#pragma once

#include <vector>
#include <cstdint>
#include <memory>
#include <array>
#include <cstddef>
#include <vulkan/vulkan.h>

// Simple vertex structure for test rendering
struct Vertex
{
    FVector Position;
    FVector Normal;
    FVector2D UV;

    // Return a binding description for this vertex type
    static VkVertexInputBindingDescription getBindingDescription()
    {
        VkVertexInputBindingDescription bindingDesc{};
        bindingDesc.binding = 0;
        bindingDesc.stride = sizeof(Vertex);
        bindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDesc;
    }

    // Return attribute descriptions: position, normal, uv
    static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions()
    {
        std::array<VkVertexInputAttributeDescription, 3> attribs{};

        attribs[0].location = 0;
        attribs[0].binding = 0;
        attribs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribs[0].offset = static_cast<uint32_t>(offsetof(Vertex, Position));

        attribs[1].location = 1;
        attribs[1].binding = 0;
        attribs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attribs[1].offset = static_cast<uint32_t>(offsetof(Vertex, Normal));

        attribs[2].location = 2;
        attribs[2].binding = 0;
        attribs[2].format = VK_FORMAT_R32G32_SFLOAT;
        attribs[2].offset = static_cast<uint32_t>(offsetof(Vertex, UV));

        return attribs;
    }
};

// Base mesh class holding vertices and indices
class Mesh
{
public:
    virtual ~Mesh() = default;

    const std::vector<Vertex>& GetVertices() const { return Vertices; }
    const std::vector<uint32_t>& GetIndices() const { return Indices; }

    void Clear()
    {
        Vertices.clear();
        Indices.clear();
    }

    public:
        // Non-const accessors for building mesh data (used by terrain generator)
        std::vector<Vertex> & GetVerticesRef () { return Vertices; }
        std::vector<uint32_t> & GetIndicesRef () { return Indices; }

    protected:
    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices;
};

// Static mesh with helper to create a cube
class CStaticMesh : public Mesh
{
public:
    CStaticMesh() = default;
    ~CStaticMesh() override = default;

    // Create a cube centered at origin with given size
    void CreateCubeMesh(float size = 1.0f)
    {
        Clear();
        float half = size * 0.5f;

        Vertex v{};
        // 0
        v.Position.x = -half; v.Position.y = -half; v.Position.z = -half; v.Normal.x = v.Normal.y = v.Normal.z = 0.f; v.UV.x = 0.f; v.UV.y = 0.f; Vertices.push_back(v);
        // 1
        v.Position.x = half; v.Position.y = -half; v.Position.z = -half; v.UV.x = 1.f; v.UV.y = 0.f; Vertices.push_back(v);
        // 2
        v.Position.x = half; v.Position.y = half; v.Position.z = -half; v.UV.x = 1.f; v.UV.y = 1.f; Vertices.push_back(v);
        // 3
        v.Position.x = -half; v.Position.y = half; v.Position.z = -half; v.UV.x = 0.f; v.UV.y = 1.f; Vertices.push_back(v);
        // 4
        v.Position.x = -half; v.Position.y = -half; v.Position.z = half; v.UV.x = 0.f; v.UV.y = 0.f; Vertices.push_back(v);
        // 5
        v.Position.x = half; v.Position.y = -half; v.Position.z = half; v.UV.x = 1.f; v.UV.y = 0.f; Vertices.push_back(v);
        // 6
        v.Position.x = half; v.Position.y = half; v.Position.z = half; v.UV.x = 1.f; v.UV.y = 1.f; Vertices.push_back(v);
        // 7
        v.Position.x = -half; v.Position.y = half; v.Position.z = half; v.UV.x = 0.f; v.UV.y = 1.f; Vertices.push_back(v);

        uint32_t idx[] = {
            // front (-Z)
            0,1,2, 2,3,0,
            // back (+Z)
            4,5,6, 6,7,4,
            // left (-X)
            4,0,3, 3,7,4,
            // right (+X)
            1,5,6, 6,2,1,
            // bottom (-Y)
            4,5,1, 1,0,4,
            // top (+Y)
            3,2,6, 6,7,3
        };

        Indices.insert(Indices.end(), std::begin(idx), std::end(idx));
    }
};
