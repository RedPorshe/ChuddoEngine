// CEStaticMeshComponent.cpp
#include "Engine/GamePlay/Components/StaticMeshComponent.h"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace CE
{
  CEStaticMeshComponent::CEStaticMeshComponent(CObject* Owner, FString NewName)
      : MeshComponent(Owner, NewName)
  {
    // Не создаем куб по умолчанию - меш будет пустым
    m_Mesh.vertices.clear();
    m_Mesh.indices.clear();
  }

  void CEStaticMeshComponent::SetMesh(const std::string& MeshPath)
  {
    m_MeshPath = MeshPath;

    if (!MeshPath.empty())
    {
      // Пробуем загрузить OBJ файл
      if (LoadOBJFile(MeshPath))
      {
        // Успешно загрузили из файла
        return;
      }

      // Если загрузка не удалась и это .cube файл, используем куб
      if (MeshPath.find(".cube") != std::string::npos)
      {
        CreateCubeMesh();
      }
      else
      {
        // Для других путей очищаем меш
        m_Mesh.vertices.clear();
        m_Mesh.indices.clear();
      }
    }
    else
    {
      // Если путь пустой - очищаем меш
      m_Mesh.vertices.clear();
      m_Mesh.indices.clear();
    }
  }

  bool CEStaticMeshComponent::LoadOBJFile(const std::string& filename)
  {
    std::ifstream file(filename);
    if (!file.is_open())
    {
      return false;
    }

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
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
        glm::vec3 pos;
        iss >> pos.x >> pos.y >> pos.z;
        positions.push_back(pos);
      }
      else if (prefix == "vt")  // Texture coordinate
      {
        glm::vec2 tex;
        iss >> tex.x >> tex.y;
        texCoords.push_back(tex);
      }
      else if (prefix == "vn")  // Normal
      {
        glm::vec3 normal;
        iss >> normal.x >> normal.y >> normal.z;
        normals.push_back(normal);
      }
      else if (prefix == "f")  // Face
      {
        ProcessOBJFace(line, positions, texCoords, normals, vertexMap, vertices, indices);
      }
    }

    file.close();

    // Обновляем данные меша только если есть вершины
    if (!vertices.empty() && !indices.empty())
    {
      m_Mesh.vertices = vertices;
      m_Mesh.indices = indices;
      m_Mesh.color = glm::vec3(1.0f);  // Белый цвет по умолчанию
      return true;
    }

    return false;
  }

  void CEStaticMeshComponent::ProcessOBJFace(const std::string& faceLine,
                                             const std::vector<glm::vec3>& positions,
                                             const std::vector<glm::vec2>& texCoords,
                                             const std::vector<glm::vec3>& normals,
                                             std::unordered_map<std::string, uint32_t>& vertexMap,
                                             std::vector<Vertex>& outVertices,
                                             std::vector<uint32_t>& outIndices)
  {
    std::istringstream iss(faceLine);
    std::string prefix;
    iss >> prefix;  // Пропускаем "f"

    std::vector<uint32_t> faceIndices;

    std::string vertexDesc;
    while (iss >> vertexDesc)
    {
      // Проверяем, есть ли уже такая вершина
      if (vertexMap.find(vertexDesc) != vertexMap.end())
      {
        faceIndices.push_back(vertexMap[vertexDesc]);
        continue;
      }

      // Парсим описание вершины: position/texture/normal
      std::istringstream vss(vertexDesc);
      std::string posStr, texStr, normStr;

      std::getline(vss, posStr, '/');
      std::getline(vss, texStr, '/');
      std::getline(vss, normStr, '/');

      Vertex vertex;

      // Позиция (обязательно)
      if (!posStr.empty())
      {
        int posIndex = std::stoi(posStr) - 1;  // OBJ индексы начинаются с 1
        if (posIndex >= 0 && posIndex < static_cast<int>(positions.size()))
        {
          vertex.position = positions[posIndex];
        }
      }

      // Текстурные координаты (опционально)
      if (!texStr.empty())
      {
        int texIndex = std::stoi(texStr) - 1;
        if (texIndex >= 0 && texIndex < static_cast<int>(texCoords.size()))
        {
          vertex.texCoord = texCoords[texIndex];
        }
      }

      // Нормали (опционально)
      if (!normStr.empty())
      {
        int normIndex = std::stoi(normStr) - 1;
        if (normIndex >= 0 && normIndex < static_cast<int>(normals.size()))
        {
          vertex.normal = normals[normIndex];
        }
      }

      // Цвет по умолчанию
      vertex.color = glm::vec3(1.0f);

      // Добавляем вершину
      outVertices.push_back(vertex);
      uint32_t newIndex = static_cast<uint32_t>(outVertices.size() - 1);
      vertexMap[vertexDesc] = newIndex;
      faceIndices.push_back(newIndex);
    }

    // Преобразуем полигон в треугольники (триангуляция для n-угольников)
    if (faceIndices.size() >= 3)
    {
      // Простая триангуляция веером
      for (size_t i = 1; i < faceIndices.size() - 1; ++i)
      {
        outIndices.push_back(faceIndices[0]);
        outIndices.push_back(faceIndices[i]);
        outIndices.push_back(faceIndices[i + 1]);
      }
    }
  }
}  // namespace CE