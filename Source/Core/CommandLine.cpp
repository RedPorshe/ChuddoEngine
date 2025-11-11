#include "Core/CommandLine.h"

#include "CoreMinimal.h"

namespace CE
{
  CommandLine CommandLine::s_Instance;

  CommandLine& CommandLine::Get()
  {
    return s_Instance;
  }

  void CommandLine::Parse(int argc, char* argv[])
  {
    auto& instance = Get();
    instance.m_Params.clear();
    instance.m_Arguments.clear();

    for (int i = 1; i < argc; ++i)  // Пропускаем argv[0] (имя программы)
    {
      std::string arg = argv[i];

      if (arg.substr(0, 2) == "--")
      {
        // Длинные параметры: --key=value или --flag
        size_t equalsPos = arg.find('=');
        if (equalsPos != std::string::npos)
        {
          std::string key = arg.substr(2, equalsPos - 2);
          std::string value = arg.substr(equalsPos + 1);
          instance.m_Params[key] = value;
          CE_CORE_DEBUG("Command line param: ", key, " = ", value);
        }
        else
        {
          std::string key = arg.substr(2);
          instance.m_Params[key] = "true";  // Флаг без значения
          CE_CORE_DEBUG("Command line flag: ", key);
        }
      }
      else if (arg.substr(0, 1) == "-")
      {
        // Короткие параметры: -k value или -f
        std::string key = arg.substr(1);
        if (i + 1 < argc && argv[i + 1][0] != '-')
        {
          instance.m_Params[key] = argv[i + 1];
          CE_CORE_DEBUG("Command line param: -", key, " = ", argv[i + 1]);
          ++i;  // Пропускаем значение
        }
        else
        {
          instance.m_Params[key] = "true";  // Флаг без значения
          CE_CORE_DEBUG("Command line flag: -", key);
        }
      }
      else
      {
        // Позиционный аргумент
        instance.m_Arguments.push_back(arg);
        CE_CORE_DEBUG("Command line argument: ", arg);
      }
    }

    CE_CORE_LOG("Command line parsed: ", instance.m_Params.size(),
                " parameters, ", instance.m_Arguments.size(), " arguments");
  }

  bool CommandLine::HasFlag(const std::string& flag) const
  {
    return m_Params.find(flag) != m_Params.end();
  }

  std::string CommandLine::GetString(const std::string& key, const std::string& defaultValue) const
  {
    auto it = m_Params.find(key);
    return it != m_Params.end() ? it->second : defaultValue;
  }

  int CommandLine::GetInt(const std::string& key, int defaultValue) const
  {
    auto it = m_Params.find(key);
    if (it != m_Params.end())
    {
      try
      {
        return std::stoi(it->second);
      }
      catch (...)
      {
        CE_CORE_WARN("Invalid int value in command line for key: ", key);
      }
    }
    return defaultValue;
  }

  float CommandLine::GetFloat(const std::string& key, float defaultValue) const
  {
    auto it = m_Params.find(key);
    if (it != m_Params.end())
    {
      try
      {
        return std::stof(it->second);
      }
      catch (...)
      {
        CE_CORE_WARN("Invalid float value in command line for key: ", key);
      }
    }
    return defaultValue;
  }

  bool CommandLine::GetBool(const std::string& key, bool defaultValue) const
  {
    auto it = m_Params.find(key);
    if (it != m_Params.end())
    {
      std::string value = it->second;
      std::transform(value.begin(), value.end(), value.begin(), ::tolower);
      return (value == "true" || value == "1" || value == "yes" || value == "on");
    }
    return defaultValue;
  }
}  // namespace CE