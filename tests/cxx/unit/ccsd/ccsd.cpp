#include "nwchemex/nwchemex.hpp"
#include <catch2/catch.hpp>
#include <mokup/mokup.hpp>

using scf_wf_pt = simde::CanonicalReference;
using ccsd_e_pt = simde::CorrelationEnergy<simde::type::canonical_reference,
                                           simde::type::canonical_reference>;

TEST_CASE("Canonical CCSD") {
    auto& world = TA::get_default_world();
    pluginplay::ModuleManager mm;
    nwchemex::load_modules(mm);

    const auto name = mokup::molecule::h2o;
    auto mol        = mokup::get_molecule(name);
    const auto bs   = mokup::basis_set::sto3g;
    auto aos        = mokup::get_bases(name, bs);
    auto H          = mokup::hamiltonian(name);
    simde::type::chemical_system chem_sys(mol);

    simde::type::els_hamiltonian H_e(H);
    auto scf_wf_mod = mm.at("SCF Wavefunction");
    auto scf_wf   = scf_wf_mod.run_as<scf_wf_pt>(H_e, aos);

    auto scf_te = mm.at("SCF Energy").run_as<simde::AOEnergy>(aos, chem_sys);

    mm.change_input("CCSD", "threshold", 1e-9);
    auto E = mm.run_as<ccsd_e_pt>("CCSD", scf_wf, H_e, scf_wf);
    std::cout << "CCSD/STO-3G Correlation Energy: " << E << std::endl;
    std::cout << "CCSD/STO-3G Total Energy: " << E + scf_te << std::endl;
}
