#This CMake module ensures that your Python interpreter has the cppyy module
#installed. It also defines a function ``cppyy_make_python_package`` which will
#skim a CMake target and automatically generate a
find_package(PythonInterp REQUIRED)
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "import cppyy"
    RESULT_VARIABLE _fcppyy_result
    OUTPUT_VARIABLE _fcppyy_output
    ERROR_VARIABLE  _fcppyy_error
)

if(NOT "${_fcppyy_result}" STREQUAL "")
    set(Cppyy_FOUND FALSE)
endif()

function(cppyy_skim_incs all_incs target)
    #List of target's include directories, usually a generator
    get_target_property(_csi_inc_dir ${target} INTERFACE_INCLUDE_DIRECTORIES)
    #List of libraries, usually a mix of targets and libraries
    get_target_property(_csi_depends ${target} INTERFACE_LINK_LIBRARIES)
    #Get each dependency's include directory by recursion
    foreach(_csi_depend_i ${_csi_depends})
        if(TARGET ${_csi_depend_i})
            cppyy_skim_incs(_csi_depend_incs ${_csi_depend_i})
            list(APPEND _csi_inc_dir ${_csi_depend_incs})
        endif()
        set(${all_incs} "${_csi_inc_dir}" PARENT_SCOPE)
    endforeach()
endfunction()

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
    set(_cmpp_options NAMESPACE PREFIX OUTPUT_DIR)
    cmake_parse_arguments(_cmpp "" "${_cmpp_options}" "" ${ARGN})
    if("${_cmpp_PREFIX}" STREQUAL "")
        get_filename_component(_cmpp_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)
    endif()
    if("${_cmpp_OUTPUT_DIR}" STREQUAL "")
        set(_cmpp_OUTPUT_DIR ${CMAKE_BINARY_DIR}/${_cmpp_PREFIX})
    endif()


    #---------------------------------------------------------------------------
    #------------Collect the information we need off the target-----------------
    #---------------------------------------------------------------------------

    #List of header files
    get_target_property(_cmpp_headers ${_cmpp_target} PUBLIC_HEADER)

    #The include paths
    cppyy_skim_incs(_cmpp_inc_dir ${_cmpp_target})

    #The library name (obviously a generator...)
    set(_cmpp_lib "$<TARGET_FILE_NAME:${_cmpp_target}>")

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
    set(_cmpp_file "${_cmpp_file}    inc = os.path.join(\"${_cmpp_PREFIX}\",h)\n")
    set(_cmpp_file "${_cmpp_file}    cppyy.include(inc)\n")
    set(_cmpp_file "${_cmpp_file}dir = os.path.realpath(__file__)\n")
    set(_cmpp_file "${_cmpp_file}dir = os.path.dirname(dir)\n")
    set(_cmpp_file "${_cmpp_file}lib = os.path.join(dir, \"${_cmpp_lib}\")\n")
    set(_cmpp_file "${_cmpp_file}cppyy.load_library(lib)\n")
    set(_cmpp_file "${_cmpp_file}cppyy.include(\"ga.h\")\n")
    set(_cmpp_file "${_cmpp_file}cppyy.include(\"macdecls.h\")\n")            
    set(_cmpp_file "${_cmpp_file}from cppyy.gbl import ${_cmpp_NAMESPACE}\n")
    set(_cmpp_file "${_cmpp_file}from sde import sde\n")
    set(_cmpp_file "${_cmpp_file}from libchemist import libchemist\n")
    set(_cmpp_file "${_cmpp_file}from scf import scf\n")
    set(_cmpp_file "${_cmpp_file}from mpi4py import MPI\n")
    set(_cmpp_file "${_cmpp_file}cppyy.gbl.GA_Initialize()\n")
    set(_cmpp_file "${_cmpp_file}cppyy.gbl.MA_init(1004, 8000000, 20000000)\n")
    set(_cmpp_file "${_cmpp_file}import atexit\n")
    set(_cmpp_file "${_cmpp_file}atexit.register(cppyy.gbl.GA_Terminate)\n")            



    #Write it out
    file(GENERATE OUTPUT ${_cmpp_file_name} CONTENT "${_cmpp_file}")
endfunction()
