// CEStaticMeshComponent.h
#pragma once
#include <unordered_map>

#include "Engine/GamePlay/Components/MeshComponent.h"

namespace CE
{
  class CEStaticMeshComponent : public MeshComponent
  {
   public:
    CEStaticMeshComponent(CEObject* Owner = nullptr, FString NewName = "CEStaticMeshComponent");
    virtual ~CEStaticMeshComponent() = default;

    // Переопределяем SetMesh для загрузки из OBJ файла
    virtual void SetMesh(const std::string& MeshPath) override;

   private:
    bool LoadOBJFile(const std::string& filename);
    void ProcessOBJFace(const std::string& faceLine,
                        const std::vector<glm::vec3>& positions,
                        const std::vector<glm::vec2>& texCoords,
                        const std::vector<glm::vec3>& normals,
                        std::unordered_map<std::string, uint32_t>& vertexMap,
                        std::vector<Vertex>& outVertices,
                        std::vector<uint32_t>& outIndices);
  };
}  // namespace CE