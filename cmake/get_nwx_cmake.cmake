# Copyright 2024 NWChemEx Community
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

include_guard()

# Puts the shared NWXCMake modules on CMAKE_MODULE_PATH. This runs *before*
# project() so factored helpers (e.g. nwx_set_version) are available to feed
# project(VERSION ...).
#
# Resolution order:
#   1. A pip-installed `nwxcmake` package (query it for its cmake/ dir). An
#      editable install (`pip install -e /path/to/NWXCMake`) points at a local
#      working copy, so NWXCMake edits are picked up with no git push.
#   2. Fallback: FetchContent from github.com/ryanmrichard/NWXCMake.
macro(get_nwx_cmake)
    if(NOT _NWX_CMAKE_MODULE_DIR)
        # find_package(Python) needs no enabled language, so it is safe here,
        # before project(). Request only the interpreter to avoid a dev/compiler
        # probe this early.
        find_package(Python QUIET COMPONENTS Interpreter)

        set(_gnc_local_dir "")
        if(Python_Interpreter_FOUND)
            execute_process(
                COMMAND "${Python_EXECUTABLE}" -c
                    "import nwxcmake,sys;sys.stdout.write(nwxcmake.cmake_dir())"
                OUTPUT_VARIABLE _gnc_local_dir
                OUTPUT_STRIP_TRAILING_WHITESPACE
                RESULT_VARIABLE _gnc_rc
                ERROR_QUIET
            )
            if(NOT _gnc_rc EQUAL 0)
                set(_gnc_local_dir "")
            endif()
        endif()

        if(_gnc_local_dir AND IS_DIRECTORY "${_gnc_local_dir}")
            message(STATUS
                "NWXCMake: using local pip package at ${_gnc_local_dir}"
            )
            set(_gnc_resolved "${_gnc_local_dir}")
        else()
            message(STATUS
                "NWXCMake: pip package not found; fetching from git"
            )
            include(FetchContent)
            FetchContent_Declare(
                nwx_cmake
                GIT_REPOSITORY https://github.com/ryanmrichard/NWXCMake
            )
            FetchContent_MakeAvailable(nwx_cmake)
            set(_gnc_resolved "${nwx_cmake_SOURCE_DIR}/cmake")
        endif()

        # Cache the resolved directory (not CMAKE_MODULE_PATH itself) so
        # reconfigures are stable. Clear this var (or the build dir) to
        # re-resolve after switching between the pip and git sources.
        set(_NWX_CMAKE_MODULE_DIR "${_gnc_resolved}"
            CACHE INTERNAL "Resolved NWXCMake module directory"
        )
    endif()

    # Prepend so NWXCMake modules win; set in the caller's scope (macro), do not
    # FORCE-cache CMAKE_MODULE_PATH (avoids accumulating stale entries).
    list(PREPEND CMAKE_MODULE_PATH "${_NWX_CMAKE_MODULE_DIR}")
endmacro()

get_nwx_cmake()
