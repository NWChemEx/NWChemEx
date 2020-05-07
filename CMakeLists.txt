cmake_minimum_required(VERSION 3.14)
project(nwchemex VERSION 1.0.0)

set(
        CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake"
        CACHE STRING "" FORCE
)

include(get_cpp)

### Options ###
option(BUILD_TESTING "Should we build the tests?" OFF)
option(BUILD_PYBINDINGS "Should we build Python3 bindings?" ON)

cpp_find_or_build_dependency(
        scf
        URL github.com/NWChemEx-Project/SCF
        PRIVATE TRUE
        BUILD_TARGET scf
        FIND_TARGET nwx::scf
        CMAKE_ARGS BUILD_TESTING=OFF
)

cpp_find_or_build_dependency(
        integrals
        URL github.com/NWChemEx-Project/Integrals
        PRIVATE TRUE
        BUILD_TARGET integrals
        FIND_TARGET nwx::integrals
        CMAKE_ARGS BUILD_TESTING=OFF
)

cpp_add_library(
        nwchemex
        SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/nwchemex"
        INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/nwchemex"
        DEPENDS nwx::scf nwx::integrals
)

if("${BUILD_TESTING}")
    cpp_find_or_build_dependency(
            Catch2
            URL github.com/catchorg/Catch2
            BUILD_TARGET Catch2
            FIND_TARGET Catch2::Catch2
    )
    cpp_add_tests(
            test_nwchemex
            SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/tests"
            INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/nwchemex"
            DEPENDS Catch2::Catch2 nwchemex
    )
endif()
