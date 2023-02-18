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

#include <chemcache/chemcache.hpp>
#include <chemist/chemist.hpp>

namespace nwchemex {

/**
 * @brief Convenience function for applying a basis set to a molecule
 *
 * @param[in] name The name of the basis set to apply
 * @param[in] mol The molecule instance to apply the basis set to.
 * @param[in] man The BasisSetManager instance to read the basis set from.
 *                Defaults to a default constructed variant.
 * @return The basis set resulting from applying it to @p mol.
 * @throw std::bad_alloc if there is insufficient memory to create the basis
 *                       set. Strong throw guarantee.
 */
inline auto apply_basis(
  const std::string& name, const chemist::Molecule& mol,
  bool cart_override                  = false,
  const chemist::BasisSetManager& man = chemcache::nwx_basis_set_manager(), ) {
    chemist::AOBasisSet<double> aos;

    for(const auto& ai : mol) {
        auto ci = man.get_basis(name, ai.Z());
        for(auto i : {0, 1, 2}) ci.coord(i) = ai.coords()[i];
        aos.add_center(ci);
    }

    if(!cart_override) return chemist::orbital_space::AOSpaceD(aos);

    for(auto& shell : aos.shells()) {
        shell.pure() = chemist::ShellType::cartesian;
    }

    return chemist::orbital_space::AOSpaceD(aos);
}

} // namespace nwchemex
