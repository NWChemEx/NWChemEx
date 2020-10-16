cmake_minimum_required(VERSION 3.14)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION)
project(nwchemex VERSION "${VERSION}" LANGUAGES CXX)

set(
        CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake"
        CACHE STRING "" FORCE
)

include(get_cpp)
include(nwx_cxx_api_docs)
nwx_cxx_api_docs("${CMAKE_CURRENT_SOURCE_DIR}/nwchemex")


### Options ###
option(BUILD_TESTING "Should we build the tests?" OFF)
option(BUILD_PYBINDINGS "Should we build Python3 bindings?" ON)

cpp_find_or_build_dependency(
        integrals
        URL github.com/NWChemEx-Project/Integrals
        PRIVATE TRUE
        BUILD_TARGET integrals
        FIND_TARGET nwx::integrals
        CMAKE_ARGS BUILD_TESTING=OFF
)

cpp_find_or_build_dependency(
        scf
        URL github.com/NWChemEx-Project/SCF
        PRIVATE TRUE
        BUILD_TARGET scf
        FIND_TARGET nwx::scf
        CMAKE_ARGS BUILD_TESTING=OFF
)

cpp_find_or_build_dependency(
        mp2
        URL github.com/NWChemEx-Project/MP2
        PRIVATE TRUE
        BUILD_TARGET mp2
        FIND_TARGET nwx::mp2
        CMAKE_ARGS BUILD_TESTING=OFF
)


cpp_add_library(
        nwchemex
        SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/nwchemex"
        INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/nwchemex"
        DEPENDS nwx::mp2 nwx::scf nwx::integrals
)

if(BUILD_PYBINDINGS)
    find_package(Python3 REQUIRED)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
    find_package(Cppyy REQUIRED)
    cppyy_make_python_package(nwchemex NAMESPACE nwchemex PREFIX nwchemex)
endif()

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
    if(BUILD_PYBINDINGS)
       add_test(
           NAME test_python
           COMMAND Python3::Interpreter
                   ${CMAKE_CURRENT_SOURCE_DIR}tests//test_python.py
       )
       set_tests_properties(
           test_python 
           PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}"
       )
    endif()
endif()
