#include "nwchemex/nwchemex.hpp"
#include <catch2/catch.hpp>
#include <mokup/mokup.hpp>

using pt = simde::TotalCanonicalEnergy;

TEST_CASE("Direct SCF") {
    const auto name = mokup::molecule::h2o;
    const auto bs   = mokup::basis_set::sto3g;
    auto aos        = mokup::get_bases(name, bs);
    auto H          = mokup::hamiltonian(name);
    simde::type::els_hamiltonian H_e(H);

    pluginplay::ModuleManager mm;
    nwchemex::load_modules(mm);

    // For density fitted. STO-3G is a terrible fitting set.
    //    mm.change_input("DFJK", "Fitting Basis", aos);
    //    mm.change_submod("Fock Matrix", "J Builder", "DFJK");
    //    mm.change_submod("Fock Matrix", "K Builder", "DFJK");

    auto& mod = mm.at("SCF");

    auto [phi0] = mod.run_as<simde::CanonicalReference>(H_e, aos);
    auto [E]    = mm.at("Total Energy").run_as<pt>(phi0, H, phi0);
    std::cout << "Total SCF Energy: " << E << std::endl;
}
