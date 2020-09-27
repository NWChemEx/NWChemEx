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
        set(DOXYGEN_EXTRA_PACKAGES amsmath)
        doxygen_add_docs("${_ncad_target}" ${ARGV})
        if("${ONLY_BUILD_DOCS}")
            return()
        endif()
    endif()
endmacro()
