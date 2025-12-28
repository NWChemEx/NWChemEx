# Copyright 2024 NWChemEx-Project
#
# Fix for libfort CMake minimum version requirement
# 
# libfort requires CMake 3.0 which is too old for modern CMake versions.
# This creates a patched version that we can use instead.
#
# Note: Uses variable-based guard instead of include_guard() so this can  
# check multiple times but only patch once.

# Only patch if we haven't already done so
if(NOT LIBFORT_PATCHED)
    # Patch libfort immediately when this file is included
    # This works because CMake processes files in order during configuration
    if(EXISTS "${CMAKE_BINARY_DIR}/_deps/libfort-src/CMakeLists.txt")
        set(LIBFORT_CMAKELISTS "${CMAKE_BINARY_DIR}/_deps/libfort-src/CMakeLists.txt")
        
        file(READ "${LIBFORT_CMAKELISTS}" LIBFORT_CMAKE_CONTENT)
        
        # Only patch if not already patched
        if(LIBFORT_CMAKE_CONTENT MATCHES "cmake_minimum_required\\(VERSION 3\\.0\\)")
            message(STATUS "Patching libfort CMakeLists.txt to require CMake 3.14")
            
            # Replace the cmake_minimum_required line
            string(REPLACE 
                "cmake_minimum_required(VERSION 3.0)"
                "cmake_minimum_required(VERSION 3.14)"
                LIBFORT_CMAKE_CONTENT_PATCHED
                "${LIBFORT_CMAKE_CONTENT}"
            )
            
            # Write the patched content back
            file(WRITE "${LIBFORT_CMAKELISTS}" "${LIBFORT_CMAKE_CONTENT_PATCHED}")
            
            message(STATUS "Successfully patched libfort CMakeLists.txt")
            set(LIBFORT_PATCHED TRUE CACHE INTERNAL "libfort has been patched")
        endif()
    endif()
endif()
