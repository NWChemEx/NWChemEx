#This CMake module ensures that your Python interpreter has the cppyy module
#installed. It also defines a function ``cppyy_make_python_package`` which will
#skim a CMake target and automatically generate a
find_package(Python3 REQUIRED)
execute_process(
    COMMAND Python3::Interpreter -c "import cppyy"
    RESULT_VARIABLE _fcppyy_result
    OUTPUT_VARIABLE _fcppyy_output
    ERROR_VARIABLE  _fcppyy_error
)

if(NOT "${_fcppyy_result}" STREQUAL "")
    set(Cppyy_FOUND FALSE)
endif()

# This function will skim a CMake target and create a file __init__.py that
# should be placed next to the shared library created by that target. This
# function assumes the target's:
#
# * public header files are in the ``PUBLIC_HEADER`` property
# * include paths are in the ``INTERFACE_INCLUDE_DIRECTORIES`` property
# * dependencies are targets and in the ``INTERFACE_LINK_LIBRAIRES`` property
#
#
# :param target: The target name we are creating bindings for. Must be a valid
#                target.
#
# :Additional Named Arguments:
#     * *NAMESPACE* - The C++ namespace that your bindings live in. Will be used
#       as the name of the Python module for the resulting package.
#     * *PREFIX* - The path relative to header include root. Typically the name
#       of the directory that the target was added in. Defaults to the directory
#       this function was called from (note it's just the directory,not the full
#       path).
#     * *OUTPUT_DIR* - The build-time directory where the resulting file should
#       be placed. By default assumed to be the binary directory with the PREFIX
#       appended to it.
function(cppyy_make_python_package _cmpp_target)
    #---------------------------------------------------------------------------
    #-------------------------Basic error-checking------------------------------
    #---------------------------------------------------------------------------
    if("${_cmpp_target}" STREQUAL "")
        message(FATAL_ERROR "Target name may not be empty.")
    endif()
    if(NOT TARGET ${_cmpp_target})
        message(FATAL_ERROR "${_cmpp_target} is not a target.")
    endif()

    #---------------------------------------------------------------------------
    #--------------------------Argument Parsing---------------------------------
    #---------------------------------------------------------------------------
    set(_cmpp_options NAMESPACE PREFIX OUTPUT_DIR TEST)
    cmake_parse_arguments(_cmpp "" "${_cmpp_options}" "" ${ARGN})
    if("${_cmpp_PREFIX}" STREQUAL "")
        get_filename_component(_cmpp_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)
        string(TOLOWER ${_cmpp_PREFIX} _cmpp_PREFIX)
    endif()
    if("${_cmpp_OUTPUT_DIR}" STREQUAL "")
        set(_cmpp_OUTPUT_DIR ${CMAKE_BINARY_DIR}/${_cmpp_PREFIX})
    endif()

    #---------------------------------------------------------------------------
    #------------Collect the information we need off the target-----------------
    #---------------------------------------------------------------------------
    #List of include directories, usually a generator
    get_target_property(
            _cmpp_inc_dir ${_cmpp_target} INTERFACE_INCLUDE_DIRECTORIES
    )
    message("incdirs ${_cmpp_inc_dir}")
    #List of libraries, usually a mix of targets and libraries
    get_target_property(_cmpp_depends ${_cmpp_target} INTERFACE_LINK_LIBRARIES)
    message("linklibs ${_cmpp_depends}")
    #Get each dependency's include directory (shouldn't have to recurse if the
    #the targets are set up correctly)
    foreach(_cmpp_depend_i ${_cmpp_depends})
        if(TARGET ${_cmpp_depend_i})
            get_target_property(
                _cmpp_depend_inc ${_cmpp_depend_i} INTERFACE_INCLUDE_DIRECTORIES
            )
            list(APPEND _cmpp_inc_dir ${_cmpp_depend_inc})
        endif()
    endforeach()
    message("incdirs2 ${_cmpp_inc_dir}")
    #Printout
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()
    #List of header files
    get_target_property(_cmpp_headers ${_cmpp_target} PUBLIC_HEADER)
    find_file(_fileloc atom.hpp)
    message("location ${_fileloc}")
    #The library name (obviously a generator...)
    set(_cmpp_lib "$<TARGET_FILE_NAME:${_cmpp_target}>")
    message("output dir ${_cmpp_OUTPUT_DIR}")

    #---------------------------------------------------------------------------
    #-----------------Generate __init__.py file contents------------------------
    #---------------------------------------------------------------------------
    set(_cmpp_file_name "${_cmpp_OUTPUT_DIR}/__init__.py")
    set(_cmpp_file "import cppyy\n")
    set(_cmpp_file "${_cmpp_file}import os\n")
    set(_cmpp_file "${_cmpp_file}paths = \"${_cmpp_inc_dir}\".split(';')\n")
    set(_cmpp_file "${_cmpp_file}for p in paths:\n")
    set(_cmpp_file "${_cmpp_file}    if p:\n")
    set(_cmpp_file "${_cmpp_file}        cppyy.add_include_path(p)\n")
    set(_cmpp_file "${_cmpp_file}headers = \"${_cmpp_headers}\".split(';')\n")
    set(_cmpp_file "${_cmpp_file}for h in headers:\n")
    set(_cmpp_file "${_cmpp_file}    if h:\n")
    set(_cmpp_file "${_cmpp_file}        cppyy.include(h)\n")
    set(_cmpp_file "${_cmpp_file}dir = os.path.realpath(__file__)\n")
    set(_cmpp_file "${_cmpp_file}dir = os.path.dirname(dir)\n")
    set(_cmpp_file "${_cmpp_file}lib = os.path.join(\"${CMAKE_BINARY_DIR}\", \"${_cmpp_lib}\")\n")
    set(_cmpp_file "${_cmpp_file}cppyy.load_library(lib)\n")
    set(_cmpp_file "${_cmpp_file}from cppyy.gbl import ${_cmpp_NAMESPACE}\n")
    set(_cmpp_file "${_cmpp_file}from cppyy.gbl import std\n")
    #Write it out
    file(GENERATE OUTPUT ${_cmpp_file_name} CONTENT "${_cmpp_file}")
    message("done generating")
endfunction()
