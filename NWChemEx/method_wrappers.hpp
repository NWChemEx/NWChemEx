#include <sde/types.hpp>
#include <sde/module_manager.hpp>
  
namespace NWChemEx {

  void change_module(sde::ModuleManager& mm, const std::string& atMod, const std::string& oldMod, const std::string& newMod);
  void change_module(sde::ModuleManager& mm, sde::Module& mod, const std::string& oldMod, const std::string& newMod);
  
}
