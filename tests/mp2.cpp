#include <catch2/catch.hpp>
#include <nwchemex/load_modules.hpp>
#include <libchemist/libchemist.hpp>
#include <property_types/correlation_energy.hpp>
#include <property_types/reference_wavefunction.hpp>

using canonical_mos = property_types::type::canonical_mos<double>;
using scf_pt = property_types::ReferenceWavefunction<double, canonical_mos>;
using mp2_pt = property_types::CorrelationEnergy<double, canonical_mos>;

TEST_CASE("Canonical MP2"){
    sde::ModuleManager mm;
    nwx::load_modules(mm);
    libchemist::MoleculeManager mols;

    auto mol = mols.at("water");
    auto bs  = libchemist::apply_basis("cc-pvdz", mol);
    const auto [E_SCF, C] = mm.run_as<scf_pt>("SCFDIIS", mol, bs);
    const auto [E_MP2] = mm.run_as<mp2_pt>("MP2", mol, bs, C);
    std::cout << E_MP2 << std::endl;
}
