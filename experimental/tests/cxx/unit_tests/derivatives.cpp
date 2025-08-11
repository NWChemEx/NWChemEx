/*
 * Copyright 2023 NWChemEx-Project
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

#include "chemist/molecule/molecule.hpp"
#include "nwchemex/nwchemex.hpp"
#include "scf/property_types/derivative_types.hpp"
#include <catch2/catch.hpp>

using namespace chemist;
using pt       = simde::AOEnergyNuclearGradient;
using tensor_t = simde::type::tensor;

TEST_CASE("SCF Numerical Gradient") {
    auto mol               = Molecule();
    unsigned int nelectron = 2;
    for(int i = 0; i < nelectron; i++) {
        mol.push_back(Atom("H", 1ul, 1.0, 0.0, 0.0, float(i)));
    }

    pluginplay::ModuleManager mm;
    nwchemex::load_modules(mm);

    auto bs = mm.at("sto-3g").run_as<simde::MolecularBasisSet>(mol);

    simde::type::ao_space aos(bs);
    simde::type::chemical_system chem_sys(mol);
    auto& ng_mod = mm.at("SCF Numerical Gradient");
    auto grad    = ng_mod.run_as<pt>(aos, chem_sys, mol);

    tensor_t ref_grad{{0., 0., 0.365407}, {0., 0., -0.365407}};
    REQUIRE(tensorwrapper::tensor::allclose(grad, ref_grad));
}
