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

// #include "nwchemex/nwchemex.hpp"
// #include <catch2/catch.hpp>
// #include <mokup/mokup.hpp>

// using pt = simde::AOEnergy;

// TEST_CASE("Canonical MP2-F12") {
//     pluginplay::ModuleManager mm;
//     nwchemex::load_modules(mm);

//     // Grab molecule and basis sets
//     const auto name   = mokup::molecule::h2;
//     const auto bs     = mokup::basis_set::ccpvdz;
//     const auto aux_bs = mokup::basis_set::ccpvdzf12optri;
//     auto mol          = mokup::get_molecule(name);
//     auto aos          = mokup::get_bases(name, bs);
//     const auto aux    = mokup::get_bases(name, aux_bs);

//     simde::type::chemical_system chem_sys(mol);

//     // Apply auxiliary set and change MP2 Energy module
//     mm.change_input("CABS", "F12 fitting basis", aux);
//     mm.change_submod("MP2 Correlation Energy", "Correlation Energy", 
//                      "Dense MP2-F12");

//     // Calculate energy
//     auto [E] = mm.at("MP2 Correlation Energy").run_as<pt>(aos, chem_sys);
//     std::cout << "MP2-F12/STO-3G Correlation Energy: " << E << std::endl;
//     REQUIRE(E == Approx(-0.034016042835582974).margin(1.0e-8));
// }
