// CEStaticMeshComponent.h
#pragma once
#include <unordered_map>

#include "Engine/GamePlay/Components/MeshComponent.h"
#include "Engine/Core/CoreTypes.h"


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
                        const std::vector<FVector>& positions,
                        const std::vector<FVector2D>& texCoords,
                        const std::vector<FVector>& normals,
                        std::unordered_map<std::string, uint32_t>& vertexMap,
                        std::vector<Vertex>& outVertices,
                        std::vector<uint32_t>& outIndices);
  };