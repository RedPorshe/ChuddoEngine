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
  enum class LogLevel
  {
    Fatal = 0,  // Самый важный
    Error = 1,
    Warning = 2,
    Display = 3,
    Log = 4,
    Verbose = 5,
    VeryVerbose = 6  // Самый подробный
  };

  class LogCategory
  {
   public:
    constexpr LogCategory(const char* name, LogLevel defaultLevel = LogLevel::Log)
        : name(name), defaultLevel(defaultLevel)
    {
    }

    const char* GetName() const
    {
      return name;
    }
    LogLevel GetDefaultLevel() const
    {
      return defaultLevel;
    }

   private:
    const char* name;
    LogLevel defaultLevel;
  };

  extern LogCategory LogTemp;
  extern LogCategory LogCore;
  extern LogCategory LogRender;
  extern LogCategory LogInput;
  extern LogCategory LogAudio;
  extern LogCategory LogGameplay;
  extern LogCategory LogEditor;
  extern LogCategory LogSystem;

  class Logger
  {
   public:
    static void Initialize(bool useUniqueLogFile = true, bool overwriteExisting = false);
    static void Shutdown();

    static void SetGlobalLogLevel(LogLevel level);
    static void SetCategoryLogLevel(const LogCategory& category, LogLevel level);
    static void SetConsoleOutput(bool enable);
    static void SetFileOutput(bool enable);

    template <typename... Args>
    static void Log(const LogCategory& category, LogLevel level, const Args&... args);

    template <typename... Args>
    static void LogFatal(const LogCategory& category, const Args&... args)
    {
      Log(category, LogLevel::Fatal, args...);
    }

    template <typename... Args>
    static void LogError(const LogCategory& category, const Args&... args)
    {
      Log(category, LogLevel::Error, args...);
    }

    template <typename... Args>
    static void LogWarning(const LogCategory& category, const Args&... args)
    {
      Log(category, LogLevel::Warning, args...);
    }

    template <typename... Args>
    static void LogDisplay(const LogCategory& category, const Args&... args)
    {
      Log(category, LogLevel::Display, args...);
    }

    template <typename... Args>
    static void LogMsg(const LogCategory& category, const Args&... args)
    {
      Log(category, LogLevel::Log, args...);
    }

    template <typename... Args>
    static void LogVerbose(const LogCategory& category, const Args&... args)
    {
      Log(category, LogLevel::Verbose, args...);
    }

    template <typename... Args>
    static void LogVeryVerbose(const LogCategory& category, const Args&... args)
    {
      Log(category, LogLevel::VeryVerbose, args...);
    }

   private:
    static void WriteToConsole(LogLevel level, const LogCategory& category, const std::string& message);
    static void WriteToFile(const std::string& message);
    static std::string GetCurrentTimeStamp();
    static std::string GetLevelString(LogLevel level);
    static std::string GetLevelDisplayName(LogLevel level);
    static std::string GetLogFilePath();

    static LogLevel s_globalLevel;
    static bool s_consoleOutput;
    static bool s_fileOutput;
    static bool s_initialized;
    static bool s_useUniqueLogFile;
    static bool s_overwriteExisting;
    static std::ofstream s_logFile;
    static std::unordered_map<const LogCategory*, LogLevel> s_categoryLevels;
  };

  template <typename... Args>
  void Logger::Log(const LogCategory& category, LogLevel level, const Args&... args)
  {
    if (!s_initialized)
      return;

    LogLevel categoryLevel = s_globalLevel;
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

    if (level == LogLevel::Fatal)
    {
      Shutdown();
      std::exit(1);
    }
  }

}  // namespace CE

// Основной макрос (переименуем чтобы избежать конфликта)
#define CE_LOG_CATEGORY(Category, Level, ...) CE::Logger::Log##Level(Category, __VA_ARGS__)

