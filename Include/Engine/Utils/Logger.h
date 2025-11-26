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

}  // namespace CE


#define CE_LOG_CATEGORY(Category, Level, ...) CE::CLogger::Log##Level(Category, __VA_ARGS__)


#define CE_CORE_FATAL(...) CE_LOG_CATEGORY(CE::LogCore, Fatal, __VA_ARGS__)
#define CE_CORE_ERROR(...) CE_LOG_CATEGORY(CE::LogCore, Error, __VA_ARGS__)
#define CE_CORE_WARN(...) CE_LOG_CATEGORY(CE::LogCore, Warning, __VA_ARGS__)
#define CE_CORE_DISPLAY(...) CE_LOG_CATEGORY(CE::LogCore, Display, __VA_ARGS__)
#define CE_CORE_LOG(...) CE_LOG_CATEGORY(CE::LogCore, Msg, __VA_ARGS__)
#define CE_CORE_DEBUG(...) CE_LOG_CATEGORY(CE::LogCore, Verbose, __VA_ARGS__)
#define CE_CORE_TRACE(...) CE_LOG_CATEGORY(CE::LogCore, VeryVerbose, __VA_ARGS__)


#define CE_RENDER_FATAL(...) CE_LOG_CATEGORY(CE::LogRender, Fatal, __VA_ARGS__)
#define CE_RENDER_ERROR(...) CE_LOG_CATEGORY(CE::LogRender, Error, __VA_ARGS__)
#define CE_RENDER_WARN(...) CE_LOG_CATEGORY(CE::LogRender, Warning, __VA_ARGS__)
#define CE_RENDER_DISPLAY(...) CE_LOG_CATEGORY(CE::LogRender, Display, __VA_ARGS__)
#define CE_RENDER_LOG(...) CE_LOG_CATEGORY(CE::LogRender, Msg, __VA_ARGS__)
#define CE_RENDER_DEBUG(...) CE_LOG_CATEGORY(CE::LogRender, Verbose, __VA_ARGS__)
#define CE_RENDER_TRACE(...) CE_LOG_CATEGORY(CE::LogRender, VeryVerbose, __VA_ARGS__)


#define CE_INPUT_FATAL(...) CE_LOG_CATEGORY(CE::LogInput, Fatal, __VA_ARGS__)
#define CE_INPUT_ERROR(...) CE_LOG_CATEGORY(CE::LogInput, Error, __VA_ARGS__)
#define CE_INPUT_WARN(...) CE_LOG_CATEGORY(CE::LogInput, Warning, __VA_ARGS__)
#define CE_INPUT_DISPLAY(...) CE_LOG_CATEGORY(CE::LogInput, Display, __VA_ARGS__)
#define CE_INPUT_LOG(...) CE_LOG_CATEGORY(CE::LogInput, Msg, __VA_ARGS__)
#define CE_INPUT_DEBUG(...) CE_LOG_CATEGORY(CE::LogInput, Verbose, __VA_ARGS__)
#define CE_INPUT_TRACE(...) CE_LOG_CATEGORY(CE::LogInput, VeryVerbose, __VA_ARGS__)


#define CE_AUDIO_FATAL(...) CE_LOG_CATEGORY(CE::LogAudio, Fatal, __VA_ARGS__)
#define CE_AUDIO_ERROR(...) CE_LOG_CATEGORY(CE::LogAudio, Error, __VA_ARGS__)
#define CE_AUDIO_WARN(...) CE_LOG_CATEGORY(CE::LogAudio, Warning, __VA_ARGS__)
#define CE_AUDIO_DISPLAY(...) CE_LOG_CATEGORY(CE::LogAudio, Display, __VA_ARGS__)
#define CE_AUDIO_LOG(...) CE_LOG_CATEGORY(CE::LogAudio, Msg, __VA_ARGS__)
#define CE_AUDIO_DEBUG(...) CE_LOG_CATEGORY(CE::LogAudio, Verbose, __VA_ARGS__)
#define CE_AUDIO_TRACE(...) CE_LOG_CATEGORY(CE::LogAudio, VeryVerbose, __VA_ARGS__)


