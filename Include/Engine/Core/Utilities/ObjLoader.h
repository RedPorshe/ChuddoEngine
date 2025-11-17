#pragma once

#include <string>
#include <vector>

#include "Engine/Core/Rendering/Data/Vertex.h"

namespace CE
{
  /**
   * @class ObjLoader
   * @brief Загрузчик OBJ файлов для импорта 3D моделей
   */
  class ObjLoader
  {
   public:
    /**
     * @brief Загружает OBJ файл и возвращает StaticMesh
     * @param filePath Путь к OBJ файлу
     * @return StaticMesh с загруженными вершинами и индексами, или пустой меш при ошибке
     */
    static StaticMesh LoadOBJ(const std::string& filePath);

   private:
    ObjLoader() = default;
  };
}  // namespace CE
