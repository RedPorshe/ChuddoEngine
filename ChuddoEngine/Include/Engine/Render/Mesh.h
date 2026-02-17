#pragma once

#include <vector>
#include <cstdint>
#include <memory>

// Simple vertex structure for test rendering
struct Vertex
{
    float Position[3];
    float Normal[3];
    float UV[2];
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
        v.Position[0] = -half; v.Position[1] = -half; v.Position[2] = -half; v.Normal[0] = v.Normal[1] = v.Normal[2] = 0.f; v.UV[0] = 0.f; v.UV[1] = 0.f; Vertices.push_back(v);
        // 1
        v.Position[0] = half; v.Position[1] = -half; v.Position[2] = -half; v.UV[0] = 1.f; v.UV[1] = 0.f; Vertices.push_back(v);
        // 2
        v.Position[0] = half; v.Position[1] = half; v.Position[2] = -half; v.UV[0] = 1.f; v.UV[1] = 1.f; Vertices.push_back(v);
        // 3
        v.Position[0] = -half; v.Position[1] = half; v.Position[2] = -half; v.UV[0] = 0.f; v.UV[1] = 1.f; Vertices.push_back(v);
        // 4
        v.Position[0] = -half; v.Position[1] = -half; v.Position[2] = half; v.UV[0] = 0.f; v.UV[1] = 0.f; Vertices.push_back(v);
        // 5
        v.Position[0] = half; v.Position[1] = -half; v.Position[2] = half; v.UV[0] = 1.f; v.UV[1] = 0.f; Vertices.push_back(v);
        // 6
        v.Position[0] = half; v.Position[1] = half; v.Position[2] = half; v.UV[0] = 1.f; v.UV[1] = 1.f; Vertices.push_back(v);
        // 7
        v.Position[0] = -half; v.Position[1] = half; v.Position[2] = half; v.UV[0] = 0.f; v.UV[1] = 1.f; Vertices.push_back(v);

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
