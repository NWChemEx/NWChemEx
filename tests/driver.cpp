//1. Has function for making molecule and AO basis set (change if needed)
#include "tests/test_common.hpp"
#include <catch2/catch.hpp>
#include <nwchemex/load_modules.hpp>
//2. Include the property type for the module you want to run
#include <property_types/aointegral.hpp>

//3. Change this to the property type you want to run as
using property_type = property_types::AOIntegral<2,double>;

TEST_CASE("Driving NWX from C++"){
    sde::ModuleManager mm;
    nwx::load_modules(mm);
    auto [mol, bs] = testing::make_molecule();
    //4. Change to the module key you want to run
    const auto key = "Overlap";
    //5. Call your module
    std::array bss{bs, bs};
    auto [S] = mm.run_as<property_type>(key, mol, bss, std::size_t{0});

}

