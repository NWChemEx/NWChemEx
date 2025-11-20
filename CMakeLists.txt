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
#project(nwchemex VERSION "1.0" LANGUAGES CXX)
#Downloads common CMake modules used throughout NWChemEx
include(cmake/get_nwx_cmake.cmake)

#Sets the version to whatever git thinks it is
include(get_version_from_git)
get_version_from_git(nwchemex_version "${CMAKE_CURRENT_LIST_DIR}")
project(nwchemex VERSION "${nwchemex_version}" LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

# Load NWX org helper modules
include(nwx_versions)
include(get_cmaize)
include(nwx_cxx_api_docs)

# Actual build system starts here

### Paths ###
set(NWCHEMEX_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(NWCHEMEX_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(NWCHEMEX_TESTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests")

# C++ API docs
nwx_cxx_api_docs("${NWCHEMEX_SOURCE_DIR}" "${NWCHEMEX_INCLUDE_DIR}")

### Options ###
cmaize_option_list(
    BUILD_TESTING OFF "Should we build the tests?"
    BUILD_FULL_CHEMCACHE ON "If ChemCache isn't found, build the full version"
    BUILD_PYBIND11_PYBINDINGS ON "Build Python bindings using Pybind11?"
)

### Plugins ###

if("${BUILD_FULL_CHEMCACHE}")
    set(NWX_CHEMCACHE_VERSION generated_data)
endif()

cmaize_find_or_build_optional_dependency(
    friendzone
    BUILD_PYBIND11_PYBINDINGS
    URL github.com/NWChemEx/FriendZone
    VERSION ${NWX_FRIENDZONE_VERSION}
    BUILD_TARGET friendzone
    FIND_TARGET nwx::friendzone
    CMAKE_ARGS BUILD_TESTING=OFF
               BUILD_PYBIND11_PYBINDINGS=${BUILD_PYBIND11_PYBINDINGS}
)

if(NOT TARGET scf) # Circular dependency band-aid
cmaize_find_or_build_dependency(
    scf
    URL github.com/NWChemEx/SCF
    VERSION ${NWX_SCF_VERSION}
    BUILD_TARGET scf
    FIND_TARGET nwx::scf
    CMAKE_ARGS BUILD_TESTING=OFF
               BUILD_PYBIND11_PYBINDINGS=${BUILD_PYBIND11_PYBINDINGS}
)
endif()

if(NOT TARGET nux) # Circular dependency band-aid
cmaize_find_or_build_dependency(
    nux
    URL github.com/NWChemEx/NUX
    VERSION master
    BUILD_TARGET nux
    FIND_TARGET nwx::nux
    CMAKE_ARGS BUILD_TESTING=OFF
               BUILD_PYBIND11_PYBINDINGS=${BUILD_PYBIND11_PYBINDINGS}
)
endif()
include(get_chemcache)
include(get_integrals)

#TOOD: Replace with cmaize_add_library when it supports Python
add_library(${PROJECT_NAME} INTERFACE)
target_link_libraries(
    ${PROJECT_NAME} INTERFACE chemcache friendzone integrals scf nux
)

if("${BUILD_TESTING}" AND "${BUILD_PYBIND11_PYBINDINGS}")
    include(CTest)
    include(nwx_pybind11)
    set(PYTHON_TEST_DIR "${NWCHEMEX_TESTS_DIR}/python")

    nwx_pybind11_tests(
        py_${PROJECT_NAME} ${PYTHON_TEST_DIR}/unit_tests/test_nwchemex.py
        SUBMODULES parallelzone pluginplay chemist simde chemcache friendzone
                   integrals scf nux tensorwrapper
    )

    # Workaround for the FriendZone being pure python (and thus not in the build
    # dir)
    get_test_property(
        py_${PROJECT_NAME}
        ENVIRONMENT py_path
    )
    set(
        py_path "${py_path}:${CMAKE_BINARY_DIR}/_deps/friendzone-src/src/python"
    )
    set_tests_properties(
        py_${PROJECT_NAME}
        PROPERTIES ENVIRONMENT "${py_path}"
    )
endif()

install(
    DIRECTORY "${NWCHEMEX_SOURCE_DIR}/python/nwchemex"
    DESTINATION "${NWX_MODULE_DIRECTORY}"
)
