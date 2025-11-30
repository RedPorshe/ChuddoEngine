// CEStaticMeshComponent.h
#pragma once
#include <unordered_map>

#include "Engine/GamePlay/Components/MeshComponent.h"


  class CStaticMeshComponent : public CMeshComponent
  {
   public:
    CStaticMeshComponent(CObject* Owner = nullptr, FString NewName = "CEStaticMeshComponent");
    virtual ~CStaticMeshComponent() = default;

    // Переопределяем SetMesh для загрузки из OBJ файла
    virtual void SetMesh(const std::string& MeshPath) override;

   private:
    bool LoadOBJFile(const std::string& filename);
    void ProcessOBJFace(const std::string& faceLine,
                        const std::vector<CEMath::Vector3f>& positions,
                        const std::vector<CEMath::Vector2f>& texCoords,
                        const std::vector<CEMath::Vector3f>& normals,
                        std::unordered_map<std::string, uint32_t>& vertexMap,
                        std::vector<Vertex>& outVertices,
                        std::vector<uint32_t>& outIndices);
  };