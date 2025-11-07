#include "Application/Application.h"
#include "Core/AppInfo.h"
#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMyGame);
DEFINE_LOG_CATEGORY(LogMyGame)

int main()
{
  CE::Logger::Initialize(true, false);

#ifdef _DEBUG
  CE::Logger::SetGlobalLogLevel(CE::LogLevel::Verbose);
  CE_CORE_DISPLAY("=== DEBUG BUILD ===");
#else
  CE::Logger::SetGlobalLogLevel(CE::LogLevel::Warning);
  CE_CORE_DISPLAY("=== RELEASE BUILD ===");
#endif

  CE_CORE_DISPLAY("=== ChuddoEngine Starting ===");

  auto ApInfo = CE::AppInfo();

  // Вся логика теперь в Application
  auto app = CE::Application(&ApInfo);
  app.Initialize();
  app.Run();
  app.Shutdown();

  CE::Logger::Shutdown();
  return 0;
}