#include "Engine/Core/Reflection.h"

namespace CE
{
  // Implementation of reflection system
  // Most functionality is in the header due to templates
  // This file can be used for any runtime initialization if needed

  void InitializeReflection()
  {
    // Any global reflection initialization can go here
    CE_CORE_DEBUG("Reflection system initialized");
  }

  void ShutdownReflection()
  {
    // Cleanup if needed
    CE_CORE_DEBUG("Reflection system shutdown");
  }

}  // namespace CE