// Удобные макросы по категориям
// Core категория
#define CE_CORE_FATAL(...) CE_LOG_CATEGORY(CE::LogCore, Fatal, __VA_ARGS__)
#define CE_CORE_ERROR(...) CE_LOG_CATEGORY(CE::LogCore, Error, __VA_ARGS__)
#define CE_CORE_WARN(...) CE_LOG_CATEGORY(CE::LogCore, Warning, __VA_ARGS__)
#define CE_CORE_DISPLAY(...) CE_LOG_CATEGORY(CE::LogCore, Display, __VA_ARGS__)
#define CE_CORE_LOG(...) CE_LOG_CATEGORY(CE::LogCore, Msg, __VA_ARGS__)
#define CE_CORE_DEBUG(...) CE_LOG_CATEGORY(CE::LogCore, Verbose, __VA_ARGS__)
#define CE_CORE_TRACE(...) CE_LOG_CATEGORY(CE::LogCore, VeryVerbose, __VA_ARGS__)

// Render категория
#define CE_RENDER_FATAL(...) CE_LOG_CATEGORY(CE::LogRender, Fatal, __VA_ARGS__)
#define CE_RENDER_ERROR(...) CE_LOG_CATEGORY(CE::LogRender, Error, __VA_ARGS__)
#define CE_RENDER_WARN(...) CE_LOG_CATEGORY(CE::LogRender, Warning, __VA_ARGS__)
#define CE_RENDER_DISPLAY(...) CE_LOG_CATEGORY(CE::LogRender, Display, __VA_ARGS__)
#define CE_RENDER_LOG(...) CE_LOG_CATEGORY(CE::LogRender, Msg, __VA_ARGS__)
#define CE_RENDER_DEBUG(...) CE_LOG_CATEGORY(CE::LogRender, Verbose, __VA_ARGS__)
#define CE_RENDER_TRACE(...) CE_LOG_CATEGORY(CE::LogRender, VeryVerbose, __VA_ARGS__)

// Input категория
#define CE_INPUT_FATAL(...) CE_LOG_CATEGORY(CE::LogInput, Fatal, __VA_ARGS__)
#define CE_INPUT_ERROR(...) CE_LOG_CATEGORY(CE::LogInput, Error, __VA_ARGS__)
#define CE_INPUT_WARN(...) CE_LOG_CATEGORY(CE::LogInput, Warning, __VA_ARGS__)
#define CE_INPUT_DISPLAY(...) CE_LOG_CATEGORY(CE::LogInput, Display, __VA_ARGS__)
#define CE_INPUT_LOG(...) CE_LOG_CATEGORY(CE::LogInput, Msg, __VA_ARGS__)
#define CE_INPUT_DEBUG(...) CE_LOG_CATEGORY(CE::LogInput, Verbose, __VA_ARGS__)
#define CE_INPUT_TRACE(...) CE_LOG_CATEGORY(CE::LogInput, VeryVerbose, __VA_ARGS__)

// Audio категория
#define CE_AUDIO_FATAL(...) CE_LOG_CATEGORY(CE::LogAudio, Fatal, __VA_ARGS__)
#define CE_AUDIO_ERROR(...) CE_LOG_CATEGORY(CE::LogAudio, Error, __VA_ARGS__)
#define CE_AUDIO_WARN(...) CE_LOG_CATEGORY(CE::LogAudio, Warning, __VA_ARGS__)
#define CE_AUDIO_DISPLAY(...) CE_LOG_CATEGORY(CE::LogAudio, Display, __VA_ARGS__)
#define CE_AUDIO_LOG(...) CE_LOG_CATEGORY(CE::LogAudio, Msg, __VA_ARGS__)
#define CE_AUDIO_DEBUG(...) CE_LOG_CATEGORY(CE::LogAudio, Verbose, __VA_ARGS__)
#define CE_AUDIO_TRACE(...) CE_LOG_CATEGORY(CE::LogAudio, VeryVerbose, __VA_ARGS__)

