cmake_minimum_required(VERSION 3.14)

# Downloads common CMake modules used throughout NWChemEx. Included before
# project() so nwx_set_version() is available to compute the version.
include(cmake/get_nwx_cmake.cmake)

# Version from scikit-build-core (wheels/sdists) or the latest git tag.
include(nwx_set_version)
nwx_set_version(nwchemex_version "${CMAKE_CURRENT_LIST_DIR}")

# NWChemEx has no C++ of its own -- this is a superbuild that fetches the
# rest of the stack and aggregates it behind a single INTERFACE target.
#
# This superbuild's dependency list (integrals, chemcache, nux, scf below)
# is deliberately NOT the same as pyproject.toml's. This file names only the
# direct C++ link closure -- the rest of the ecosystem (chemist, pluginplay,
# simde, parallelzone, ...) arrives transitively, since each fetched repo
# runs its own get_dependencies(). pyproject.toml instead lists every
# package python/nwchemex needs importable at runtime (simde and friendzone
# included), because that closure is a Python import graph, not a C++ link
# graph.
project(nwchemex VERSION "${nwchemex_version}" LANGUAGES NONE)

include(disable_in_source_builds)
include(set_default_nwx_options)

### Dependencies ###
include(get_dependencies)

get_dependencies(integrals chemcache nux scf)

### "Library" (aggregator INTERFACE target) ###
# GET_DEPENDENCIES_TARGETS (set by get_dependencies() above) is unusable
# here: every dependencies/*.cmake file's SKBUILD branch names a "nwx::..."
# ALIAS that is never actually defined anywhere in the ecosystem (get_dependencies
# always fetches via FetchContent, even under SKBUILD, so that branch is
# dead/wrong) -- linking against it fails at generate time with "the target
# was not found". NWX_DEP_TARGET_<name> is the mechanism every other
# consumer in the ecosystem actually relies on to resolve a dependency's
# real target name, so use that instead.
if(NOT TARGET nwchemex)
    add_library(nwchemex INTERFACE)
    target_link_libraries(nwchemex INTERFACE
        ${NWX_DEP_TARGET_integrals}
        ${NWX_DEP_TARGET_chemcache}
        ${NWX_DEP_TARGET_nux}
        ${NWX_DEP_TARGET_scf}
    )
endif()

include(install_target)
install_library(nwchemex "")
