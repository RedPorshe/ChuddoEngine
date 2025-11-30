#pragma once

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

#ifdef _WIN32
#include <windows.h>
#endif

namespace CE
{
  enum class ELogLevel
  {
    Fatal = 0,  // Самый важный
    Error = 1,
    Warning = 2,
    Display = 3,
    Log = 4,
    Verbose = 5,
    VeryVerbose = 6  // Самый подробный
  };

  class CLogCategory
  {
   public:
    constexpr CLogCategory(const char* name, ELogLevel defaultLevel = ELogLevel::Log)
        : name(name), defaultLevel(defaultLevel)
    {
    }

    const char* GetName() const
    {
      return name;
    }
    ELogLevel GetDefaultLevel() const
    {
      return defaultLevel;
    }

   private:
    const char* name;
    ELogLevel defaultLevel;
  };

  extern CLogCategory LogTemp;
  extern CLogCategory LogCore;
  extern CLogCategory LogRender;
  extern CLogCategory LogInput;
  extern CLogCategory LogAudio;
  extern CLogCategory LogGameplay;
  extern CLogCategory LogEditor;
  extern CLogCategory LogSystem;

  class CLogger
{
 public:
  static void Initialize(bool useUniqueLogFile = true, bool overwriteExisting = false);
  static void Shutdown();

  static void SetGlobalLogLevel(ELogLevel level);
  static void SetCategoryLogLevel(const CLogCategory& category, ELogLevel level);
  static void SetConsoleOutput(bool enable);
  static void SetFileOutput(bool enable);

  template <typename... Args>
  static void Log(const CLogCategory& category, ELogLevel level, const Args&... args);

  template <typename... Args>
  static void LogFatal(const CLogCategory& category, const Args&... args)
  {
    Log(category, ELogLevel::Fatal, args...);
  }

  template <typename... Args>
  static void LogError(const CLogCategory& category, const Args&... args)
  {
    Log(category, ELogLevel::Error, args...);
  }

  template <typename... Args>
  static void LogWarning(const CLogCategory& category, const Args&... args)
  {
    Log(category, ELogLevel::Warning, args...);
  }

  template <typename... Args>
  static void LogDisplay(const CLogCategory& category, const Args&... args)
  {
    Log(category, ELogLevel::Display, args...);
  }

  template <typename... Args>
  static void LogMsg(const CLogCategory& category, const Args&... args)
  {
    Log(category, ELogLevel::Log, args...);
  }

  template <typename... Args>
  static void LogVerbose(const CLogCategory& category, const Args&... args)
  {
    Log(category, ELogLevel::Verbose, args...);
  }

  template <typename... Args>
  static void LogVeryVerbose(const CLogCategory& category, const Args&... args)
  {
    Log(category, ELogLevel::VeryVerbose, args...);
  }

 private:
  static void WriteToConsole(ELogLevel level, const CLogCategory& category, const std::string& message);
  static void WriteToFile(const std::string& message);
  static std::string GetCurrentTimeStamp();
  static std::string GetLevelString(ELogLevel level);
  static std::string GetLevelDisplayName(ELogLevel level);
  static std::string GetLogFilePath();

  static ELogLevel s_globalLevel;
  static bool s_consoleOutput;
  static bool s_fileOutput;
  static bool s_initialized;
  static bool s_useUniqueLogFile;
  static bool s_overwriteExisting;
  static std::ofstream s_logFile;
  static std::unordered_map<const CLogCategory*, ELogLevel> s_categoryLevels;
};

template <typename... Args>
void CLogger::Log(const CLogCategory& category, ELogLevel level, const Args&... args)
{
  if (!s_initialized)
    return;

  ELogLevel categoryLevel = s_globalLevel;
  auto it = s_categoryLevels.find(&category);
  if (it != s_categoryLevels.end())
  {
    categoryLevel = it->second;
  }

  if (static_cast<int>(level) > static_cast<int>(categoryLevel))
    return;

  std::ostringstream stream;
  ((stream << args), ...);

  std::string message = GetCurrentTimeStamp() + " " + GetLevelDisplayName(level) +
                        ": [" + category.GetName() + "] " + stream.str();

  if (s_consoleOutput)
  {
    WriteToConsole(level, category, message);
  }

  if (s_fileOutput)
  {
    WriteToFile(message);
  }

  if (level == ELogLevel::Fatal)
  {
    Shutdown();
    std::exit(1);
  }
}
}

