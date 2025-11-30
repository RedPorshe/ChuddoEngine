// CEStaticMeshComponent.cpp
#include "Engine/GamePlay/Components/StaticMeshComponent.h"

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
      
      if (LoadOBJFile(MeshPath))
      {
        
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

  bool CStaticMeshComponent::LoadOBJFile(const std::string& filename)
  {
    std::ifstream file(filename);
    if (!file.is_open())
    {
      CORE_ERROR("Failed to open OBJ file: ", filename);
      return false;
    }
    CORE_DEBUG("Loading OBJ file: ", filename);

    std::vector<CEMath::Vector3f> positions;
    std::vector<CEMath::Vector2f> texCoords;
    std::vector<CEMath::Vector3f> normals;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    std::unordered_map<std::string, uint32_t> vertexMap;

    std::string line;
    while (std::getline(file, line))
    {
      std::istringstream iss(line);
      std::string prefix;
      iss >> prefix;

      if (prefix == "v")  // Vertex position
      {
        CEMath::Vector3f pos;
        iss >> pos.x >> pos.y >> pos.z;
        positions.push_back(pos);
      }
      else if (prefix == "vt")  // Texture coordinate
      {
        CEMath::Vector2f tex;
        iss >> tex.x >> tex.y;
        texCoords.push_back(tex);
      }
      else if (prefix == "vn")  // Normal
      {
        CEMath::Vector3f normal;
        iss >> normal.x >> normal.y >> normal.z;
        normals.push_back(normal);
      }
      else if (prefix == "f")  // Face
      {
        ProcessOBJFace(line, positions, texCoords, normals, vertexMap, vertices, indices);
      }
    }

    file.close();

    
    if (!vertices.empty() && !indices.empty())
    {
      m_Mesh.vertices = vertices;
      m_Mesh.indices = indices;
      m_Mesh.color = CEMath::Vector3f(1.0f);  // Белый цвет по умолчанию
      return true;
    }

    return false;
  }

  void CStaticMeshComponent::ProcessOBJFace(const std::string& faceLine,
                                             const std::vector<CEMath::Vector3f>& positions,
                                             const std::vector<CEMath::Vector2f>& texCoords,
                                             const std::vector<CEMath::Vector3f>& normals,
                                             std::unordered_map<std::string, uint32_t>& vertexMap,
                                             std::vector<Vertex>& outVertices,
                                             std::vector<uint32_t>& outIndices)
  {
    std::istringstream iss(faceLine);
    std::string prefix;
    iss >> prefix;  

    std::vector<uint32_t> faceIndices;

    std::string vertexDesc;
    while (iss >> vertexDesc)
    {
      
      if (vertexMap.find(vertexDesc) != vertexMap.end())
      {
        faceIndices.push_back(vertexMap[vertexDesc]);
        continue;
      }

      
      std::istringstream vss(vertexDesc);
      std::string posStr, texStr, normStr;

      std::getline(vss, posStr, '/');
      std::getline(vss, texStr, '/');
      std::getline(vss, normStr, '/');

      Vertex vertex;

      
      if (!posStr.empty())
      {
        int posIndex = std::stoi(posStr) - 1;  
        if (posIndex >= 0 && posIndex < static_cast<int>(positions.size()))
        {
          vertex.position = positions[posIndex];
        }
      }

      
      if (!texStr.empty())
      {
        int texIndex = std::stoi(texStr) - 1;
        if (texIndex >= 0 && texIndex < static_cast<int>(texCoords.size()))
        {
          vertex.texCoord = texCoords[texIndex];
        }
      }

      
      if (!normStr.empty())
      {
        int normIndex = std::stoi(normStr) - 1;
        if (normIndex >= 0 && normIndex < static_cast<int>(normals.size()))
        {
          vertex.normal = normals[normIndex];
        }
      }

      
      vertex.color = CEMath::Vector3f(1.0f);

      
      outVertices.push_back(vertex);
      uint32_t newIndex = static_cast<uint32_t>(outVertices.size() - 1);
      vertexMap[vertexDesc] = newIndex;
      faceIndices.push_back(newIndex);
    }

   
    if (faceIndices.size() >= 3)
    {
      
      for (size_t i = 1; i < faceIndices.size() - 1; ++i)
      {
        outIndices.push_back(faceIndices[0]);
        outIndices.push_back(faceIndices[i]);
        outIndices.push_back(faceIndices[i + 1]);
      }
    }
  }