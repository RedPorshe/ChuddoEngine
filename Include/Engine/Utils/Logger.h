#pragma once

#include <chrono>
#include <cstdarg>
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

  static std::string FormatString(const char* format, ...)
  {
    va_list args;
    va_start(args, format);
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    return std::string(buffer);
  }

  static void Log(const CLogCategory& category, ELogLevel level, const std::string& message)
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

    std::string fullMessage = GetCurrentTimeStamp() + " " + GetLevelDisplayName(level) +
                              ": [" + category.GetName() + "] " + message;

    if (s_consoleOutput)
    {
      WriteToConsole(level, category, fullMessage);
    }

    if (s_fileOutput)
    {
      WriteToFile(fullMessage);
    }

    if (level == ELogLevel::Fatal)
    {
      Shutdown();
      std::exit(1);
    }
  }

  static void LogFatal(const CLogCategory& category, const std::string& message);
  static void LogError(const CLogCategory& category, const std::string& message);
  static void LogWarning(const CLogCategory& category, const std::string& message);
  static void LogDisplay(const CLogCategory& category, const std::string& message);
  static void LogMsg(const CLogCategory& category, const std::string& message);
  static void LogVerbose(const CLogCategory& category, const std::string& message);
  static void LogVeryVerbose(const CLogCategory& category, const std::string& message);

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

#define CORE_FATAL(...) CE::CLogger::LogFatal(LogCore, CE::CLogger::FormatString(__VA_ARGS__))
#define CORE_ERROR(...) CE::CLogger::LogError(LogCore, CE::CLogger::FormatString(__VA_ARGS__))
#define CORE_WARN(...) CE::CLogger::LogWarning(LogCore, CE::CLogger::FormatString(__VA_ARGS__))
#define CORE_DISPLAY(...) CE::CLogger::LogDisplay(LogCore, CE::CLogger::FormatString(__VA_ARGS__))
#define CORE_LOG(...) CE::CLogger::LogMsg(LogCore, CE::CLogger::FormatString(__VA_ARGS__))
#define CORE_DEBUG(...) CE::CLogger::LogVerbose(LogCore, CE::CLogger::FormatString(__VA_ARGS__))
#define CORE_TRACE(...) CE::CLogger::LogVeryVerbose(LogCore, CE::CLogger::FormatString(__VA_ARGS__))


#define RENDER_FATAL(...) CE::CLogger::LogFatal(LogRender, CE::CLogger::FormatString(__VA_ARGS__))
#define RENDER_ERROR(...) CE::CLogger::LogError(LogRender, CE::CLogger::FormatString(__VA_ARGS__))
#define RENDER_WARN(...) CE::CLogger::LogWarning(LogRender, CE::CLogger::FormatString(__VA_ARGS__))
#define RENDER_DISPLAY(...) CE::CLogger::LogDisplay(LogRender, CE::CLogger::FormatString(__VA_ARGS__))
#define RENDER_LOG(...) CE::CLogger::LogMsg(LogRender, CE::CLogger::FormatString(__VA_ARGS__))
#define RENDER_DEBUG(...) CE::CLogger::LogVerbose(LogRender, CE::CLogger::FormatString(__VA_ARGS__))
#define RENDER_TRACE(...) CE::CLogger::LogVeryVerbose(LogRender, CE::CLogger::FormatString(__VA_ARGS__))


#define INPUT_FATAL(...) CE::CLogger::LogFatal(LogInput, CE::CLogger::FormatString(__VA_ARGS__))
#define INPUT_ERROR(...) CE::CLogger::LogError(LogInput, CE::CLogger::FormatString(__VA_ARGS__))
#define INPUT_WARN(...) CE::CLogger::LogWarning(LogInput, CE::CLogger::FormatString(__VA_ARGS__))
#define INPUT_DISPLAY(...) CE::CLogger::LogDisplay(LogInput, CE::CLogger::FormatString(__VA_ARGS__))
#define INPUT_LOG(...) CE::CLogger::LogMsg(LogInput, CE::CLogger::FormatString(__VA_ARGS__))
#define INPUT_DEBUG(...) CE::CLogger::LogVerbose(LogInput, CE::CLogger::FormatString(__VA_ARGS__))
#define INPUT_TRACE(...) CE::CLogger::LogVeryVerbose(LogInput, CE::CLogger::FormatString(__VA_ARGS__))


#define AUDIO_FATAL(...) CE::CLogger::LogFatal(LogAudio, CE::CLogger::FormatString(__VA_ARGS__))
#define AUDIO_ERROR(...) CE::CLogger::LogError(LogAudio, CE::CLogger::FormatString(__VA_ARGS__))
#define AUDIO_WARN(...) CE::CLogger::LogWarning(LogAudio, CE::CLogger::FormatString(__VA_ARGS__))
#define AUDIO_DISPLAY(...) CE::CLogger::LogDisplay(LogAudio, CE::CLogger::FormatString(__VA_ARGS__))
#define AUDIO_LOG(...) CE::CLogger::LogMsg(LogAudio, CE::CLogger::FormatString(__VA_ARGS__))
#define AUDIO_DEBUG(...) CE::CLogger::LogVerbose(LogAudio, CE::CLogger::FormatString(__VA_ARGS__))
#define AUDIO_TRACE(...) CE::CLogger::LogVeryVerbose(LogAudio, CE::CLogger::FormatString(__VA_ARGS__))