using CE::CLogCategory;
using CE::LogTemp;
using CE::LogCore;
using CE::LogRender;
using CE::LogInput;
using CE::LogAudio;
using CE::LogGameplay;
using CE::LogEditor;
using CE::LogSystem;

#define LOG_CATEGORY(Category, Level, ...) CE::CLogger::Log##Level(Category, __VA_ARGS__)


#define CORE_FATAL(...) LOG_CATEGORY(LogCore, Fatal, __VA_ARGS__)
#define CORE_ERROR(...) LOG_CATEGORY(LogCore, Error, __VA_ARGS__)
#define CORE_WARN(...) LOG_CATEGORY(LogCore, Warning, __VA_ARGS__)
#define CORE_DISPLAY(...) LOG_CATEGORY(LogCore, Display, __VA_ARGS__)
#define CORE_LOG(...) LOG_CATEGORY(LogCore, Msg, __VA_ARGS__)
#define CORE_DEBUG(...) LOG_CATEGORY(LogCore, Verbose, __VA_ARGS__)
#define CORE_TRACE(...) LOG_CATEGORY(LogCore, VeryVerbose, __VA_ARGS__)


#define RENDER_FATAL(...) LOG_CATEGORY(LogRender, Fatal, __VA_ARGS__)
#define RENDER_ERROR(...) LOG_CATEGORY(LogRender, Error, __VA_ARGS__)
#define RENDER_WARN(...) LOG_CATEGORY(LogRender, Warning, __VA_ARGS__)
#define RENDER_DISPLAY(...) LOG_CATEGORY(LogRender, Display, __VA_ARGS__)
#define RENDER_LOG(...) LOG_CATEGORY(LogRender, Msg, __VA_ARGS__)
#define RENDER_DEBUG(...) LOG_CATEGORY(LogRender, Verbose, __VA_ARGS__)
#define RENDER_TRACE(...) LOG_CATEGORY(LogRender, VeryVerbose, __VA_ARGS__)


#define INPUT_FATAL(...) LOG_CATEGORY(LogInput, Fatal, __VA_ARGS__)
#define INPUT_ERROR(...) LOG_CATEGORY(LogInput, Error, __VA_ARGS__)
#define INPUT_WARN(...) LOG_CATEGORY(LogInput, Warning, __VA_ARGS__)
#define INPUT_DISPLAY(...) LOG_CATEGORY(LogInput, Display, __VA_ARGS__)
#define INPUT_LOG(...) LOG_CATEGORY(LogInput, Msg, __VA_ARGS__)
#define INPUT_DEBUG(...) LOG_CATEGORY(LogInput, Verbose, __VA_ARGS__)
#define INPUT_TRACE(...) LOG_CATEGORY(LogInput, VeryVerbose, __VA_ARGS__)


#define AUDIO_FATAL(...) LOG_CATEGORY(LogAudio, Fatal, __VA_ARGS__)
#define AUDIO_ERROR(...) LOG_CATEGORY(LogAudio, Error, __VA_ARGS__)
#define AUDIO_WARN(...) LOG_CATEGORY(LogAudio, Warning, __VA_ARGS__)
#define AUDIO_DISPLAY(...) LOG_CATEGORY(LogAudio, Display, __VA_ARGS__)
#define AUDIO_LOG(...) LOG_CATEGORY(LogAudio, Msg, __VA_ARGS__)
#define AUDIO_DEBUG(...) LOG_CATEGORY(LogAudio, Verbose, __VA_ARGS__)
#define AUDIO_TRACE(...) LOG_CATEGORY(LogAudio, VeryVerbose, __VA_ARGS__)


