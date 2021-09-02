#include "nwchemex/nwchemex.hpp"
#include <catch2/catch.hpp>

// 3. Change this to the property type you want to run as
using pt_type = simde::CoreH;

TEST_CASE("Driving NWX from C++") {
    pluginplay::ModuleManager mm;
    nwx::load_modules(mm);
    libchemist::MoleculeManager mols;

    auto mol = mols.at("water");
    auto bs  = libchemist::apply_basis("cc-pvdz", mol);

    // 4. Change to the module key you want to run
    const auto module_key = "CoreH";
    // 5. (Optional) Change options

    // 6. Run the module
    // const auto& [H] = mm.run_as<pt_type>(module_key, bs, bs);
}
