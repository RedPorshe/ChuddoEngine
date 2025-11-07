#include "Utils/Logger.h"

namespace CE
{
  LogCategory LogTemp("LogTemp");
  LogCategory LogCore("LogCore");
  LogCategory LogRender("LogRender");
  LogCategory LogInput("LogInput");
  LogCategory LogAudio("LogAudio");
  LogCategory LogGameplay("LogGameplay");
  LogCategory LogEditor("LogEditor");
  LogCategory LogSystem("LogSystem");

  // Static member initialization
  LogLevel Logger::s_globalLevel = LogLevel::Log;
  bool Logger::s_consoleOutput = true;
  bool Logger::s_fileOutput = true;
  bool Logger::s_initialized = false;
  bool Logger::s_useUniqueLogFile = true;
  bool Logger::s_overwriteExisting = false;
  std::ofstream Logger::s_logFile;
  std::unordered_map<const LogCategory*, LogLevel> Logger::s_categoryLevels;

  void Logger::Initialize(bool useUniqueLogFile, bool overwriteExisting)
  {
    if (s_initialized)
      return;

    s_useUniqueLogFile = useUniqueLogFile;
    s_overwriteExisting = overwriteExisting;

    std::filesystem::path logPath = GetLogFilePath();
    std::filesystem::create_directories(logPath.parent_path());

    std::ios_base::openmode mode = std::ios::out;
    if (!s_overwriteExisting)
    {
      mode |= std::ios::app;
    }

    s_logFile.open(logPath, mode);
    if (!s_logFile.is_open())
    {
      std::cerr << "Failed to open log file: " << logPath << std::endl;
      s_fileOutput = false;
    }

    s_initialized = true;
    CE_CORE_DISPLAY("Logger initialized");
    CE_CORE_DISPLAY("Log file: ", logPath.string());
  }

  void Logger::Shutdown()
  {
    if (!s_initialized)
      return;

    CE_CORE_DISPLAY("Logger shutting down");

    if (s_logFile.is_open())
    {
      s_logFile.close();
    }

    s_initialized = false;
  }

  void Logger::SetGlobalLogLevel(LogLevel level)
  {
    s_globalLevel = level;
    CE_CORE_DISPLAY("Global log level set to: ", GetLevelString(level));
  }

  void Logger::SetCategoryLogLevel(const LogCategory& category, LogLevel level)
  {
    s_categoryLevels[&category] = level;
    CE_CORE_DISPLAY("Category ", category.GetName(), " log level set to: ", GetLevelString(level));
  }

  void Logger::SetConsoleOutput(bool enable)
  {
    s_consoleOutput = enable;
    CE_CORE_DISPLAY("Console output: ", enable ? "enabled" : "disabled");
  }

  void Logger::SetFileOutput(bool enable)
  {
    s_fileOutput = enable;
    CE_CORE_DISPLAY("File output: ", enable ? "enabled" : "disabled");
  }

  void Logger::WriteToConsole(LogLevel level, const LogCategory& category, const std::string& message)
  {
    (void)category;  // Помечаем как неиспользуемый

#ifdef _WIN32
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD color = 7;  // Default white

    // Цвета как в Unreal Engine
    switch (level)
    {
      case LogLevel::Fatal:
        color = BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
      case LogLevel::Error:
        color = FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;
      case LogLevel::Warning:
        color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;  // Yellow
        break;
      case LogLevel::Display:
        color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
      case LogLevel::Log:
        color = 7;  // White
        break;
      case LogLevel::Verbose:
        color = 8;  // Gray
        break;
      case LogLevel::VeryVerbose:
        color = 8;  // Gray
        break;
    }

    SetConsoleTextAttribute(console, color);
    std::cout << message << std::endl;
    SetConsoleTextAttribute(console, 7);  // Reset to white
#else
    const char* colorCode = "";

    switch (level)
    {
      case LogLevel::Fatal:
        colorCode = "\033[1;41;37m";
        break;  // White on red background
      case LogLevel::Error:
        colorCode = "\033[1;31m";
        break;  // Bright Red
      case LogLevel::Warning:
        colorCode = "\033[1;33m";
        break;  // Bright Yellow
      case LogLevel::Display:
        colorCode = "\033[1;32m";
        break;  // Bright Green
      case LogLevel::Log:
        colorCode = "\033[0m";
        break;  // Normal
      case LogLevel::Verbose:
        colorCode = "\033[90m";
        break;  // Gray
      case LogLevel::VeryVerbose:
        colorCode = "\033[90m";
        break;  // Gray
    }

    std::cout << colorCode << message << "\033[0m" << std::endl;
#endif
  }

  void Logger::WriteToFile(const std::string& message)
  {
    if (s_logFile.is_open())
    {
      s_logFile << message << std::endl;
      s_logFile.flush();
    }
  }

  std::string Logger::GetCurrentTimeStamp()
  {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                  now.time_since_epoch()) %
              1000;

    std::tm timeInfo;
#ifdef _WIN32
    localtime_s(&timeInfo, &time_t);
#else
    localtime_r(&time_t, &timeInfo);
#endif

    std::ostringstream stream;
    stream << std::put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
    stream << "." << std::setfill('0') << std::setw(3) << ms.count();

    return stream.str();
  }

  std::string Logger::GetLevelString(LogLevel level)
  {
    switch (level)
    {
      case LogLevel::Fatal:
        return "Fatal";
      case LogLevel::Error:
        return "Error";
      case LogLevel::Warning:
        return "Warning";
      case LogLevel::Display:
        return "Display";
      case LogLevel::Log:
        return "Log";
      case LogLevel::Verbose:
        return "Verbose";
      case LogLevel::VeryVerbose:
        return "VeryVerbose";
      default:
        return "Unknown";
    }
  }

  std::string Logger::GetLevelDisplayName(LogLevel level)
  {
    switch (level)
    {
      case LogLevel::Fatal:
        return "Fatal";
      case LogLevel::Error:
        return "Error";
      case LogLevel::Warning:
        return "Warning";
      case LogLevel::Display:
        return "Display";
      case LogLevel::Log:
        return "Log";
      case LogLevel::Verbose:
        return "Verbose";
      case LogLevel::VeryVerbose:
        return "VeryVerbose";
      default:
        return "Unknown";
    }
  }

  std::string Logger::GetLogFilePath()
  {
    std::filesystem::path exePath = std::filesystem::current_path();
    std::filesystem::path logDir = exePath / "logs";

    std::string filename;
    if (s_useUniqueLogFile)
    {
      auto now = std::chrono::system_clock::now();
      auto time_t = std::chrono::system_clock::to_time_t(now);
      std::tm timeInfo;
#ifdef _WIN32
      localtime_s(&timeInfo, &time_t);
#else
      localtime_r(&time_t, &timeInfo);
#endif

      std::ostringstream stream;
      stream << std::put_time(&timeInfo, "game_%Y-%m-%d_%H-%M-%S.log");
      filename = stream.str();
    }
    else
    {
      filename = "game.log";
    }

    return (logDir / filename).string();
  }
}  // namespace CE