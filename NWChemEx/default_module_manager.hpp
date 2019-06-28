#include <sde/module_manager.hpp>

namespace NWChemEx {
  sde::ModuleManager default_mm();
  void load_mm(sde::ModuleManager&& mm);
}
