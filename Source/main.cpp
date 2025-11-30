#include "CoreMinimal.h"
#include "Engine/Core/GuardedMain.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMyGame);
DEFINE_LOG_CATEGORY(LogMyGame)

int main(int argc, char* argv[])
{
  // Инициализация логгера ДО всего
  CE::CLogger::Initialize(false, true);

#ifdef _DEBUG
  CE::CLogger::SetGlobalLogLevel(CE::ELogLevel::Verbose);
  CORE_DISPLAY("=== DEBUG BUILD ===");
#else
  CE::CLogger::SetGlobalLogLevel(CE::ELogLevel::Warning);
  CORE_DISPLAY("=== RELEASE BUILD ===");
#endif

  int result = 0;

  try
  {
    result = GuardedMain(argc, argv);
  }
  catch (const std::exception& e)
  {
    CORE_FATAL("Unhandled exception: ", e.what());
    result = -1;
  }
  catch (...)
  {
    CORE_FATAL("Unknown exception occurred");
    result = -1;
  }

  // Завершение логгера ПОСЛЕ всего
  CE::CLogger::Shutdown();

  return result;
}
