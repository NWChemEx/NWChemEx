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

using simde::type::chemical_system;
using ptype       = simde::SystemHamiltonian;
using molecule_pt = simde::MoleculeFromString;

TEST_CASE("System Hamiltonian") {
    pluginplay::ModuleManager mm;
    nwchemex::load_modules(mm);
    auto mod = mm.at("SystemHamiltonian");

    std::string name{"water"};
    auto mol = mm.at("NWX Molecules").run_as<molecule_pt>(name);

    simde::type::many_electrons es{10ul};
    simde::type::els_kinetic T(es);
    simde::type::els_nuc_coulomb V(es, mol.nuclei());
    simde::type::els_coulomb G(es);
    simde::type::nuc_coulomb Vnn(mol.nuclei());
    simde::type::hamiltonian H_corr(T, V, G, Vnn);

    chemical_system sys{mol, 10};
    auto H = mod.run_as<ptype>(sys);
    REQUIRE(H == H_corr);
}