#define GAMEPLAY_FATAL(...) LOG_CATEGORY(LogGameplay, Fatal, __VA_ARGS__)
#define GAMEPLAY_ERROR(...) LOG_CATEGORY(LogGameplay, Error, __VA_ARGS__)
#define GAMEPLAY_WARN(...) LOG_CATEGORY(LogGameplay, Warning, __VA_ARGS__)
#define GAMEPLAY_DISPLAY(...) LOG_CATEGORY(LogGameplay, Display, __VA_ARGS__)
#define GAMEPLAY_LOG(...) LOG_CATEGORY(LogGameplay, Msg, __VA_ARGS__)
#define GAMEPLAY_DEBUG(...) LOG_CATEGORY(LogGameplay, Verbose, __VA_ARGS__)
#define GAMEPLAY_TRACE(...) LOG_CATEGORY(LogGameplay, VeryVerbose, __VA_ARGS__)


#define EDITOR_FATAL(...) LOG_CATEGORY(LogEditor, Fatal, __VA_ARGS__)
#define EDITOR_ERROR(...) LOG_CATEGORY(LogEditor, Error, __VA_ARGS__)
#define EDITOR_WARN(...) LOG_CATEGORY(LogEditor, Warning, __VA_ARGS__)
#define EDITOR_DISPLAY(...) LOG_CATEGORY(LogEditor, Display, __VA_ARGS__)
#define EDITOR_LOG(...) LOG_CATEGORY(LogEditor, Msg, __VA_ARGS__)
#define EDITOR_DEBUG(...) LOG_CATEGORY(LogEditor, Verbose, __VA_ARGS__)
#define EDITOR_TRACE(...) LOG_CATEGORY(LogEditor, VeryVerbose, __VA_ARGS__)


#define SYSTEM_FATAL(...) LOG_CATEGORY(LogSystem, Fatal, __VA_ARGS__)
#define SYSTEM_ERROR(...) LOG_CATEGORY(LogSystem, Error, __VA_ARGS__)
#define SYSTEM_WARN(...) LOG_CATEGORY(LogSystem, Warning, __VA_ARGS__)
#define SYSTEM_DISPLAY(...) LOG_CATEGORY(LogSystem, Display, __VA_ARGS__)
#define SYSTEM_LOG(...) LOG_CATEGORY(LogSystem, Msg, __VA_ARGS__)
#define SYSTEM_DEBUG(...) LOG_CATEGORY(LogSystem, Verbose, __VA_ARGS__)
#define SYSTEM_TRACE(...) LOG_CATEGORY(LogSystem, VeryVerbose, __VA_ARGS__)


#define CFATAL(...) LOG_CATEGORY(LogTemp, Fatal, __VA_ARGS__)
#define CERROR(...) LOG_CATEGORY(LogTemp, Error, __VA_ARGS__)
#define CWARN(...) LOG_CATEGORY(LogTemp, Warning, __VA_ARGS__)
#define CDISPLAY(...) LOG_CATEGORY(LogTemp, Display, __VA_ARGS__)
#define CLOG(...) LOG_CATEGORY(LogTemp, Msg, __VA_ARGS__)
#define CDEBUG(...) LOG_CATEGORY(LogTemp, Verbose, __VA_ARGS__)
#define CTRACE(...) LOG_CATEGORY(LogTemp, VeryVerbose, __VA_ARGS__)


#define CHECK(Expression, ...)                            \
  do                                                         \
  {                                                          \
    if (!(Expression))                                       \
    {                                                        \
      FATAL("Check failed: " #Expression, ##__VA_ARGS__); \
    }                                                        \
  } while (0)

#define ASSERT(Expression, ...)                               \
  do                                                             \
  {                                                              \
    if (!(Expression))                                           \
    {                                                            \
      ERROR("Assertion failed: " #Expression, ##__VA_ARGS__); \
    }                                                            \
  } while (0)


#define DECLARE_LOG_CATEGORY_EXTERN(CategoryName) extern CE::CLogCategory CategoryName;
#define DEFINE_LOG_CATEGORY(CategoryName) CE::CLogCategory CategoryName(#CategoryName);
