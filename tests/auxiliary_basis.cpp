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
#include <mokup/mokup.hpp>

using ptype = simde::AuxiliaryBasisSet;

TEST_CASE("Auxiliary Basis") {
    pluginplay::ModuleManager mm;
    nwchemex::load_modules(mm);

    auto mol      = mokup::get_molecule(mokup::molecule::h2o);
    auto tz       = nwchemex::apply_basis("cc-pvtz", mol);
    auto tz_jkfit = nwchemex::apply_basis("cc-pvtz-jkfit", mol);
    auto tz_rifit = nwchemex::apply_basis("cc-pvtz-rifit", mol);

    SECTION("JK Fit") {
        auto mod         = mm.at("Standard JK Fitting Basis");
        auto [aux_space] = mod.run_as<ptype>(simde::type::ao_space{tz});
        REQUIRE(aux_space == tz_jkfit);
    }

    SECTION("RI Fit") {
        auto mod         = mm.at("Standard RI Fitting Basis");
        auto [aux_space] = mod.run_as<ptype>(simde::type::ao_space{tz});
        REQUIRE(aux_space == tz_rifit);
    }
}