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

#include "driver_modules.hpp"
#include <simde/simde.hpp>

namespace nwchemex {

using ao_energy_pt = simde::AOEnergy;
using sys_H_pt     = simde::SystemHamiltonian;
using ref_dens_pt  = simde::SCFGuessDensity;
using energy_pt    = simde::OneEDensityTotalEnergy;

MODULE_CTOR(ReferenceEnergyDensityDriver) {
    satisfies_property_type<ao_energy_pt>();
    description("Calculates the reference energy from a chemical system in a "
                "given basis set");

    add_submodule<sys_H_pt>("System Hamiltonian");
    add_submodule<ref_dens_pt>("Reference Density");
    add_submodule<energy_pt>("Reference Energy");
}

MODULE_RUN(ReferenceEnergyDensityDriver) {
    const auto& [aos, chem_sys] = ao_energy_pt::unwrap_inputs(inputs);
    auto& hamiltonian_mod       = submods.at("System Hamiltonian");
    auto& density_mod           = submods.at("Reference Density");
    auto& energy_mod            = submods.at("Reference Energy");

    auto H = hamiltonian_mod.run_as<sys_H_pt>(chem_sys);
    simde::type::els_hamiltonian H_e(H);

    auto rho0 = density_mod.run_as<ref_dens_pt>(H_e, aos);
    auto E    = energy_mod.run_as<energy_pt>(H, rho0);

    auto rv = results();
    return ao_energy_pt::wrap_results(rv, E);
}

} // namespace nwchemex
