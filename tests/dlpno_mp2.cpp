/// TODO: Re-enable once sparse map error is solved

// #include "nwchemex/nwchemex.hpp"
// #include <catch2/catch.hpp>
// #include <mokup/mokup.hpp>

// using ham_pt   = simde::SystemHamiltonian;
// using local_pt = simde::NoncanonicalToLocal;

// using chemist::wavefunction::make_wavefunction;

// TEST_CASE("MP2 Dipole Approximation") {
//     pluginplay::ModuleManager mm;
//     nwchemex::load_modules(mm);

//     // Basic input
//     const auto name       = mokup::molecule::h2o;
//     const auto mol        = mokup::get_molecule(name);
//     const auto nelectrons = 10;
//     const auto aos        = mokup::get_bases(name, mokup::basis_set::sto3g);

//     // Make the electronic Hamiltonian
//     simde::type::chemical_system sys{mol, nelectrons};
//     const auto& [H] = mm.at("SystemHamiltonian").run_as<ham_pt>(sys);
//     simde::type::els_hamiltonian H_e(H);

//     // Compute SCF wavefunction w/ electronic Hamiltonian
//     auto& scf_wf_mod  = mm.at("SCF Driver");
//     const auto& [ref] = scf_wf_mod.run_as<simde::CanonicalReference>(H_e, aos);

//     // Unpack the canonical occupied orbitals, and converged Fock operator
//     const auto& occ   = ref.basis_set().occupied_orbitals();
//     const auto& f_hat = ref.basis_set().fock_operator();

//     // Localize occupied orbitals and make a local wavefunction
//     // TODO: Actually localize the orbitals
//     simde::type::derived_space lmos(occ);
//     simde::type::derived_space empty; // This gets filled in by local_mod
//     const auto local_ref = make_wavefunction(lmos, empty, f_hat);

//     // Use the localized wf to make an orbital-dependent LMO/PAO wavefunction
//     // N.B. This line bypasses contraction of PAOs to atoms
//     auto& local_mod = mm.at("LMO-PAO Wavefunction");
//     local_mod.change_submod("L(i->a)", mm.at("L(LMO->LMO) (prescreening)"));
//     const auto& [sparse_ref] = local_mod.run_as<local_pt>(local_ref);

//     // Unpack the oribtal-dependent spaces
//     const auto& lmos_i = sparse_ref.basis_set().occupied_orbitals();
//     const auto& paos_i = sparse_ref.basis_set().virtual_orbitals();

//     // Run dipoles w/ unpacked spaces
//     auto& dip_mod     = mm.at("MP2 Dipole");
//     const auto& [eij] = dip_mod.run_as<simde::SparsePairEnergy>(lmos_i, paos_i);
//     std::cout << eij << std::endl;
// }
