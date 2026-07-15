cmake_minimum_required(VERSION 3.14)

# Downloads common CMake modules used throughout NWChemEx. Included before
# project() so nwx_set_version() is available to compute the version.
include(cmake/get_nwx_cmake.cmake)

# Version from scikit-build-core (wheels/sdists) or the latest git tag.
include(nwx_set_version)
nwx_set_version(nwchemex_version "${CMAKE_CURRENT_LIST_DIR}")

# NWChemEx has no C++ of its own -- this is a superbuild that fetches the
# rest of the stack and aggregates it behind a single INTERFACE target.
project(nwchemex VERSION "${nwchemex_version}" LANGUAGES NONE)

include(disable_in_source_builds)
include(set_default_nwx_options)

### Dependencies ###
include(get_dependencies)

# BUILD_TESTING is a global CACHE flag, so without this toggle each of
# integrals/chemcache/nux would also register (and, for CTest purposes,
# run) its own unit-test suite as a side effect of being fetched here --
# e.g. chemcache's own py_utils_test_chemcache, which needs test-only
# Python deps (requests) this build never installs. We only want the
# library targets, so build them with testing off, restored immediately
# after (same approach as libint2.cmake).
set(_nwchemex_bt_backup "${BUILD_TESTING}")
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
get_dependencies(integrals chemcache nux)
set(BUILD_TESTING "${_nwchemex_bt_backup}" CACHE BOOL "" FORCE)
unset(_nwchemex_bt_backup)

### "Library" (aggregator INTERFACE target) ###
if(NOT TARGET nwchemex)
    add_library(nwchemex INTERFACE)
    target_link_libraries(nwchemex INTERFACE ${GET_DEPENDENCIES_TARGETS})
endif()

include(install_target)
install_library(nwchemex "")