#define GAMEPLAY_FATAL(...) CE::CLogger::LogFatal(LogGameplay, CE::CLogger::FormatString(__VA_ARGS__))
#define GAMEPLAY_ERROR(...) CE::CLogger::LogError(LogGameplay, CE::CLogger::FormatString(__VA_ARGS__))
#define GAMEPLAY_WARN(...) CE::CLogger::LogWarning(LogGameplay, CE::CLogger::FormatString(__VA_ARGS__))
#define GAMEPLAY_DISPLAY(...) CE::CLogger::LogDisplay(LogGameplay, CE::CLogger::FormatString(__VA_ARGS__))
#define GAMEPLAY_LOG(...) CE::CLogger::LogMsg(LogGameplay, CE::CLogger::FormatString(__VA_ARGS__))
#define GAMEPLAY_DEBUG(...) CE::CLogger::LogVerbose(LogGameplay, CE::CLogger::FormatString(__VA_ARGS__))
#define GAMEPLAY_TRACE(...) CE::CLogger::LogVeryVerbose(LogGameplay, CE::CLogger::FormatString(__VA_ARGS__))


#define EDITOR_FATAL(...) CE::CLogger::LogFatal(LogEditor, CE::CLogger::FormatString(__VA_ARGS__))
#define EDITOR_ERROR(...) CE::CLogger::LogError(LogEditor, CE::CLogger::FormatString(__VA_ARGS__))
#define EDITOR_WARN(...) CE::CLogger::LogWarning(LogEditor, CE::CLogger::FormatString(__VA_ARGS__))
#define EDITOR_DISPLAY(...) CE::CLogger::LogDisplay(LogEditor, CE::CLogger::FormatString(__VA_ARGS__))
#define EDITOR_LOG(...) CE::CLogger::LogMsg(LogEditor, CE::CLogger::FormatString(__VA_ARGS__))
#define EDITOR_DEBUG(...) CE::CLogger::LogVerbose(LogEditor, CE::CLogger::FormatString(__VA_ARGS__))
#define EDITOR_TRACE(...) CE::CLogger::LogVeryVerbose(LogEditor, CE::CLogger::FormatString(__VA_ARGS__))


#define SYSTEM_FATAL(...) CE::CLogger::LogFatal(LogSystem, CE::CLogger::FormatString(__VA_ARGS__))
#define SYSTEM_ERROR(...) CE::CLogger::LogError(LogSystem, CE::CLogger::FormatString(__VA_ARGS__))
#define SYSTEM_WARN(...) CE::CLogger::LogWarning(LogSystem, CE::CLogger::FormatString(__VA_ARGS__))
#define SYSTEM_DISPLAY(...) CE::CLogger::LogDisplay(LogSystem, CE::CLogger::FormatString(__VA_ARGS__))
#define SYSTEM_LOG(...) CE::CLogger::LogMsg(LogSystem, CE::CLogger::FormatString(__VA_ARGS__))
#define SYSTEM_DEBUG(...) CE::CLogger::LogVerbose(LogSystem, CE::CLogger::FormatString(__VA_ARGS__))
#define SYSTEM_TRACE(...) CE::CLogger::LogVeryVerbose(LogSystem, CE::CLogger::FormatString(__VA_ARGS__))


#define CFATAL(...) CE::CLogger::LogFatal(LogTemp, CE::CLogger::FormatString(__VA_ARGS__))
#define CERROR(...) CE::CLogger::LogError(LogTemp, CE::CLogger::FormatString(__VA_ARGS__))
#define CWARN(...) CE::CLogger::LogWarning(LogTemp, CE::CLogger::FormatString(__VA_ARGS__))
#define CDISPLAY(...) CE::CLogger::LogDisplay(LogTemp, CE::CLogger::FormatString(__VA_ARGS__))
#define CLOG(...) CE::CLogger::LogMsg(LogTemp, CE::CLogger::FormatString(__VA_ARGS__))
#define CDEBUG(...) CE::CLogger::LogVerbose(LogTemp, CE::CLogger::FormatString(__VA_ARGS__))
#define CTRACE(...) CE::CLogger::LogVeryVerbose(LogTemp, CE::CLogger::FormatString(__VA_ARGS__))


#define CHECK(Expression, ...)                            \
  do                                                         \
  {                                                          \
    if (!(Expression))                                       \
    {                                                        \
      CFATAL("Check failed: " #Expression, ##__VA_ARGS__); \
    }                                                        \
  } while (0)

#define ASSERT(Expression, ...)                               \
  do                                                             \
  {                                                              \
    if (!(Expression))                                           \
    {                                                            \
      CERROR("Assertion failed: " #Expression, ##__VA_ARGS__); \
    }                                                            \
  } while (0)


#define DECLARE_LOG_CATEGORY_EXTERN(CategoryName) extern CE::CLogCategory CategoryName;
#define DEFINE_LOG_CATEGORY(CategoryName) CE::CLogCategory CategoryName(#CategoryName);
