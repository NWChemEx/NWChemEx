#include <catch2/catch.hpp>
#include <nwchemex/load_modules.hpp>
#include <libchemist/libchemist.hpp>
#include <property_types/reference_wavefunction.hpp>

using canonical_mos = property_types::type::canonical_mos<double>;
using pt_type = property_types::ReferenceWavefunction<double, canonical_mos>;

TEST_CASE("Direct SCF"){
    sde::ModuleManager mm;
    nwx::load_modules(mm);
    libchemist::MoleculeManager mols;

    auto mol = mols.at("water");
    auto bs  = libchemist::apply_basis("cc-pvdz", mol);
    const auto [E, C] = mm.run_as<pt_type>("SCFDIIS", mol, bs);

    std::cout << E << std::endl;
}
