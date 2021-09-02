#include "nwchemex/nwchemex.hpp"
#include <catch2/catch.hpp>
#include <mokup/mokup.hpp>

using pt = simde::TotalCanonicalEnergy;

TEST_CASE("Direct SCF") {
    pluginplay::ModuleManager mm;
    nwx::load_modules(mm);
    auto& mod = mm.at("SCF");

    const auto name = mokup::molecule::h2o;
    const auto bs   = mokup::basis_set::sto3g;
    auto mol        = mokup::get_molecules().at(name);
    auto aos        = mokup::get_bases().at(name).at(bs);
    auto H          = mokup::hamiltonian(name);
    simde::type::els_hamiltonian H_e(H);
    auto [phi0] = mod.run_as<simde::CanonicalReference>(H_e, aos);
    auto [E]    = mm.at("Total Energy").run_as<pt>(phi0, H, phi0);
    std::cout << "Total SCF Energy: " << E << std::endl;
}
