#include "Engine/Core/GuardedMain.h"

#include "CoreMinimal.h"
#include "Engine/Core/AppInfo.h"
#include "Engine/Core/CommandLine.h"
#include "Engine/Core/Config.h"
#include "Game/Application/GameApplication.h"


  void ApplyCommandLineOverrides(Config& config);
  AppInfo CreateAppInfoFromConfig();

  int GuardedMain(int argc, char* argv[])
  {
    CORE_DISPLAY("=== ChuddoEngine Starting ===");


    if (argc > 0)
    {
      CommandLine::Parse(argc, argv);
    }

    // Check for headless mode
    bool isHeadless = CommandLine::Get().HasFlag("headless") || CommandLine::Get().HasFlag("h");
    if (isHeadless)
    {
      CORE_DISPLAY("Running in headless mode - skipping rendering");
      return 0;  // Exit early in headless mode
    }

    std::string configFile = CommandLine::Get().GetString("config", "engine.cfg");
    configFile = CommandLine::Get().GetString("c", configFile);  // Короткая версия

    auto& config = Config::Get();
    if (!config.Load(configFile))
    {
      CORE_WARN("Failed to load config: ", configFile, ", using defaults");
    }


    ApplyCommandLineOverrides(config);


    auto ApInfo = CreateAppInfoFromConfig();


    auto app = GameApp(&ApInfo);
    app.Initialize();
    app.Run();
    app.Shutdown();


    config.Save(configFile);

    return 0;
  }

  void ApplyCommandLineOverrides(Config& config)
  {
    auto& cmd = CommandLine::Get();

    // Окно
    if (cmd.HasFlag("width") || cmd.HasFlag("w"))
    {
      int width = cmd.GetInt("width", cmd.GetInt("w", 1024));
      config.SetInt("WindowWidth", width);
      CORE_DISPLAY("Command line override: WindowWidth = ", width);
    }

    if (cmd.HasFlag("height") || cmd.HasFlag("h"))
    {
      int height = cmd.GetInt("height", cmd.GetInt("h", 768));
      config.SetInt("WindowHeight", height);
      CORE_DISPLAY("Command line override: WindowHeight = ", height);
    }

    // ПРИОРИТЕТ: --windowed отключает --fullscreen
    if (cmd.HasFlag("windowed"))
    {
      config.SetBool("Fullscreen", false);
      CORE_DISPLAY("Command line override: Fullscreen = false (windowed)");
    }
    else if (cmd.HasFlag("fullscreen") || cmd.HasFlag("f"))
    {
      config.SetBool("Fullscreen", true);
      CORE_DISPLAY("Command line override: Fullscreen = true");
    }

    if (cmd.HasFlag("vsync"))
    {
      bool vsync = cmd.GetBool("vsync", true);
      config.SetBool("VSync", vsync);
      CORE_DISPLAY("Command line override: VSync = ", vsync);
    }

    if (cmd.HasFlag("msaa"))
    {
      int msaa = cmd.GetInt("msaa", 4);
      config.SetInt("MSAASamples", msaa);
      CORE_DISPLAY("Command line override: MSAASamples = ", msaa);
    }

   
    if (cmd.HasFlag("log-level"))
    {
      int logLevel = cmd.GetInt("log-level", 2);
      config.SetInt("LogLevel", logLevel);
      CORE_DISPLAY("Command line override: LogLevel = ", logLevel);
    }
  }

  AppInfo CreateAppInfoFromConfig()
  {
    auto ApInfo = AppInfo();
    ApInfo.AppName = "Chuddo Engine tests";
    ApInfo.AppVerion = {0, 0, 1};
    ApInfo.EngineName = "Chuddo Engine";
    ApInfo.EngineVersion = {0, 0, 1};

    
    ApInfo.LoadFromConfig();

    return ApInfo;
  }