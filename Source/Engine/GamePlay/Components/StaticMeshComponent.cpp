// CEStaticMeshComponent.cpp
#include "Engine/GamePlay/Components/StaticMeshComponent.h"
#include "Engine/Core/Utilities/ObjLoader.h"

#include "Engine/Core/CoreTypes.h"
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>


  CStaticMeshComponent::CStaticMeshComponent(CObject* Owner, FString NewName)
      : CMeshComponent(Owner, NewName)
  {
    // Не создаем куб по умолчанию - меш будет пустым
    m_Mesh.vertices.clear();
    m_Mesh.indices.clear();
  }

  void CStaticMeshComponent::SetMesh(const std::string& MeshPath)
  {
    m_MeshPath = MeshPath;
    if (!MeshPath.empty())
    {
      FStaticMesh loadedMesh = ObjLoader::LoadOBJ(MeshPath);
      if (!loadedMesh.vertices.empty() && !loadedMesh.indices.empty())
      {
        m_Mesh = loadedMesh;
        return;
      }

      if (MeshPath.find(".cube") != std::string::npos)
      {
        CreateCubeMesh();
      }
      else
      {
        m_Mesh.vertices.clear();
        m_Mesh.indices.clear();
      }
    }
    else
    {
      m_Mesh.vertices.clear();
      m_Mesh.indices.clear();
    }
  }


