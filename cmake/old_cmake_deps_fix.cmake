# Copyright 2024 NWChemEx-Project
#
# Fix for old CMake minimum version requirements in dependencies
# 
# Both libfort and libxc require CMake 3.0 which is too old for modern CMake.
# This pre-fetches them, patches them, then lets CMaize use them.

include_guard()

include(FetchContent)

# Set policy to allow FetchContent_Populate (we need it to patch before configuring)
if(POLICY CMP0169)
    cmake_policy(SET CMP0169 OLD)
endif()

# Function to patch a library's CMakeLists.txt
function(patch_cmake_version lib_name source_dir binary_dir)
    set(LIB_CMAKE "${source_dir}/CMakeLists.txt")
    if(EXISTS "${LIB_CMAKE}")
        file(READ "${LIB_CMAKE}" CONTENT)
        if(CONTENT MATCHES "cmake_minimum_required\\(VERSION 3\\.0\\)")
            message(STATUS "Patching ${lib_name}: CMake 3.0 -> 3.14")
            string(REPLACE 
                "cmake_minimum_required(VERSION 3.0)"
                "cmake_minimum_required(VERSION 3.14)"
                CONTENT_PATCHED
                "${CONTENT}"
            )
            file(WRITE "${LIB_CMAKE}" "${CONTENT_PATCHED}")
            message(STATUS "${lib_name} successfully patched")
        endif()
    endif()
    
    # Build it by adding the subdirectory
    add_subdirectory(${source_dir} ${binary_dir})
    message(STATUS "${lib_name} is ready (pre-fetched, patched, and built)")
endfunction()

# Declare and populate libfort ourselves BEFORE CMaize tries to
message(STATUS "Pre-fetching libfort to patch it...")

FetchContent_Declare(
    libfort
    GIT_REPOSITORY https://github.com/seleznevae/libfort
    GIT_TAG        master
)

# Only populate (download), don't make available yet
FetchContent_GetProperties(libfort)
if(NOT libfort_POPULATED)
    FetchContent_Populate(libfort)
    patch_cmake_version("libfort" ${libfort_SOURCE_DIR} ${libfort_BINARY_DIR})
    set(libfort_POPULATED TRUE CACHE INTERNAL "")
endif()

# Declare and populate libxc ourselves BEFORE CMaize tries to
message(STATUS "Pre-fetching libxc to patch it...")

FetchContent_Declare(
    libxc
    GIT_REPOSITORY https://gitlab.com/libxc/libxc
    GIT_TAG        devel
)

FetchContent_GetProperties(libxc)
if(NOT libxc_POPULATED)
    FetchContent_Populate(libxc)
    patch_cmake_version("libxc" ${libxc_SOURCE_DIR} ${libxc_BINARY_DIR})
    set(libxc_POPULATED TRUE CACHE INTERNAL "")
endif()
