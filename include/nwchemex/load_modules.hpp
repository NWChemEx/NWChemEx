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

#pragma once
#include <pluginplay/pluginplay.hpp>
#include <simde/simde.hpp>

namespace nwchemex {

/** @brief Convenience function for loading all of NWChemEx's modules into an
 *         existing ModuleManager instance.
 *
 *  @param[in] mm The ModuleManager instance to load the modules into.
 *
 *  @throw std::bad_alloc if there is insufficient memory to create the new
 *                        modules. Weak throw guarantee.
 */
void load_modules(pluginplay::ModuleManager& mm);

} // namespace nwchemex

namespace simde {

DECLARE_PROPERTY_TYPE(QCSchemaFactory);
PROPERTY_TYPE_INPUTS(QCSchemaFactory) {
    using simde::type::canonical_reference;
    using simde::type::els_hamiltonian;
    auto rv =
      pluginplay::declare_input()
        .add_field<const els_hamiltonian&>("Electronic Hamiltonian")
        .add_field<const canonical_reference&>("Reference Wavefunction");
    return rv;
}

PROPERTY_TYPE_RESULTS(QCSchemaFactory) { return pluginplay::declare_result(); }

DECLARE_MODULE(HDF5QCSchema);

} // namespace simde
