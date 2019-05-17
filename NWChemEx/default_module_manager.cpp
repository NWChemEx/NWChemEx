#include <NWChemEx/default_module_manager.hpp>

#include <scf/scf_mm.hpp>

namespace NWChemEx {
  sde::ModuleManager default_mm(){
    auto mm = sde::ModuleManager();
    scf::load_modules(mm);
    return mm;
  }
}