// Gameplay категория
#define CE_GAMEPLAY_FATAL(...) CE_LOG_CATEGORY(CE::LogGameplay, Fatal, __VA_ARGS__)
#define CE_GAMEPLAY_ERROR(...) CE_LOG_CATEGORY(CE::LogGameplay, Error, __VA_ARGS__)
#define CE_GAMEPLAY_WARN(...) CE_LOG_CATEGORY(CE::LogGameplay, Warning, __VA_ARGS__)
#define CE_GAMEPLAY_DISPLAY(...) CE_LOG_CATEGORY(CE::LogGameplay, Display, __VA_ARGS__)
#define CE_GAMEPLAY_LOG(...) CE_LOG_CATEGORY(CE::LogGameplay, Msg, __VA_ARGS__)
#define CE_GAMEPLAY_DEBUG(...) CE_LOG_CATEGORY(CE::LogGameplay, Verbose, __VA_ARGS__)
#define CE_GAMEPLAY_TRACE(...) CE_LOG_CATEGORY(CE::LogGameplay, VeryVerbose, __VA_ARGS__)

// Editor категория
#define CE_EDITOR_FATAL(...) CE_LOG_CATEGORY(CE::LogEditor, Fatal, __VA_ARGS__)
#define CE_EDITOR_ERROR(...) CE_LOG_CATEGORY(CE::LogEditor, Error, __VA_ARGS__)
#define CE_EDITOR_WARN(...) CE_LOG_CATEGORY(CE::LogEditor, Warning, __VA_ARGS__)
#define CE_EDITOR_DISPLAY(...) CE_LOG_CATEGORY(CE::LogEditor, Display, __VA_ARGS__)
#define CE_EDITOR_LOG(...) CE_LOG_CATEGORY(CE::LogEditor, Msg, __VA_ARGS__)
#define CE_EDITOR_DEBUG(...) CE_LOG_CATEGORY(CE::LogEditor, Verbose, __VA_ARGS__)
#define CE_EDITOR_TRACE(...) CE_LOG_CATEGORY(CE::LogEditor, VeryVerbose, __VA_ARGS__)

// System категория
#define CE_SYSTEM_FATAL(...) CE_LOG_CATEGORY(CE::LogSystem, Fatal, __VA_ARGS__)
#define CE_SYSTEM_ERROR(...) CE_LOG_CATEGORY(CE::LogSystem, Error, __VA_ARGS__)
#define CE_SYSTEM_WARN(...) CE_LOG_CATEGORY(CE::LogSystem, Warning, __VA_ARGS__)
#define CE_SYSTEM_DISPLAY(...) CE_LOG_CATEGORY(CE::LogSystem, Display, __VA_ARGS__)
#define CE_SYSTEM_LOG(...) CE_LOG_CATEGORY(CE::LogSystem, Msg, __VA_ARGS__)
#define CE_SYSTEM_DEBUG(...) CE_LOG_CATEGORY(CE::LogSystem, Verbose, __VA_ARGS__)
#define CE_SYSTEM_TRACE(...) CE_LOG_CATEGORY(CE::LogSystem, VeryVerbose, __VA_ARGS__)

// Общие макросы (используют LogTemp)
#define CE_FATAL(...) CE_LOG_CATEGORY(CE::LogTemp, Fatal, __VA_ARGS__)
#define CE_ERROR(...) CE_LOG_CATEGORY(CE::LogTemp, Error, __VA_ARGS__)
#define CE_WARN(...) CE_LOG_CATEGORY(CE::LogTemp, Warning, __VA_ARGS__)
#define CE_DISPLAY(...) CE_LOG_CATEGORY(CE::LogTemp, Display, __VA_ARGS__)
#define CE_LOG(...) CE_LOG_CATEGORY(CE::LogTemp, Msg, __VA_ARGS__)
#define CE_DEBUG(...) CE_LOG_CATEGORY(CE::LogTemp, Verbose, __VA_ARGS__)
#define CE_TRACE(...) CE_LOG_CATEGORY(CE::LogTemp, VeryVerbose, __VA_ARGS__)

// Проверки
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

// Макросы для создания категорий
#define DECLARE_LOG_CATEGORY_EXTERN(CategoryName) extern CE::LogCategory CategoryName;
#define DEFINE_LOG_CATEGORY(CategoryName) CE::LogCategory CategoryName(#CategoryName);