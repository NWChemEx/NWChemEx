# Copyright 2024 NWChemEx-Project
#
# Pre-fetch and patch dependencies that require old CMake versions
# This must be included BEFORE any find_package or cmaize calls for these deps

include_guard()

# ==============================================================================
# Patch libfort (requires cmake_minimum_required 3.0, incompatible with CMake 4+)
# ==============================================================================

message(STATUS "Pre-fetching libfort to patch it...")

include(FetchContent)

FetchContent_Declare(
    libfort
    GIT_REPOSITORY https://github.com/seleznevae/libfort.git
    GIT_TAG        v0.4.2
)

# Download only, don't configure yet
FetchContent_GetProperties(libfort)
if(NOT libfort_POPULATED)
    FetchContent_Populate(libfort)
    
    # Patch the CMakeLists.txt to use CMake 3.14 instead of 3.0
    file(READ "${libfort_SOURCE_DIR}/CMakeLists.txt" LIBFORT_CMAKE_CONTENT)
    string(REPLACE 
        "cmake_minimum_required(VERSION 3.0)" 
        "cmake_minimum_required(VERSION 3.14)"
        LIBFORT_CMAKE_CONTENT 
        "${LIBFORT_CMAKE_CONTENT}"
    )
    file(WRITE "${libfort_SOURCE_DIR}/CMakeLists.txt" "${LIBFORT_CMAKE_CONTENT}")
    message(STATUS "Patching libfort: CMake 3.0 -> 3.14")
    
    # Now add it to the build
    add_subdirectory(${libfort_SOURCE_DIR} ${libfort_BINARY_DIR})
    message(STATUS "libfort successfully patched")
endif()

message(STATUS "libfort is ready (pre-fetched, patched, and built)")

# ==============================================================================
# Detect and use system libxc if available (brew, system package manager, etc.)
# Only fetch and patch if not found
# ==============================================================================

# Add common homebrew paths to CMAKE_PREFIX_PATH if they exist
if(APPLE)
    if(EXISTS "/opt/homebrew")
        list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
    endif()
    if(EXISTS "/usr/local")
        list(APPEND CMAKE_PREFIX_PATH "/usr/local")
    endif()
endif()

# Try to find system libxc (from brew, apt, etc.)
find_package(Libxc 7.0 QUIET CONFIG)

if(Libxc_FOUND)
    message(STATUS "Found system libxc ${Libxc_VERSION}, skipping fetch and patch")
    # Make sure it's available to the rest of the build
    set(libxc_FOUND TRUE PARENT_SCOPE)
else()
    message(STATUS "System libxc not found, pre-fetching and patching...")
    
    FetchContent_Declare(
        libxc
        GIT_REPOSITORY https://gitlab.com/libxc/libxc.git
        GIT_TAG        7.0.0
    )

    # Download only, don't configure yet
    FetchContent_GetProperties(libxc)
    if(NOT libxc_POPULATED)
        FetchContent_Populate(libxc)
        
        # Patch the CMakeLists.txt to use CMake 3.14 instead of 3.0
        file(READ "${libxc_SOURCE_DIR}/CMakeLists.txt" LIBXC_CMAKE_CONTENT)
        string(REPLACE 
            "cmake_minimum_required(VERSION 3.0.2 FATAL_ERROR)" 
            "cmake_minimum_required(VERSION 3.14 FATAL_ERROR)"
            LIBXC_CMAKE_CONTENT 
            "${LIBXC_CMAKE_CONTENT}"
        )
        file(WRITE "${libxc_SOURCE_DIR}/CMakeLists.txt" "${LIBXC_CMAKE_CONTENT}")
        
        # Now add it to the build
        add_subdirectory(${libxc_SOURCE_DIR} ${libxc_BINARY_DIR})
    endif()
    
    message(STATUS "libxc is ready (pre-fetched, patched, and built)")
endif()
