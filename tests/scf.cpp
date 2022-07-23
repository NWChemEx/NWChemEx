#include "nwchemex/nwchemex.hpp"
#include <catch2/catch.hpp>
#include <mokup/mokup.hpp>

using pt = simde::AOEnergy;

TEST_CASE("SCF") {
    pluginplay::ModuleManager mm;
    nwchemex::load_modules(mm);

    // Grab molecule and build a basis set
    const auto name = mokup::molecule::h2o;
    auto mol        = mokup::get_molecule(name);
    auto aos        = nwchemex::apply_basis("sto-3g", mol);

#if 1
    simde::type::chemical_system chem_sys(mol);

    // Calculate energy
    auto [E] = mm.at("SCF Energy").run_as<pt>(aos, chem_sys);
    std::cout << "Total SCF/STO-3G Energy: " << E << std::endl;
    REQUIRE(E == Approx(-74.942080058072833).margin(1.0e-8));
#else

    auto H_full     = mokup::hamiltonian(name);
    auto H_e        = mokup::electronic_hamiltonian(name);

    auto& scf_mod = mm.at("SCFDIIS Driver");
    auto& ene_mod = mm.at("Total Energy");
    auto& ee_mod  = mm.at("Electronic Energy");
    using scf_pt = simde::CanonicalReference;
    using ene_pt = simde::TotalCanonicalEnergy;
    using ee_pt  = simde::CanonicalElectronicEnergy;


    auto [phi0] = scf_mod.run_as<scf_pt>(H_e, aos);
    auto [Ee]   = ee_mod.run_as<ee_pt>(phi0, H_e, phi0);
    auto [E]    = ene_mod.run_as<ene_pt>(phi0, H_full, phi0);

    std::cout << std::setprecision(10) << "HF Electronic Energy " << Ee << std::endl;
    std::cout << std::setprecision(10) << "Total HF Energy " << E << std::endl;

    mm.at("HDF5 QCSchema").run_as<simde::QCSchemaFactory>(H_e, phi0);
#endif

}
