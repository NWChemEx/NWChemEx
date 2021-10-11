#include "nwchemex/nwchemex.hpp"
#include <catch2/catch.hpp>
#include <mokup/mokup.hpp>

using scf_wf_pt    = simde::CanonicalReference;
using mp1_wf_pt    = simde::CanonicalManyBodyWf;
using mp2_f12_e_pt = simde::CanonicalCorrelationEnergy;
using k_pt         = simde::TransformedExchange;

TEST_CASE("Canonical MP2-F12") {
    auto& world = TA::get_default_world();
    pluginplay::ModuleManager mm;
    nwx::load_modules(mm);

    const auto name   = mokup::molecule::h2;
    const auto bs     = mokup::basis_set::ccpvdz;
    const auto aux_bs = mokup::basis_set::ccpvdzf12optri;
    auto mol          = mokup::get_molecule(name);
    auto aos          = mokup::get_bases(name, bs);
    const auto aux    = mokup::get_bases(name, aux_bs);
    auto H            = mokup::hamiltonian(name);
    simde::type::els_hamiltonian H_e(H);

    mm.change_input("CABS", "F12 fitting basis", aux);

    auto scf_wf_mod    = mm.at("SCF");
    auto mp1_wf_mod    = mm.at("MP1 Wavefunction");
    auto mp2_f12_e_mod = mm.at("Dense MP2-F12");

    auto [scf_wf] = scf_wf_mod.run_as<scf_wf_pt>(H_e, aos);
    auto [mp1_wf] = mp1_wf_mod.run_as<mp1_wf_pt>(H_e, scf_wf);
    auto [E]      = mp2_f12_e_mod.run_as<mp2_f12_e_pt>(scf_wf, H_e, mp1_wf);
    std::cout << "MP2-F12/STO-3G Correlation Energy: " << E << std::endl;
}
