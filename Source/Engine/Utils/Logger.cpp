#include "Engine/Utils/Logger.h"

namespace CE
{
  CLogCategory LogTemp("LogTemp");
  CLogCategory LogCore("LogCore");
  CLogCategory LogRender("LogRender");
  CLogCategory LogInput("LogInput");
  CLogCategory LogAudio("LogAudio");
  CLogCategory LogGameplay("LogGameplay");
  CLogCategory LogEditor("LogEditor");
  CLogCategory LogSystem("LogSystem");

  // Static member initialization
  ELogLevel CLogger::s_globalLevel = ELogLevel::Log;
  bool CLogger::s_consoleOutput = true;
  bool CLogger::s_fileOutput = true;
  bool CLogger::s_initialized = false;
  bool CLogger::s_useUniqueLogFile = true;
  bool CLogger::s_overwriteExisting = false;
  std::ofstream CLogger::s_logFile;
  std::unordered_map<const CLogCategory*, ELogLevel> CLogger::s_categoryLevels;

  void CLogger::Initialize(bool useUniqueLogFile, bool overwriteExisting)
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
    CORE_DISPLAY("Logger initialized");
    CORE_DISPLAY("Log file: ", logPath.string());
  }

  void CLogger::Shutdown()
  {
    if (!s_initialized)
      return;

    CORE_DISPLAY("Logger shutting down");

    if (s_logFile.is_open())
    {
      s_logFile.close();
    }

    s_initialized = false;
  }

  void CLogger::SetGlobalLogLevel(ELogLevel level)
  {
    s_globalLevel = level;
    CORE_DISPLAY("Global log level set to: ", GetLevelString(level));
  }

  void CLogger::SetCategoryLogLevel(const CLogCategory& category, ELogLevel level)
  {
    s_categoryLevels[&category] = level;
    CORE_DISPLAY("Category ", category.GetName(), " log level set to: ", GetLevelString(level));
  }

  void CLogger::SetConsoleOutput(bool enable)
  {
    s_consoleOutput = enable;
    CORE_DISPLAY("Console output: ", enable ? "enabled" : "disabled");
  }

  void CLogger::SetFileOutput(bool enable)
  {
    s_fileOutput = enable;
    CORE_DISPLAY("File output: ", enable ? "enabled" : "disabled");
  }

  void CLogger::WriteToConsole(ELogLevel level, const CLogCategory& category, const std::string& message)
  {
    (void)category;  // Помечаем как неиспользуемый

#ifdef _WIN32
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
    WORD color = 7;  // Default white

    // Цвета как в Unreal Engine
    switch (level)
    {
      case ELogLevel::Fatal:
        color = BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
        break;
      case ELogLevel::Error:
        color = FOREGROUND_RED | FOREGROUND_INTENSITY;
        break;
      case ELogLevel::Warning:
        color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;  // Yellow
        break;
      case ELogLevel::Display:
        color = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
        break;
      case ELogLevel::Log:
        color = 7;  // White
        break;
      case ELogLevel::Verbose:
        color = 8;  // Gray
        break;
      case ELogLevel::VeryVerbose:
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
      case ELogLevel::Fatal:
        colorCode = "\033[1;41;37m";
        break;  // White on red background
      case ELogLevel::Error:
        colorCode = "\033[1;31m";
        break;  // Bright Red
      case ELogLevel::Warning:
        colorCode = "\033[1;33m";
        break;  // Bright Yellow
      case ELogLevel::Display:
        colorCode = "\033[1;32m";
        break;  // Bright Green
      case ELogLevel::Log:
        colorCode = "\033[0m";
        break;  // Normal
      case ELogLevel::Verbose:
        colorCode = "\033[90m";
        break;  // Gray
      case ELogLevel::VeryVerbose:
        colorCode = "\033[90m";
        break;  // Gray
    }

    std::cout << colorCode << message << "\033[0m" << std::endl;
#endif
  }

  void CLogger::WriteToFile(const std::string& message)
  {
    if (s_logFile.is_open())
    {
      s_logFile << message << std::endl;
      s_logFile.flush();
    }
  }

  std::string CLogger::GetCurrentTimeStamp()
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

  std::string CLogger::GetLevelString(ELogLevel level)
  {
    switch (level)
    {
      case ELogLevel::Fatal:
        return "Fatal";
      case ELogLevel::Error:
        return "Error";
      case ELogLevel::Warning:
        return "Warning";
      case ELogLevel::Display:
        return "Display";
      case ELogLevel::Log:
        return "Log";
      case ELogLevel::Verbose:
        return "Verbose";
      case ELogLevel::VeryVerbose:
        return "VeryVerbose";
      default:
        return "Unknown";
    }
  }

  std::string CLogger::GetLevelDisplayName(ELogLevel level)
  {
    switch (level)
    {
      case ELogLevel::Fatal:
        return "Fatal";
      case ELogLevel::Error:
        return "Error";
      case ELogLevel::Warning:
        return "Warning";
      case ELogLevel::Display:
        return "Display";
      case ELogLevel::Log:
        return "Log";
      case ELogLevel::Verbose:
        return "Verbose";
      case ELogLevel::VeryVerbose:
        return "VeryVerbose";
      default:
        return "Unknown";
    }
  }

  std::string CLogger::GetLogFilePath()
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