#include "nwchemex/nwchemex.hpp"
#include <catch2/catch.hpp>
#include <mokup/mokup.hpp>

using molecule_type = simde::type::molecule;
using atom_type     = simde::type::atom;
using size_type     = typename atom_type::size_type;
using coord_type    = typename atom_type::coord_type;
using mass_type     = typename atom_type::mass_type;
using name_type     = typename atom_type::name_type;

using reference_pt = simde::CanonicalReference;
using energy_pt    = simde::TotalCanonicalEnergy;
using sys_H_pt     = simde::SystemHamiltonian;

TEST_CASE("Mock Procedure") {
    pluginplay::ModuleManager mm;
    nwchemex::load_modules(mm);

    // Build a molecule
    molecule_type mol;
    unsigned int nelectrons = 2;
    for(int i = 0; i < nelectrons; i++) {
        mol.push_back(atom_type(name_type{"H"}, coord_type{0.0, 0.0, float(i)},
                                mass_type{1.0}, size_type{1}));
    }

    // Basis sets
    auto aos     = nwchemex::apply_basis("cc-pvtz", mol);
    auto aux_aos = nwchemex::apply_basis("cc-pvtz-jkfit", mol);
    mm.change_input("DFJK", "Fitting Basis", aux_aos);
    mm.change_submod("Fock Matrix", "J Builder", "DFJK");
    mm.change_submod("Fock Matrix", "K Builder", "DFJK");

    // Build a Hamiltonians
    auto chem_sys = simde::type::chemical_system{mol, nelectrons};
    auto [H]      = mm.at("SystemHamiltonian").run_as<sys_H_pt>(chem_sys);
    auto H_e      = simde::type::els_hamiltonian{H};

    // Get wavefunction, then energy
    auto [phi0] = mm.at("SCF Driver").run_as<reference_pt>(H_e, aos);
    auto [E]    = mm.at("Total Energy").run_as<energy_pt>(phi0, H, phi0);
    std::cout << "Total SCF Energy: " << E << std::endl;
}