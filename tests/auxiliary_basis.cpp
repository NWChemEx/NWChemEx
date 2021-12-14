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
    std::cout << "here" << std::endl;

    SECTION("JK Fit") {
        auto mod         = mm.at("AuxiliaryBasis");
        auto [aux_space] = mod.run_as<ptype>(simde::type::ao_space{tz});
        REQUIRE(aux_space == tz_jkfit);
    }

    SECTION("RI Fit") {
        auto mod = mm.at("AuxiliaryBasis");
        mod.change_input("Aux Basis Suffix", "-rifit");
        auto [aux_space] = mod.run_as<ptype>(simde::type::ao_space{tz});
        REQUIRE(aux_space == tz_rifit);
    }
}