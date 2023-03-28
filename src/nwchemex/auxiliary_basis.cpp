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
#include "nwchemex/apply_basis.hpp"
#include <simde/simde.hpp>

namespace nwchemex {

using simde::type::ao_basis_set;
using simde::type::ao_space;
using simde::type::atom;
using simde::type::molecule;

using ptype        = simde::AuxiliaryBasisSet;
using manager_type = chemist::BasisSetManager;

MODULE_CTOR(AuxiliaryBasis) {
    satisfies_property_type<ptype>();
    description("Produces an auxiliary basis set corresponding to the provided "
                "atomic basis set.");

    add_input<std::string>("Aux Basis Suffix").set_default("-jkfit");
}

MODULE_RUN(AuxiliaryBasis) {
    const auto& [basis_space] = ptype::unwrap_inputs(inputs);
    auto suffix = inputs.at("Aux Basis Suffix").value<std::string>();
    // TODO: Make this an additional input; basis set managers not hashable ATM
    auto manager = chemcache::nwx_basis_set_manager();

    // Use an AtomicBasisSet to make a single atom molecule instance.
    // Gets passed to apply_basis.
    auto atomic_sys = [](const chemist::AtomicBasisSet<double>& center) {
        return molecule{atom{"X", center.atomic_number(), 0.0, center.x(),
                             center.y(), center.z()}};
    };

    // convert atomic basis to auxiliary basis
    ao_basis_set aux_basis;
    for(auto& center : basis_space.basis_set()) {
        auto aux_name    = center.basis_set_name() + suffix;
        auto atom_as_mol = atomic_sys(center);

        auto aux_center = apply_basis(aux_name, atom_as_mol, manager);
        aux_basis       = aux_basis + aux_center.basis_set();
    }

    auto rv = results();
    return ptype::wrap_results(rv, ao_space{aux_basis});
}

} // namespace nwchemex