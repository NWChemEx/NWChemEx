#include "nwchemex/nwchemex.hpp"
#include <catch2/catch.hpp>
#include <mokup/mokup.hpp>

using simde::type::chemical_system;
using ptype = simde::SystemHamiltonian;

TEST_CASE("System Hamiltonian") {
    pluginplay::ModuleManager mm;
    nwchemex::load_modules(mm);
    auto mod = mm.at("SystemHamiltonian");

    auto name   = mokup::molecule::h2o;
    auto mol    = mokup::get_molecule(name);
    auto H_corr = mokup::hamiltonian(name);

    chemical_system sys{mol, 10};

    auto [H] = mod.run_as<ptype>(sys);
    REQUIRE(H == H_corr);
}
