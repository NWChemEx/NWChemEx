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

#Downloads common CMake modules used throughout NWChemEx
include(cmake/get_nwx_cmake.cmake)

#Sets the version to whatever git thinks it is
include(get_version_from_git)
get_version_from_git(simde_version "${CMAKE_CURRENT_LIST_DIR}")
project(nwchemex VERSION "${simde_version}" LANGUAGES CXX)

include(nwx_versions)
include(get_cmaize)
include(nwx_cxx_api_docs)

### Paths ###
set(NWCHEMEX_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(NWCHEMEX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(NWCHEMEX_TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")
set(NWCHEMEX_EXP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/experimental")

nwx_cxx_api_docs("${NWCHEMEX_SOURCE_DIR}" "${NWCHEMEX_INCLUDE_DIR}")

### Options ###
cmaize_option_list(
    BUILD_TESTING OFF "Should we build the tests?"
    BUILD_PYBIND11_PYBINDINGS ON "Use pybind11 to build Python3 bindings?"
    BUILD_FULL_CHEMCACHE ON "If ChemCache isn't found, build the full version"
)

if("${BUILD_FULL_CHEMCACHE}")
    set(NWX_CHEMCACHE_VERSION generated_data)
endif()

cmaize_find_or_build_dependency(
    chemcache
    URL github.com/NWChemEx/ChemCache
    VERSION ${NWX_CHEMCACHE_VERSION}
    BUILD_TARGET chemcache
    FIND_TARGET nwx::chemcache
    CMAKE_ARGS BUILD_TESTING=OFF
               BUILD_PYBIND11_PYBINDINGS="${BUILD_PYBIND11_PYBINDINGS}"
)

cmaize_find_or_build_dependency(
    friendzone
    URL github.com/NWChemEx/FriendZone
    VERSION ${NWX_FRIENDZONE_VERSION}
    BUILD_TARGET friendzone
    FIND_TARGET nwx::friendzone
    CMAKE_ARGS BUILD_TESTING=OFF
               BUILD_PYBIND11_PYBINDINGS="${BUILD_PYBIND11_PYBINDINGS}"
)

cmaize_add_library(
    nwchemex
    SOURCE_DIR "${NWCHEMEX_SOURCE_DIR}/nwchemex"
    INCLUDE_DIRS "${NWCHEMEX_INCLUDE_DIR}/nwchemex"
    DEPENDS chemcache friendzone
)

include(nwx_pybind11)
nwx_add_pybind11_module(
    ${PROJECT_NAME}
    SOURCE_DIR "${NWCHEMEX_SOURCE_DIR}/python"
    DEPENDS "${PROJECT_NAME}"
)

if("${BUILD_TESTING}")
    set(CXX_TEST_DIR "${NWCHEMEX_TEST_DIR}/cxx")
    set(PYTHON_TEST_DIR "${NWCHEMEX_TEST_DIR}/python")

    cmaize_find_or_build_dependency(
        Catch2
        URL github.com/catchorg/Catch2
        BUILD_TARGET Catch2
        VERSION ${NWX_CATCH2_VERSION}
        FIND_TARGET Catch2::Catch2
    )
    cmaize_add_tests(
       test_unit_${PROJECT_NAME}
       SOURCE_DIR "${CXX_TEST_DIR}/unit_tests"
       INCLUDE_DIRS "${NWCHEMEX_SOURCE_DIR}/${PROJECT_NAME}"
       DEPENDS Catch2 ${PROJECT_NAME}
    )
    nwx_pybind11_tests(
        py_nwchemex ${PYTHON_TEST_DIR}/unit_tests/test_nwchemex.py
        SUBMODULES parallelzone pluginplay chemist simde chemcache friendzone
    )
endif()

cmaize_add_package(${PROJECT_NAME} NAMESPACE nwx::)
