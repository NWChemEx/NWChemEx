// #include <catch2/catch.hpp>
// #include <libchemist/libchemist.hpp>
// #include <nwchemex/load_modules.hpp>
// #include <property_types/correlation_energy.hpp>
// #include <property_types/reference_wavefunction.hpp>
// #include <testing/testing.hpp>

// using canonical_mos = property_types::type::canonical_space_t<double>;
// using scf_pt = property_types::ReferenceWavefunction<double, canonical_mos>;
// using mp2_pt = property_types::CorrelationEnergy<double, canonical_mos>;

// TEST_CASE("Canonical MP2-F12") {
//     sde::ModuleManager mm;
//     nwx::load_modules(mm);

//     std::vector names{testing::molecule::h2, testing::molecule::h2o};
//     for(const auto& name : names) {
//         SECTION(testing::as_string(name)) {
//             auto mol       = testing::get_molecules().at(name);
//             const auto aos = libchemist::apply_basis("cc-pvdz", mol);
//             const auto aux = libchemist::apply_basis("cc-pvdz-f12_optri",
//             mol); mm.change_input("CABS", "F12 fitting basis", aux); const
//             auto [E_SCF, C] = mm.run_as<scf_pt>("SCFDIIS", mol, aos); const
//             auto [E_MP2] =
//               mm.run_as<mp2_pt>("Dense MP2-F12", mol, aos, C);
//             std::cout << "Correlation Energy: " << E_MP2 << std::endl;
//             std::cout << "MP2-F12 Energy: " << E_SCF + E_MP2 << std::endl;
//         }
//     }
// }
