#include "nwchemex/nwchemex.hpp"
#include <catch2/catch.hpp>
#include <mokup/mokup.hpp>

using pt = simde::AOEnergy;

TEST_CASE("DF-SCF") {
    pluginplay::ModuleManager mm;
    nwchemex::load_modules(mm);

    // Grab molecule and build basis sets
    const auto name = mokup::molecule::h2;
    auto mol        = mokup::get_molecule(name);
    auto aos        = nwchemex::apply_basis("cc-pvtz", mol);
    auto aux_aos    = nwchemex::apply_basis("cc-pvtz-jkfit", mol);

    simde::type::chemical_system chem_sys(mol);

    // Apply auxiliary set and change J and K build
    mm.change_input("DFJK", "Fitting Basis", aux_aos);
    mm.change_submod("Fock Matrix", "J Builder", "DFJK");
    mm.change_submod("Fock Matrix", "K Builder", "DFJK");

    // Calculate energy
    auto [E] = mm.at("SCF Energy").run_as<pt>(aos, chem_sys);
    std::cout << "Total DF-SCF/STO-3G Energy: " << E << std::endl;
    REQUIRE(E == Approx(-1.1201712380602133).margin(1.0e-8));
}
