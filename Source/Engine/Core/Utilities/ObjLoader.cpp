#include "Engine/Core/Utilities/ObjLoader.h"

#include <fstream>
#include <glm/glm.hpp>
#include <map>
#include <sstream>

#include "Engine/Utils/Logger.h"
#include "Engine/Core/CoreTypes.h"

FStaticMesh ObjLoader::LoadOBJ(const std::string& filePath)
{
  FStaticMesh mesh;
  std::vector<FVector> positions;
  std::vector<FVector> normals;
  std::vector<FVector2D> texCoords;
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  std::map<Vertex, uint32_t> vertexMap;

  std::string fullPath = filePath;
  std::ifstream file(fullPath);
  if (!file.is_open() && fullPath.find("Assets/") == 0) {
    fullPath = "../" + fullPath;
    file.open(fullPath);
  }
  if (!file.is_open())
  {
    CORE_ERROR("Failed to open OBJ file: ", filePath.c_str());
    return mesh;
  }

  std::string line;
  uint32_t lineNum = 0;

  while (std::getline(file, line))
  {
    lineNum++;
    if (line.empty() || line[0] == '#')
      continue;

    std::istringstream iss(line);
    std::string token;
    iss >> token;

    if (token == "v")
    {
      float x, y, z;
      if (iss >> x >> y >> z)
      {
        positions.push_back(FVector(x, y, z));
      }
      else
      {
       CORE_WARN("Invalid vertex format at line ", lineNum);
      }
    }
    else if (token == "vn")
    {
      float x, y, z;
      if (iss >> x >> y >> z)
      {
        normals.push_back(FVector(x, y, z));
      }
    }
    else if (token == "vt")
    {
      float u, v;
      if (iss >> u >> v)
      {
        texCoords.push_back(FVector2D(u, v));
      }
    }
    else if (token == "f")
    {
      

      std::vector<uint32_t> faceIndices;
      std::string vertexStr;

      while (iss >> vertexStr)
      {
        std::istringstream vertexStream(vertexStr);
        int posIdx = 0, texIdx = 0, normIdx = 0;
        char slash;

        // v
        if (vertexStream >> posIdx)
        {
          // Adjust for negative indices
          if (posIdx > 0) posIdx--;
          else if (posIdx < 0) posIdx += positions.size();

          if (vertexStream.peek() == '/')
          {
            vertexStream >> slash;


            if (vertexStream.peek() != '/')
            {
              vertexStream >> texIdx;
              if (texIdx > 0) texIdx--;
              else if (texIdx < 0) texIdx += texCoords.size();
            }

            if (vertexStream.peek() == '/')
            {
              vertexStream >> slash >> normIdx;
              if (normIdx > 0) normIdx--;
              else if (normIdx < 0) normIdx += normals.size();
            }
          }

         
          Vertex vertex;
          if (posIdx >= 0 && static_cast<size_t>(posIdx) < positions.size())
          {
            vertex.position = positions[posIdx];
          }
          if (normIdx >= 0 && static_cast<size_t>(normIdx) < normals.size())
          {
            vertex.normal = normals[normIdx];
          }
          else
          {
            vertex.normal = FVector(0.0f, 1.0f, 0.0f);  // Дефолтная нормаль
          }
          if (texIdx >= 0 && static_cast<size_t>(texIdx) < texCoords.size())
          {
            vertex.texCoord = texCoords[texIdx];
          }
          vertex.color = FVector(1.0f);

          // Deduplicate vertices
          auto it = vertexMap.find(vertex);
          if (it == vertexMap.end())
          {
            uint32_t index = static_cast<uint32_t>(vertices.size());
            vertices.push_back(vertex);
            vertexMap[vertex] = index;
            faceIndices.push_back(index);
          }
          else
          {
            faceIndices.push_back(it->second);
          }
        }
      }

      

      if (faceIndices.size() == 3)
      {
        // Already triangulated, add directly
        indices.push_back(faceIndices[0]);
        indices.push_back(faceIndices[1]);
        indices.push_back(faceIndices[2]);
      }
      else if (faceIndices.size() > 3)
      {
        // Triangulate polygon using fan triangulation
        for (size_t i = 1; i + 1 < faceIndices.size(); ++i)
        {
          indices.push_back(faceIndices[0]);
          indices.push_back(faceIndices[i + 1]);
          indices.push_back(faceIndices[i]);
        }
      }
    }
  }

  file.close();

  if (vertices.empty() || indices.empty())
  {
    CORE_WARN("OBJ file loaded but contains no geometry: ", filePath.c_str());
    return mesh;
  }

  mesh.vertices = vertices;
  mesh.indices = indices;
  mesh.color = FVector(1.0f);

  CORE_LOG("Successfully loaded OBJ: ", filePath.c_str(), " (vertices: ", vertices.size(), ", indices: ", indices.size(),
         ")");

  return mesh;
}
