#include <NWChemEx/method_wrappers.hpp>
#include <memory>

namespace NWChemEx {

void change_module(sde::ModuleManager& mm, const std::string& atMod, const std::string& oldMod, const std::string& newMod){

  mm.at(atMod).change_submod(oldMod, std::make_shared<sde::Module>(mm.at(newMod)));

}

  void change_module(sde::ModuleManager& mm, sde::Module& mod, const std::string& oldMod, const std::string& newMod){
    mod.change_submod(oldMod, std::make_shared<sde::Module>(mm.at(newMod)));
  }

}