#define CE_GAMEPLAY_FATAL(...) CE_LOG_CATEGORY(CE::LogGameplay, Fatal, __VA_ARGS__)
#define CE_GAMEPLAY_ERROR(...) CE_LOG_CATEGORY(CE::LogGameplay, Error, __VA_ARGS__)
#define CE_GAMEPLAY_WARN(...) CE_LOG_CATEGORY(CE::LogGameplay, Warning, __VA_ARGS__)
#define CE_GAMEPLAY_DISPLAY(...) CE_LOG_CATEGORY(CE::LogGameplay, Display, __VA_ARGS__)
#define CE_GAMEPLAY_LOG(...) CE_LOG_CATEGORY(CE::LogGameplay, Msg, __VA_ARGS__)
#define CE_GAMEPLAY_DEBUG(...) CE_LOG_CATEGORY(CE::LogGameplay, Verbose, __VA_ARGS__)
#define CE_GAMEPLAY_TRACE(...) CE_LOG_CATEGORY(CE::LogGameplay, VeryVerbose, __VA_ARGS__)


#define CE_EDITOR_FATAL(...) CE_LOG_CATEGORY(CE::LogEditor, Fatal, __VA_ARGS__)
#define CE_EDITOR_ERROR(...) CE_LOG_CATEGORY(CE::LogEditor, Error, __VA_ARGS__)
#define CE_EDITOR_WARN(...) CE_LOG_CATEGORY(CE::LogEditor, Warning, __VA_ARGS__)
#define CE_EDITOR_DISPLAY(...) CE_LOG_CATEGORY(CE::LogEditor, Display, __VA_ARGS__)
#define CE_EDITOR_LOG(...) CE_LOG_CATEGORY(CE::LogEditor, Msg, __VA_ARGS__)
#define CE_EDITOR_DEBUG(...) CE_LOG_CATEGORY(CE::LogEditor, Verbose, __VA_ARGS__)
#define CE_EDITOR_TRACE(...) CE_LOG_CATEGORY(CE::LogEditor, VeryVerbose, __VA_ARGS__)


#define CE_SYSTEM_FATAL(...) CE_LOG_CATEGORY(CE::LogSystem, Fatal, __VA_ARGS__)
#define CE_SYSTEM_ERROR(...) CE_LOG_CATEGORY(CE::LogSystem, Error, __VA_ARGS__)
#define CE_SYSTEM_WARN(...) CE_LOG_CATEGORY(CE::LogSystem, Warning, __VA_ARGS__)
#define CE_SYSTEM_DISPLAY(...) CE_LOG_CATEGORY(CE::LogSystem, Display, __VA_ARGS__)
#define CE_SYSTEM_LOG(...) CE_LOG_CATEGORY(CE::LogSystem, Msg, __VA_ARGS__)
#define CE_SYSTEM_DEBUG(...) CE_LOG_CATEGORY(CE::LogSystem, Verbose, __VA_ARGS__)
#define CE_SYSTEM_TRACE(...) CE_LOG_CATEGORY(CE::LogSystem, VeryVerbose, __VA_ARGS__)


#define CE_FATAL(...) CE_LOG_CATEGORY(CE::LogTemp, Fatal, __VA_ARGS__)
#define CE_ERROR(...) CE_LOG_CATEGORY(CE::LogTemp, Error, __VA_ARGS__)
#define CE_WARN(...) CE_LOG_CATEGORY(CE::LogTemp, Warning, __VA_ARGS__)
#define CE_DISPLAY(...) CE_LOG_CATEGORY(CE::LogTemp, Display, __VA_ARGS__)
#define CE_LOG(...) CE_LOG_CATEGORY(CE::LogTemp, Msg, __VA_ARGS__)
#define CE_DEBUG(...) CE_LOG_CATEGORY(CE::LogTemp, Verbose, __VA_ARGS__)
#define CE_TRACE(...) CE_LOG_CATEGORY(CE::LogTemp, VeryVerbose, __VA_ARGS__)


#define CE_CHECK(Expression, ...)                            \
  do                                                         \
  {                                                          \
    if (!(Expression))                                       \
    {                                                        \
      CE_FATAL("Check failed: " #Expression, ##__VA_ARGS__); \
    }                                                        \
  } while (0)

#define CE_ASSERT(Expression, ...)                               \
  do                                                             \
  {                                                              \
    if (!(Expression))                                           \
    {                                                            \
      CE_ERROR("Assertion failed: " #Expression, ##__VA_ARGS__); \
    }                                                            \
  } while (0)


#define DECLARE_LOG_CATEGORY_EXTERN(CategoryName) extern CE::CLogCategory CategoryName;
#define DEFINE_LOG_CATEGORY(CategoryName) CE::CLogCategory CategoryName(#CategoryName);