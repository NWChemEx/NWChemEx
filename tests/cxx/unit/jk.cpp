/*
 * Copyright 2022 NWChemEx-Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "nwchemex/nwchemex.hpp"
#include <catch2/catch.hpp>

using pt          = simde::AOEnergy;
using mol_bs_pt   = simde::MolecularBasisSet;
using sys_H_pt    = simde::SystemHamiltonian;
using j_pt        = simde::MeanFieldJ;
using k_pt        = simde::MeanFieldK;
using ref_dens_pt = simde::SCFGuessDensity;
using sad_rho_pt  = simde::InitialDensity; // H_e -> rho
using j_type      = simde::type::el_scf_j;
using k_type      = simde::type::el_scf_k;
using molecule_pt = simde::MoleculeFromString;

TEST_CASE("JK mixed") {
    pluginplay::ModuleManager mm;
    nwchemex::load_modules(mm);
    mm.change_submod("CanK", "ERI Builder", "ERI4");

    auto sys_H_mod   = mm.at("SystemHamiltonian");
    auto density_mod = mm.at("SCF Density Driver");
    auto canj_mod    = mm.at("CanJ");
    auto cank_mod    = mm.at("CanK");
    auto canjk_mod   = mm.at("CanJK");

    auto sad_mod_0 = mm.at("SADDensity");
    sad_mod_0.change_submod("Atomic Density", mm.at("6-31g atomic dm"));

    // Grab molecule and build a basis set
    std::string name{"water"};
    auto mol = mm.at("NWX Molecules").run_as<molecule_pt>(name);

    auto bs0 = mm.at("6-31g").run_as<mol_bs_pt>(mol);
    auto bs1 = mm.at("sto-3g").run_as<mol_bs_pt>(mol);

    simde::type::ao_space aos0(bs0);
    simde::type::ao_space aos1(bs1);

    simde::type::chemical_system chem_sys(mol);

    auto H = sys_H_mod.run_as<sys_H_pt>(chem_sys);
    simde::type::els_hamiltonian H_e(H);

    // get a density in aos0

    auto rho0 = sad_mod_0.run_as<sad_rho_pt>(H_e);

    simde::type::el e;
    simde::type::el_scf_j j0(e, rho0);
    simde::type::el_scf_k k0(e, rho0);

    // build J/K in aos1 from rho0
    simde::type::tensor J01_canjk, K01_canjk;
    auto inputs = canjk_mod.inputs();
    inputs.at("bra").change(aos1);
    inputs.at("ket").change(aos1);
    inputs.at("(r₁₂)⁻¹").change(j0);
    inputs.at("k").change(k0);

    std::cout << "J01 from CanJK: ";
    auto t0             = std::chrono::high_resolution_clock::now();
    std::tie(J01_canjk) = j_pt::unwrap_results(canjk_mod.run(inputs));
    auto t1             = std::chrono::high_resolution_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    std::cout << dt.count() << " ms\n";

    std::cout << "K01 from CanJK: ";
    t0                  = std::chrono::high_resolution_clock::now();
    std::tie(K01_canjk) = k_pt::unwrap_results(canjk_mod.run(inputs));
    t1                  = std::chrono::high_resolution_clock::now();
    dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    std::cout << dt.count() << " ms\n";

    std::cout << "J01 from CanJ: ";
    t0            = std::chrono::high_resolution_clock::now();
    auto J01_canj = canj_mod.run_as<j_pt>(aos1, j0, aos1);
    t1            = std::chrono::high_resolution_clock::now();
    dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    std::cout << dt.count() << " ms\n";

    std::cout << "K01 from CanK: ";
    t0            = std::chrono::high_resolution_clock::now();
    auto K01_cank = cank_mod.run_as<k_pt>(aos1, k0, aos1);
    t1            = std::chrono::high_resolution_clock::now();
    dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    std::cout << dt.count() << " ms\n";

    REQUIRE(tensorwrapper::tensor::allclose(J01_canjk, J01_canj));
    REQUIRE(tensorwrapper::tensor::allclose(K01_canjk, K01_cank));

    // build J/K in aos0 from rho0
    simde::type::tensor J00_canjk, K00_canjk;
    inputs.at("bra").change(aos0);
    inputs.at("ket").change(aos0);
    inputs.at("(r₁₂)⁻¹").change(j0);
    inputs.at("k").change(k0);

    std::cout << "J00 from CanJK: ";
    t0                  = std::chrono::high_resolution_clock::now();
    std::tie(J00_canjk) = j_pt::unwrap_results(canjk_mod.run(inputs));
    t1                  = std::chrono::high_resolution_clock::now();
    dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    std::cout << dt.count() << " ms\n";

    std::cout << "K00 from CanJK: ";
    t0                  = std::chrono::high_resolution_clock::now();
    std::tie(K00_canjk) = k_pt::unwrap_results(canjk_mod.run(inputs));
    t1                  = std::chrono::high_resolution_clock::now();
    dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    std::cout << dt.count() << " ms\n";

    std::cout << "J00 from CanJ: ";
    t0            = std::chrono::high_resolution_clock::now();
    auto J00_canj = canj_mod.run_as<j_pt>(aos0, j0, aos0);
    t1            = std::chrono::high_resolution_clock::now();
    dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    std::cout << dt.count() << " ms\n";

    std::cout << "K00 from CanK: ";
    t0            = std::chrono::high_resolution_clock::now();
    auto K00_cank = cank_mod.run_as<k_pt>(aos0, k0, aos0);
    t1            = std::chrono::high_resolution_clock::now();
    dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    std::cout << dt.count() << " ms\n";

    REQUIRE(tensorwrapper::tensor::allclose(J00_canjk, J00_canj));
    REQUIRE(tensorwrapper::tensor::allclose(K00_canjk, K00_cank));
}

TEST_CASE("DFJK mixed") {
    auto t0 = std::chrono::high_resolution_clock::now();
    auto t1 = std::chrono::high_resolution_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    pluginplay::ModuleManager mm;
    nwchemex::load_modules(mm);
    mm.change_submod("DFK", "ERI Builder", "Transformed ERI3");

    auto sys_H_mod   = mm.at("SystemHamiltonian");
    auto density_mod = mm.at("SCF Density Driver");

    auto sad_mod_0 = mm.at("SADDensity");
    sad_mod_0.change_submod("Atomic Density", mm.at("6-31g atomic dm"));

    // Grab molecule and build a basis set
    std::string name{"water"};
    auto mol = mm.at("NWX Molecules").run_as<molecule_pt>(name);

    auto bs0    = mm.at("6-31g").run_as<mol_bs_pt>(mol);
    auto bs1    = mm.at("sto-3g").run_as<mol_bs_pt>(mol);
    auto aux_bs = mm.at("cc-pvtz-jkfit").run_as<mol_bs_pt>(mol);

    simde::type::ao_space aos0(bs0);
    simde::type::ao_space aos1(bs1);
    simde::type::ao_space aux_aos(aux_bs);
    mm.change_input("DFJK", "Fitting Basis", aux_aos);
    mm.change_input("DFJ", "Fitting Basis", aux_aos);
    mm.change_input("DFK", "Fitting Basis", aux_aos);

    auto dfj_mod  = mm.at("DFJ");
    auto dfk_mod  = mm.at("DFK");
    auto dfjk_mod = mm.at("DFJK");

    simde::type::chemical_system chem_sys(mol);

    auto H = sys_H_mod.run_as<sys_H_pt>(chem_sys);
    simde::type::els_hamiltonian H_e(H);

    // get a density in aos0
    auto rho0 = sad_mod_0.run_as<sad_rho_pt>(H_e);

    simde::type::el e;
    simde::type::el_scf_j j0(e, rho0);
    simde::type::el_scf_k k0(e, rho0);

    // build J/K in aos1 from rho0
    simde::type::tensor J01_dfjk, K01_dfjk;
    auto inputs = dfjk_mod.inputs();
    inputs.at("bra").change(aos1);
    inputs.at("ket").change(aos1);
    inputs.at("(r₁₂)⁻¹").change(j0);
    inputs.at("k").change(k0);
    inputs.at("Fitting Basis").change(aux_aos);

    // J01 from DFJK
    std::tie(J01_dfjk) = j_pt::unwrap_results(dfjk_mod.run(inputs));
    // K01 from DFJK
    std::tie(K01_dfjk) = k_pt::unwrap_results(dfjk_mod.run(inputs));
    // J01 from DFJ
    auto J01_dfj = dfj_mod.run_as<j_pt>(aos1, j0, aos1);
    // K01 from DFK
    auto K01_dfk = dfk_mod.run_as<k_pt>(aos1, k0, aos1);

    REQUIRE(tensorwrapper::tensor::allclose(J01_dfjk, J01_dfj));
    REQUIRE(tensorwrapper::tensor::allclose(K01_dfjk, K01_dfk));

    // build J/K in aos0 from rho0
    simde::type::tensor J00_dfjk, K00_dfjk;
    inputs.at("bra").change(aos0);
    inputs.at("ket").change(aos0);
    inputs.at("(r₁₂)⁻¹").change(j0);
    inputs.at("k").change(k0);

    // J00 from DFJK
    std::tie(J00_dfjk) = j_pt::unwrap_results(dfjk_mod.run(inputs));
    // K00 from DFJK
    std::tie(K00_dfjk) = k_pt::unwrap_results(dfjk_mod.run(inputs));
    // J00 from DFJ
    auto J00_dfj = dfj_mod.run_as<j_pt>(aos0, j0, aos0);
    // K00 from DFK
    auto K00_dfk = dfk_mod.run_as<k_pt>(aos0, k0, aos0);

    REQUIRE(tensorwrapper::tensor::allclose(J00_dfjk, J00_dfj));
    REQUIRE(tensorwrapper::tensor::allclose(K00_dfjk, K00_dfk));

    // TODO: this will fail unless eri_mod memoization is fixed
    // DFJK turns on memoization if density and output space use same basis
    // above, eri_mod was run with 6-31g for bra/ket/jk_basis, so that result
    // will be returned if the module is run again with memoization turned on

    // auto sad_mod_1 = sad_mod_0.unlocked_copy();
    // sad_mod_1.change_submod("Atomic Density", mm.at("sto-3g atomic dm"));
    // auto rho1 = sad_mod_1.run_as<sad_rho_pt>(H_e);
    // simde::type::el_scf_j j1(e, rho1);
    // simde::type::el_scf_k k1(e, rho1);
    // // build J/K in aos1 from rho1
    // simde::type::tensor J11_dfjk, K11_dfjk;
    // inputs.at("bra").change(aos1);
    // inputs.at("ket").change(aos1);
    // inputs.at("(r₁₂)⁻¹").change(j1);
    // inputs.at("k").change(k1);

    // // J11 from DFJK
    // std::tie(J11_dfjk) = j_pt::unwrap_results(dfjk_mod.run(inputs));
    // // K11 from DFJK
    // std::tie(K11_dfjk) = k_pt::unwrap_results(dfjk_mod.run(inputs));
    // // J11 from DFJ
    // auto J11_dfj = dfj_mod.run_as<j_pt>(aos1, j1, aos1);
    // // K11 from DFK
    // auto K11_dfk = dfk_mod.run_as<k_pt>(aos1, k1, aos1);

    // REQUIRE(tensorwrapper::tensor::allclose(J11_dfjk, J11_dfj));
    // REQUIRE(tensorwrapper::tensor::allclose(K11_dfjk, K11_dfk));
