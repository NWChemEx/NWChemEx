#include <catch2/catch.hpp>
#include <nwchemex/load_modules.hpp>
#include <libchemist/libchemist.hpp>
#include <property_types/correlation_energy.hpp>
#include <property_types/reference_wavefunction.hpp>

using canonical_mos  = property_types::type::canonical_mos<double>;
using orthogonal_mos = property_types::type::orthogonal_orbs<double>;
using orbitals       = property_types::type::orbitals<double>;
using scf_pt  = property_types::ReferenceWavefunction<double, canonical_mos>;
using mp2_pt  = property_types::CorrelationEnergy<double, orthogonal_mos>;

TEST_CASE("DLPNO-MP2"){
    sde::ModuleManager mm;
    nwx::load_modules(mm);
    libchemist::MoleculeManager mols;

    using libchemist::Atom;
    using coord_type = typename Atom::coord_type;
    Atom He1{2ul, coord_type{0.0, 0.0, 0.0}};
    Atom He2{2ul, coord_type{0.0, 0.0, 15.0}};
    libchemist::Molecule mol(He1, He2);
    auto bs  = libchemist::apply_basis("cc-pvdz", mol);
    const auto [E_SCF, C] = mm.run_as<scf_pt>("SCFDIIS", mol, bs);
    const auto [E_MP2] = mm.run_as<mp2_pt>("DLPNO", mol, bs, C);
    std::cout << E_MP2 << std::endl;
}
