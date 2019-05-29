#include <nwchemex/nwchemex_mm.hpp>
#include <property_types/energy.hpp>

int main() {
    //Make a ModuleManager and load it up
    sde::ModuleManager mm;
    nwchemex::load_modules(mm);

    //Make your molecular system and apply a basis set
    auto mol = libchemist::MoleculeManager().at("water");
    auto bs  = libchemist::apply_basis("sto-3g", mol);

    //Bind this run's options to the relevant modules
    mm.at("SCF").change_input("Basis Set", bs);

    //Run your modules
    auto [egy] = mm.at("SCF").run_as<property_types::Energy<>>(mol);

    return 0;
}
