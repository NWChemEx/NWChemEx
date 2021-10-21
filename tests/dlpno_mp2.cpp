#include "nwchemex/nwchemex.hpp"
#include <catch2/catch.hpp>
#include <mokup/mokup.hpp>

using local_pt = simde::NoncanonicalToLocal;

TEST_CASE("MP2 Dipole Approximation") {
    pluginplay::ModuleManager mm;
    nwx::load_modules(mm);

    // Basic input
    const auto name = mokup::molecule::h2o;
    const auto bs   = mokup::basis_set::sto3g;
    auto aos        = mokup::get_bases(name, bs);

    // TODO: Call the module which makes a Hamiltonian
    auto H = mokup::hamiltonian(name);
    simde::type::els_hamiltonian H_e(H);

    // SCF wavefunction
    auto& scf_wf_mod  = mm.at("SCF");
    const auto& [ref] = scf_wf_mod.run_as<simde::CanonicalReference>(H_e, aos);

    // TODO: Orbitals are canonical, need to localize and make a non-canoical
    // wavefunction with the results.

    // Convert non-canonical wf to LMO/PAO wavefunction
    auto& local_mod          = mm.at("LMO-PAO Wavefunction");
    const auto& [sparse_ref] = local_mod.run_as<local_pt>(ref);
    const auto& lmos         = sparse_ref.basis_set().occupied_orbitals();
    const auto& paos         = sparse_ref.basis_set().virtual_orbitals();

    // Run dipoles
    auto& dip_mod     = mm.at("MP2 Dipole");
    const auto& [eij] = dip_mod.run_as<simde::SparsePairEnergy>(lmos, paos);
}
