#include <catch2/catch.hpp>
#include <libchemist/libchemist.hpp>
#include <nwchemex/load_modules.hpp>
#include <property_types/correlation_energy.hpp>
#include <property_types/reference_wavefunction.hpp>

using canonical_mos  = property_types::type::canonical_space_t<double>;
using orthogonal_mos = property_types::type::derived_space_t<double>;
using orbitals       = property_types::type::orbital_space_t<double>;
using scf_pt = property_types::ReferenceWavefunction<double, canonical_mos>;
using mp2_pt = property_types::CorrelationEnergy<double, orthogonal_mos>;

TEST_CASE("DLPNO-MP2") {
    // sde::ModuleManager mm;
    // nwx::load_modules(mm);
    // libchemist::MoleculeManager mols;

    // auto mol = mols.at("water");
    // auto bs  = libchemist::apply_basis("cc-pvdz", mol);

    // const auto [E_SCF, C] = mm.run_as<scf_pt>("SCFDIIS", mol, bs);
    // property_types::type::orbital_map<orthogonal_mos> m{
    //   {"Occupied", C.at("Occupied")}};
    // const auto [E_MP2] = mm.run_as<mp2_pt>("DLPNO", mol, bs, m);
    // std::cout << E_MP2 << std::endl;
    // Canonical = -0.214348
}
