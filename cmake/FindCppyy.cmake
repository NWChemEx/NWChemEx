#This CMake module ensures that your Python interpreter has the cppyy module
#installed. It also defines a function ``cppyy_make_python_package`` which will
#skim a CMake target and automatically generate a

find_package(Python3 COMPONENTS Interpreter REQUIRED)

#Check if cppyy Python package exists
execute_process(
    COMMAND Python3::Interpreter -c "import cppyy"
    RESULT_VARIABLE _fcppyy_result
)
if(NOT "${_fcppyy_result}" STREQUAL "")
    #Try to install cppyy Python package, as it doesn't exist
    if(DEFINED ENV{VIRTUAL_ENV} OR DEFINED ENV{CONDA_PREFIX})
      set(_pip_args)
    else()
      set(_pip_args "--user")
    endif()
    set(_pypkg_name "cppyy")
    execute_process(COMMAND ${Python3_EXECUTABLE} -m pip install ${_pypkg_name} ${_pip_args})
    #Check again if cppyy Python works
    execute_process(COMMAND Python3::Interpreter -c "import cppyy" RESULT_VARIABLE _fcppyy_result)
    if(NOT "${_fcppyy_result}" STREQUAL "")
       set(Cppyy_FOUND FALSE)
    endif()
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
        set(_cmpp_OUTPUT_DIR "${CMAKE_BINARY_DIR}/PyNWChemEx")
    endif()

    #---------------------------------------------------------------------------
    #------------Collect the information we need off the target-----------------
    #---------------------------------------------------------------------------
    #List of include directories, usually a generator
    get_target_property(
            _cmpp_inc_dir ${_cmpp_target} SOURCE_DIR)
    #Add dependent libraries
    list(APPEND _cmpp_inc_dir ${utilities_SOURCE_DIR} ${libchemist_SOURCE_DIR})
    list(APPEND _cmpp_inc_dir ${property_types_SOURCE_DIR} ${sde_SOURCE_DIR})
    list(APPEND _cmpp_inc_dir ${BPHash_SOURCE_DIR})
    list(APPEND _cmpp_inc_dir "${MADNESS_SOURCE_DIR}/src" "${MADNESS_BINARY_DIR}/src")
    list(APPEND _cmpp_inc_dir "${TiledArray_SOURCE_DIR}/src" "${TiledArray_BINARY_DIR}/src")
    list(APPEND _cmpp_inc_dir "${blacspp_SOURCE_DIR}/include" "${scalapackpp_SOURCE_DIR}/include")
    list(APPEND _cmpp_inc_dir ${MPI_CXX_HEADER_DIR}) 
    list(APPEND _cmpp_inc_dir ${CMAKE_INSTALL_OLDINCLUDEDIR} "${CMAKE_INSTALL_OLDINCLUDEDIR}/eigen3")
    #The library name (obviously a generator...)
    set(_cmpp_lib "$<TARGET_FILE_NAME:${_cmpp_target}>")

    #---------------------------------------------------------------------------
    #-----------------Generate __init__.py file contents------------------------
    #---------------------------------------------------------------------------
    set(_cmpp_file_name "${_cmpp_OUTPUT_DIR}/__init__.py")
    set(_cmpp_file "import cppyy\n\n")
    foreach(_item ${_cmpp_inc_dir})
          set(_cmpp_file "${_cmpp_file}cppyy.add_include_path(\"${_item}\")\n")
    endforeach()
    set(_cmpp_file "${_cmpp_file}cppyy.include(\"${nwchemex_SOURCE_DIR}/nwchemex/load_modules.hpp\")\n")
    set(_cmpp_file "${_cmpp_file}cppyy.include(\"${libchemist_SOURCE_DIR}/libchemist/libchemist.hpp\")\n")
    set(_cmpp_file "${_cmpp_file}cppyy.include(\"${sde_SOURCE_DIR}/sde/module_manager.hpp\")\n")
    set(_cmpp_file "${_cmpp_file}cppyy.include(\"${property_types_SOURCE_DIR}/property_types/types.hpp\")\n")
    set(_cmpp_file "${_cmpp_file}cppyy.include(\"${property_types_SOURCE_DIR}/property_types/reference_wavefunction.hpp\")\n")
    set(_cmpp_file "${_cmpp_file}cppyy.include(\"${property_types_SOURCE_DIR}/property_types/correlation_energy.hpp\")\n")
    set(_cmpp_file "${_cmpp_file}cppyy.include(\"${property_types_SOURCE_DIR}/property_types/core_hamiltonian.hpp\")\n")
    set(_cmpp_file "${_cmpp_file}\ncppyy.load_library(\"${CMAKE_BINARY_DIR}/${_cmpp_lib}\")\n\n")
    set(_cmpp_file "${_cmpp_file}from cppyy.gbl import sde, libchemist, nwx, property_types, TA\n")
    set(_cmpp_file "${_cmpp_file}from cppyy.gbl.std import array, vector\n")
    set(_cmpp_file "${_cmpp_file}from ctypes import c_int\n\n")
    set(_cmpp_file "${_cmpp_file}def pythonize_class(klass, name):\n")
    set(_cmpp_file "${_cmpp_file}    def x_init(self, *args, **kwds):\n")
    set(_cmpp_file "${_cmpp_file}        newargs = list(args)\n")
    set(_cmpp_file "${_cmpp_file}        for kw, value in kwds.items():\n")
    set(_cmpp_file "${_cmpp_file}            try:\n")
    set(_cmpp_file "${_cmpp_file}                newargs.append(getattr(klass, kw)(value))\n")
    set(_cmpp_file "${_cmpp_file}            except AttributeError as e:\n")
    set(_cmpp_file "${_cmpp_file}                break\n")
    set(_cmpp_file "${_cmpp_file}        else:\n")
    set(_cmpp_file "${_cmpp_file}            return self.__orig_init__(*newargs)\n")
    set(_cmpp_file "${_cmpp_file}        raise TypeError(\"__init__\(\) got an unexpected keyword argument \'\%s\'\" \% kw)\n\n")
    set(_cmpp_file "${_cmpp_file}    klass.__orig_init__ = klass.__init__\n")
    set(_cmpp_file "${_cmpp_file}    klass.__init__ = x_init\n\n")
    set(_cmpp_file "${_cmpp_file}cppyy.py.add_pythonization(pythonize_class, \"libchemist\")\n")
    #Write it out
    file(GENERATE OUTPUT ${_cmpp_file_name} CONTENT "${_cmpp_file}")
endfunction()
