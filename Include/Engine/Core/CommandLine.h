#pragma once
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "CoreMinimal.h"

namespace CE
{
  class CommandLine
  {
   public:
    static void Parse(int argc, char* argv[]);
    static CommandLine& Get();

    // Проверка флагов
    bool HasFlag(const std::string& flag) const;

    // Получение значений
    std::string GetString(const std::string& key, const std::string& defaultValue = "") const;
    int GetInt(const std::string& key, int defaultValue = 0) const;
    float GetFloat(const std::string& key, float defaultValue = 0.0f) const;
    bool GetBool(const std::string& key, bool defaultValue = false) const;

    // Получение позиционных аргументов
    const std::vector<std::string>& GetArguments() const
    {
      return m_Arguments;
    }
    size_t GetArgumentCount() const
    {
      return m_Arguments.size();
    }

   private:
    CommandLine() = default;

    std::unordered_map<std::string, std::string> m_Params;
    std::vector<std::string> m_Arguments;

    static CommandLine s_Instance;
  };
}  // namespace CE