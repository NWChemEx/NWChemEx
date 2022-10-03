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

// TEST_CASE("Canonical MP2") {
//     pluginplay::ModuleManager mm;
//     nwchemex::load_modules(mm);

//     // Grab molecule and build a basis set
//     const auto name = mokup::molecule::h2o;
//     auto mol        = mokup::get_molecule(name);
//     auto aos        = nwchemex::apply_basis("sto-3g", mol);

//     simde::type::chemical_system chem_sys(mol);

//     // Calculate energy
//     auto [E] = mm.at("MP2 Correlation Energy").run_as<pt>(aos, chem_sys);
//     std::cout << "MP2/STO-3G Correlation Energy: " << E << std::endl;
//     REQUIRE(E == Approx(-0.049150436516131206).margin(1.0e-8));

/// TODO: This is completely untested at this point. Needs checking once MP2 is
///       back up and running.
//     // Total Energy
//     auto [E] = mm.at("MP2 Energy").run_as<pt>(aos, chem_sys);
//     std::cout << "MP2/STO-3G Total Energy: " << E << std::endl;
//     REQUIRE(E == Approx(-74.99123049458896).margin(1.0e-8));
// }
