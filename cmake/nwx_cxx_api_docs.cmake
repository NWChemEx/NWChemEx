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

################################################################################
#
# The master version of this file lives at
# NWChemEx-Project/DeveloperTools/cmake/nwx_cxx_api_docs.cmake. The version
# contained in all other NWChemEx-Project repositories  is a synchronized copy
# of the master version. Any changes made to the non-master version will be lost
# the next time the master version is updated. To make changes please make them
# to the master version.
#
################################################################################

include_guard()

option(BUILD_DOCS "Should we build the documentation?" OFF)
option(ONLY_BUILD_DOCS
      "If enabled and BUILD_DOCS is true no libraries will be built" OFF
)

macro(nwx_cxx_api_docs)
    string(TOLOWER "${PROJECT_NAME}" _ncad_lc_name)
    set(_ncad_target "${_ncad_lc_name}_cxx_api")
    if("${BUILD_DOCS}")
        find_package(Doxygen REQUIRED)
        set(DOXYGEN_EXTRA_PACKAGES amsmath amsfonts)
        doxygen_add_docs("${_ncad_target}" ${ARGV})
        if("${ONLY_BUILD_DOCS}")
            return()
        endif()
    endif()
endmacro()
