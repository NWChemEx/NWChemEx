#include "nwchemex/nwchemex.hpp"
#include <catch2/catch.hpp>
#include <mokup/mokup.hpp>

using scf_wf_pt = simde::CanonicalReference;
using mp1_wf_pt = simde::CanonicalManyBodyWf;
using mp2_e_pt  = simde::CanonicalCorrelationEnergy;

TEST_CASE("Canonical MP2") {
    auto& world = TA::get_default_world();
    pluginplay::ModuleManager mm;
    nwx::load_modules(mm);

    const auto name = mokup::molecule::h2o;
    const auto bs   = mokup::basis_set::sto3g;
    auto mol        = mokup::get_molecules().at(name);
    auto aos        = mokup::get_bases().at(name).at(bs);
    auto H          = mokup::hamiltonian(name);
    simde::type::els_hamiltonian H_e(H);
    auto scf_wf_mod = mm.at("SCF");
    auto mp1_wf_mod = mm.at("MP1 Wavefunction");
    auto [scf_wf]   = scf_wf_mod.run_as<scf_wf_pt>(H_e, aos);
    auto [mp1_wf]   = mp1_wf_mod.run_as<mp1_wf_pt>(H_e, scf_wf);
    auto [E]        = mm.run_as<mp2_e_pt>("MP2", scf_wf, H_e, mp1_wf);
    std::cout << E << std::endl;
}
