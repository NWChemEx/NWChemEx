#include <catch2/catch.hpp>
#include <libchemist/libchemist.hpp>
#include <mp2/mp2.hpp>
#include <nwchemex/load_modules.hpp>
#include <property_types/correlation_energy.hpp>
#include <property_types/reference_wavefunction.hpp>
#include <testing/testing.hpp>

using canonical_mos  = property_types::type::canonical_space_t<double>;
using orthogonal_mos = property_types::type::derived_space_t<double>;
using orbitals       = property_types::type::orbital_space_t<double>;
using scf_pt  = property_types::ReferenceWavefunction<double, canonical_mos>;
using mp2_pt  = mp2::pt::canonical_correlation_energy<double>;
using orb_map = property_types::type::orbital_map<canonical_mos>;

TEST_CASE("DLPNO-MP2") {
    sde::ModuleManager mm;
    nwx::load_modules(mm);
    libchemist::MoleculeManager mols;

    std::vector<std::string> names{"h2", "h2o"};
    for(const auto& name : names) {
        auto mol       = testing::get_molecules().at(name);
        const auto bs  = "sto-3g";
        const auto aos = testing::get_bases().at(name).at(bs);

        SECTION(name + "/" + bs) {
            const auto [E_SCF, C] = mm.run_as<scf_pt>("SCFDIIS", mol, aos);
            orb_map m{{"Occupied", C.at("Occupied")}};

            const auto [E_MP2] = mm.run_as<mp2_pt>("DLPNO-MP2", mol, aos, m);
            std::cout << E_MP2 << std::endl;
        }
    }
}
