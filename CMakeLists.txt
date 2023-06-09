# Copyright 2022 NWChemEx-Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required(VERSION 3.14)
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/version.txt" VERSION)
project(nwchemex VERSION "${VERSION}" LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    nwx_cmake
    GIT_REPOSITORY https://github.com/NWChemEx-Project/NWXCMake
)
FetchContent_MakeAvailable(nwx_cmake)
list(APPEND CMAKE_MODULE_PATH "${nwx_cmake_SOURCE_DIR}/cmake")

set(
    CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${PROJECT_SOURCE_DIR}/cmake"
    CACHE STRING "" FORCE
)

include(get_cmaize)
include(nwx_versions)
include(nwx_cxx_api_docs)

nwx_cxx_api_docs("${CMAKE_CURRENT_SOURCE_DIR}/src" 
                 "${CMAKE_CURRENT_SOURCE_DIR}/include")


### Options ###
option(BUILD_TESTING "Should we build the tests?" OFF)
option(BUILD_CPPYY_PYBINDINGS "Should we build Python3 bindings?" ON)

cmaize_find_or_build_dependency(
    integrals
    URL github.com/NWChemEx-Project/Integrals
    PRIVATE TRUE
    VERSION ${NWX_INTEGRALS_VERSION}
    BUILD_TARGET integrals
    FIND_TARGET nwx::integrals
    #VERSION gpu_end_to_end_driver
    CMAKE_ARGS BUILD_TESTING=OFF
)

cmaize_find_or_build_dependency(
    scf
    URL github.com/NWChemEx-Project/SCF
    PRIVATE TRUE
    VERSION ${NWX_SCF_VERSION}
    BUILD_TARGET scf
    FIND_TARGET nwx::scf
    #VERSION gpu_end_to_end_driver
    CMAKE_ARGS BUILD_TESTING=OFF
)

# Uncomment once MP2 is working again
# cmaize_find_or_build_dependency(
#         mp2
#         URL github.com/NWChemEx-Project/MP2
#         PRIVATE TRUE
#         VERSION ${NWX_MP2_VERSION}
#         BUILD_TARGET mp2
#         FIND_TARGET nwx::mp2
#         CMAKE_ARGS BUILD_TESTING=OFF
# )

cmaize_find_or_build_dependency(
    chemcache
    URL github.com/NWChemEx-Project/ChemCache
    PRIVATE TRUE
    VERSION ${NWX_CHEMCACHE_VERSION}
    BUILD_TARGET chemcache
    FIND_TARGET nwx::chemcache
    VERSION generated_data
    CMAKE_ARGS BUILD_TESTING=OFF
)

cmaize_add_library(
    nwchemex
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src/nwchemex"
    INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include/nwchemex"
    DEPENDS scf integrals chemcache # nwx::mp2
)

cmaize_add_executable(
        nwx_driver
        SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/end_to_end_driver"
        INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/end_to_end_driver"
        DEPENDS nwchemex
)

if("${BUILD_TESTING}")
    cmaize_find_or_build_dependency(
        Catch2
        URL github.com/catchorg/Catch2
        BUILD_TARGET Catch2
        VERSION ${NWX_CATCH2_VERSION}
        FIND_TARGET Catch2::Catch2
    )
    cmaize_find_or_build_dependency(
        mokup
        URL github.com/NWChemEx-Project/mokup
        PRIVATE TRUE
        VERSION ${NWX_MOKUP_VERSION}
        BUILD_TARGET mokup
        FIND_TARGET nwx::mokup
        CMAKE_ARGS BUILD_TESTING=OFF
    )
    cmaize_add_tests(
       test_nwchemex
       SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/tests"
       INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/nwchemex"
       DEPENDS Catch2 nwchemex mokup
    )
    if(BUILD_CPPYY_PYBINDINGS)
       set (python_tests
            test_python dlpno_mp2_python
           )
       foreach(py_test ${python_tests})
         add_test(NAME ${py_test}
                  COMMAND Python3::Interpreter
                          ${CMAKE_CURRENT_SOURCE_DIR}/tests/${py_test}.py
                 )
       endforeach()
       set_tests_properties(
           ${python_tests}
           PROPERTIES ENVIRONMENT "PYTHONPATH=${CMAKE_BINARY_DIR}/Python"
       )
    endif()
endif()

include(nwx_python_mods)
if("${BUILD_TESTING}")
    cppyy_make_python_package(PACKAGE nwchemex NAMESPACES nwchemex DEPPACKAGES integrals scf chemcache mokup ) # mp2
else()
    cppyy_make_python_package(PACKAGE nwchemex NAMESPACES nwchemex DEPPACKAGES integrals scf chemcache ) # mp2
endif()

cmaize_add_package(${PROJECT_NAME} NAMESPACE nwx::)
