// #include "nwchemex/nwchemex.hpp"
// #include <catch2/catch.hpp>
// #include <mokup/mokup.hpp>

// using pt = simde::AOEnergy;

// TEST_CASE("Canonical MP2") {
//     pluginplay::ModuleManager mm;
//     nwchemex::load_modules(mm);

//     // Grab molecule and build a basis set
//     const auto name = mokup::molecule::h2o;
//     auto mol        = mokup::get_molecule(name);
//     auto aos        = nwchemex::apply_basis("sto-3g", mol);

//     simde::type::chemical_system chem_sys(mol);

//     // Calculate energy
//     auto [E] = mm.at("MP2 Energy").run_as<pt>(aos, chem_sys);
//     std::cout << "MP2/STO-3G Correlation Energy: " << E << std::endl;
//     REQUIRE(E == Approx(-0.049150436516131206).margin(1.0e-8));
// }
