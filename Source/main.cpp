#include "Core/GuardedMain.h"
#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMyGame);
DEFINE_LOG_CATEGORY(LogMyGame)

int main(int argc, char* argv[])
{
  // Инициализация логгера ДО всего
  CE::Logger::Initialize(false, true);

#ifdef _DEBUG
  CE::Logger::SetGlobalLogLevel(CE::LogLevel::Verbose);
  CE_CORE_DISPLAY("=== DEBUG BUILD ===");
#else
  CE::Logger::SetGlobalLogLevel(CE::LogLevel::Warning);
  CE_CORE_DISPLAY("=== RELEASE BUILD ===");
#endif

  int result = 0;

  try
  {
    result = CE::GuardedMain(argc, argv);
  }
  catch (const std::exception& e)
  {
    CE_CORE_FATAL("Unhandled exception: ", e.what());
    result = -1;
  }
  catch (...)
  {
    CE_CORE_FATAL("Unknown exception occurred");
    result = -1;
  }

  // Завершение логгера ПОСЛЕ всего
  CE::Logger::Shutdown();

  return result;
}