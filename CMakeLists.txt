cmake_minimum_required(VERSION 3.8)
project(NWCHEMEX VERSION 1.0.0)
set(CPP_NAMESPACE nwx)
find_package(CPP REQUIRED)

### Options ###
option(BUILD_TESTS "Should we build the tests?" OFF)
option(BUILD_PYBINDINGS "Should we build Python3 bindings?" ON)

### Dependencies ####
cpp_find_or_build_dependency(
    NAME sde
    URL github.com/NWChemEx-Project/SDE
    PRIVATE
    CMAKE_ARGS BUILD_TESTS=OFF BUILD_PYBINDINGS=ON
)

cpp_find_or_build_dependency(
    NAME property_types
    URL github.com/NWChemEx-Project/PropertyTypes
    PRIVATE
    CMAKE_ARGS BUILD_TESTS=OFF
)

cpp_find_or_build_dependency(
        NAME integrals
        URL github.com/jboschen/integrals
        BRANCH bigger_tiling
        PRIVATE
        CMAKE_ARGS BUILD_TESTS=OFF
)

cpp_find_or_build_dependency(
        NAME scf
        URL github.com/ryanmrichard/scf
        BRANCH formatting
        PRIVATE
        CMAKE_ARGS BUILD_TESTS=OFF
)

### Target ###
add_subdirectory(NWChemEx)

### Testing ###
if(BUILD_TESTS_disableForNow)
    cpp_find_or_build_dependency(NAME Catch2 URL github.com/catchorg/Catch2)
    enable_testing()
    add_subdirectory(NWChemEx_Test)
endif()
