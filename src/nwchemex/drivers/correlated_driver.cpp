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

using ao_energy_pt   = simde::AOEnergy;
using sys_H_pt       = simde::SystemHamiltonian;
using reference_pt   = simde::CanonicalReference;
using ref_energy_pt  = simde::TotalCanonicalEnergy;
using manybody_pt    = simde::CanonicalManyBodyWf;
using corr_energy_pt = simde::CanonicalCorrelationEnergy;

MODULE_CTOR(CorrelatedEnergyDriver) {
    satisfies_property_type<ao_energy_pt>();
    description("Calculates the correlation energy from a chemical system in a "
                "given basis set");

    add_submodule<sys_H_pt>("System Hamiltonian");
    add_submodule<reference_pt>("Reference Wave Function");
    add_submodule<ref_energy_pt>("Reference Energy");
    add_submodule<manybody_pt>("Many Body Wave Function");
    add_submodule<corr_energy_pt>("Correlation Energy");
}

MODULE_RUN(CorrelatedEnergyDriver) {
    const auto& [aos, chem_sys] = ao_energy_pt::unwrap_inputs(inputs);
    auto& hamiltonian_mod       = submods.at("System Hamiltonian");
    auto& reference_wf_mod      = submods.at("Reference Wave Function");
    auto& ref_energy_mod        = submods.at("Reference Energy");
    auto& many_body_wf_mod      = submods.at("Many Body Wave Function");
    auto& corr_energy_mod       = submods.at("Correlation Energy");

    auto H = hamiltonian_mod.run_as<sys_H_pt>(chem_sys);
    simde::type::els_hamiltonian H_e(H);

    auto ref_wf  = reference_wf_mod.run_as<reference_pt>(H_e, aos);
    auto ref_E   = ref_energy_mod.run_as<ref_energy_pt>(ref_wf, H, ref_wf);
    auto corr_wf = many_body_wf_mod.run_as<manybody_pt>(H_e, ref_wf);
    auto corr_E =
      corr_energy_mod.run_as<corr_energy_pt>(ref_wf, H_e, corr_wf);

    auto total_E = ref_E + corr_E;

    auto rv = results();
    return ao_energy_pt::wrap_results(rv, total_E);
}

} // namespace nwchemex
