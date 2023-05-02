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

#include "modules.hpp"
#include <simde/simde.hpp>

namespace nwchemex {

using simde::type::els_coulomb;
using simde::type::els_kinetic;
using simde::type::els_nuc_coulomb;
using simde::type::hamiltonian;
using simde::type::many_electrons;
using simde::type::nuc_coulomb;

using ptype = simde::SystemHamiltonian;

MODULE_CTOR(SystemHamiltonian) {
    satisfies_property_type<ptype>();
    description("Makes a Hamiltonian operator from a Chemical System");
}

MODULE_RUN(SystemHamiltonian) {
    const auto& [sys] = ptype::unwrap_inputs(inputs);
    auto rv           = results();

    auto n_electrons   = many_electrons{sys.nelectrons()};
    const auto& nuclei = sys.molecule();

    hamiltonian H(nuc_coulomb{nuclei.nuclei()}, els_kinetic{n_electrons},
                  els_nuc_coulomb{n_electrons, nuclei.nuclei()},
                  els_coulomb{n_electrons});

    return ptype::wrap_results(rv, H);
}

} // namespace nwchemex
