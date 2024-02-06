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
using molecule_pt = simde::MoleculeFromString;

TEST_CASE("SCF") {
    auto start = std::chrono::high_resolution_clock::now();

    pluginplay::ModuleManager mm;
    nwchemex::load_modules(mm);

    // Grab molecule and build a basis set
    std::string name{"water"};
    auto mol = mm.at("NWX Molecules").run_as<molecule_pt>(name);
    auto bs  = mm.at("sto-3g").run_as<mol_bs_pt>(mol);

    simde::type::ao_space aos(bs);
    simde::type::chemical_system chem_sys(mol);

    // Calculate energy
    auto E = mm.at("SCF Energy").run_as<pt>(aos, chem_sys);
    std::cout << "Total SCF/STO-3G Energy: " << E << std::endl;
    REQUIRE(E == Approx(-74.942080058072833).margin(1.0e-8));

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << "Time taken by SCF Test with Core Guess: " << duration.count()
              << " microseconds" << std::endl;
}

TEST_CASE("SCF with SAD guess") {
    auto start = std::chrono::high_resolution_clock::now();

    pluginplay::ModuleManager mm;
    nwchemex::load_modules(mm);

    // Grab molecule and build a basis set
    std::string name{"water"};
    auto mol = mm.at("NWX Molecules").run_as<molecule_pt>(name);
    auto bs  = mm.at("sto-3g").run_as<mol_bs_pt>(mol);

    simde::type::ao_space aos(bs);
    simde::type::chemical_system chem_sys(mol);

    // Use SAD guess
    mm.change_submod("SADDensity", "Atomic Density", "sto-3g atomic dm");
    mm.change_submod("SCF Wavefunction", "Guess", "SADGuess");

    // Calculate energy
    auto E = mm.at("SCF Energy").run_as<pt>(aos, chem_sys);
    std::cout << "Total SCF/STO-3G Energy: " << E << std::endl;
    REQUIRE(E == Approx(-74.942080058072833).margin(1.0e-8));

    auto stop = std::chrono::high_resolution_clock::now();

    auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << "Time taken by SCF Test with SAD Guess: " << duration.count()
              << " microseconds" << std::endl;
}
