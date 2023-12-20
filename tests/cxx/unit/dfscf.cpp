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

using pt        = simde::AOEnergy;
using mol_bs_pt = simde::MolecularBasisSet;

TEST_CASE("DF-SCF") {
    pluginplay::ModuleManager mm;
    nwchemex::load_modules(mm);

    // Grab molecule and build basis sets
    simde::type::atom H1{"H", 1ul, 0.0, 0.0, 0.0, 0.0};
    simde::type::atom H2{"H", 1ul, 0.0, 0.0, 0.0, 1.6818473865225443};
    simde::type::molecule mol{H1, H2};
    auto bs     = mm.at("sto-3g").run_as<mol_bs_pt>(mol);
    auto aux_bs = mm.at("sto-3g").run_as<mol_bs_pt>(mol);

    simde::type::ao_space aos(bs);
    simde::type::ao_space aux_aos(aux_bs);
    simde::type::chemical_system chem_sys(mol);

    // Apply auxiliary set and change J and K build
    mm.change_input("DFJK", "Fitting Basis", aux_aos);
    mm.change_submod("Fock Matrix", "J Builder", "DFJK");
    mm.change_submod("Fock Matrix", "K Builder", "DFJK");

    // Calculate energy
    auto E = mm.at("SCF Energy").run_as<pt>(aos, chem_sys);
    std::cout << "Total DF-SCF/STO-3G Energy: " << E << std::endl;
    REQUIRE(E == Approx(-1.16282097647378).margin(1.0e-8));
}
