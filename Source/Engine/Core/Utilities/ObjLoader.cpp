#include "Engine/Core/Utilities/ObjLoader.h"

#include <fstream>
#include <glm/glm.hpp>
#include <sstream>

#include "Engine/Utils/Logger.h"

namespace CE
{
  StaticMesh ObjLoader::LoadOBJ(const std::string& filePath)
  {
    StaticMesh mesh;
    std::vector<Math::Vector3f> positions;
    std::vector<Math::Vector3f> normals;
    std::vector<Math::Vector2f> texCoords;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    std::ifstream file(filePath);
    if (!file.is_open())
    {
      CE_ERROR("Failed to open OBJ file: ", filePath.c_str());
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
          positions.push_back(Math::Vector3f(x, y, z));
        }
        else
        {
          CE_WARN("Invalid vertex format at line ", lineNum);
        }
      }
      else if (token == "vn")
      {
        float x, y, z;
        if (iss >> x >> y >> z)
        {
          normals.push_back(Math::Vector3f(x, y, z));
        }
      }
      else if (token == "vt")
      {
        float u, v;
        if (iss >> u >> v)
        {
          texCoords.push_back(Math::Vector2f(u, v));
        }
      }
      else if (token == "f")
      {
        // Парсим грань: поддерживаем форматы:
        // v (только позиции)
        // v/vt (позиции + текстурные координаты)
        // v/vt/vn (позиции + текстуры + нормали)
        // v//vn (позиции + нормали без текстур)

        std::vector<uint32_t> faceIndices;
        std::string vertexStr;

        while (iss >> vertexStr)
        {
          std::istringstream vertexStream(vertexStr);
          uint32_t posIdx = 0, texIdx = 0, normIdx = 0;
          char slash;

          // v
          if (vertexStream >> posIdx)
          {
            posIdx--;  // OBJ индексы начинаются с 1

            // v/vt или v/vt/vn или v//vn
            if (vertexStream.peek() == '/')
            {
              vertexStream >> slash;

              // Проверяем, есть ли текстурная координата (не сразу //)
              if (vertexStream.peek() != '/')
              {
                vertexStream >> texIdx;
                texIdx--;
              }

              if (vertexStream.peek() == '/')
              {
                vertexStream >> slash >> normIdx;
                normIdx--;
              }
            }

            // Создаём вершину
            Vertex vertex;
            if (posIdx < positions.size())
            {
              vertex.position = positions[posIdx];
            }
            if (normIdx < normals.size())
            {
              vertex.normal = normals[normIdx];
            }
            else
            {
              vertex.normal = Math::Vector3f(0.0f, 1.0f, 0.0f);  // Дефолтная нормаль
            }
            if (texIdx < texCoords.size())
            {
              vertex.texCoord = texCoords[texIdx];
            }
            vertex.color = Math::Vector3f(1.0f);

            vertices.push_back(vertex);
            faceIndices.push_back(static_cast<uint32_t>(vertices.size()) - 1);
          }
        }

        // Триангулируем грань (если это не треугольник)
        if (faceIndices.size() >= 3)
        {
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
      CE_WARN("OBJ file loaded but contains no geometry: ", filePath.c_str());
      return mesh;
    }

    mesh.vertices = vertices;
    mesh.indices = indices;
    mesh.color = Math::Vector3f(1.0f);

    CE_LOG("Successfully loaded OBJ: ", filePath.c_str(), " (vertices: ", vertices.size(), ", indices: ", indices.size(),
           ")");

    return mesh;
  }
}  // namespace CE
