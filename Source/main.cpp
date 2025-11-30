#include "CoreMinimal.h"
#include "Engine/Core/GuardedMain.h"

DECLARE_LOG_CATEGORY_EXTERN(LogMyGame);
DEFINE_LOG_CATEGORY(LogMyGame)

int main(int argc, char* argv[])
{
  // Инициализация логгера ДО всего
  CLogger::Initialize(false, true);

#ifdef _DEBUG
  CLogger::SetGlobalLogLevel(ELogLevel::Verbose);
  CE_CORE_DISPLAY("=== DEBUG BUILD ===");
#else
  CLogger::SetGlobalLogLevel(ELogLevel::Warning);
  CE_CORE_DISPLAY("=== RELEASE BUILD ===");
#endif

  int result = 0;

  try
  {
    result = GuardedMain(argc, argv);
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
  CLogger::Shutdown();

  return result;
}
