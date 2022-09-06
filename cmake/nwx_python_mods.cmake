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

# This function will skim a CMake target and create a file __init__.py that
# should be placed next to the shared library created by that target. This
# function assumes the target's:
#
# * public header files are in the ``PUBLIC_HEADER`` property
# * include paths are in the ``INTERFACE_INCLUDE_DIRECTORIES`` property
# * dependencies are targets and in the ``INTERFACE_LINK_LIBRARIES`` property
#
# :Additional Named Arguments:
#     * NAMESPACES - The C++ namespace that your bindings live in. 
#     * PACKAGE - Package name to used as an alternative to NAMESPACE.  
#     * DEPPACKAGES - Packages this module depends on.
#     * PYTHONIZE - Add special function to Pythonize class with more complex 
#       arguments. Helps LibChemist.
#     * MPI - When set will ensure MPI stuff is handled properly.
#

function(cppyy_make_python_package)
    #---------------------------------------------------------------------------
    #----------------------Check if Python bindings need to be build------------
    #---------------------------------------------------------------------------
    if (NOT BUILD_PYBINDINGS)
        return()
    endif()
    #---------------------------------------------------------------------------
    #----------------------Do not build bindings if shared_libs is off----------
    #---------------------------------------------------------------------------
    if (NOT BUILD_SHARED_LIBS)
        message(WARNING, "BUILD_SHARED_LIBS is OFF, and Python needs shared libraries")
        return()
    endif()
    #---------------------------------------------------------------------------
    #-----------------------Make sure we have cppyy installed-------------------
    #---------------------------------------------------------------------------
    find_package(Cppyy REQUIRED)
    #---------------------------------------------------------------------------
    #--------------------------Argument Parsing---------------------------------
    #---------------------------------------------------------------------------
    #set(options MPI PYTHONIZE BLAS TILED)
    set(options MPI PYTHONIZE)
    set(oneValueArgs PACKAGE)
    set(multiValueArgs NAMESPACES DEPPACKAGES)
    cmake_parse_arguments(install_data "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
    #---------------------------------------------------------------------------
    #--------------------------Get include directories--------------------------
    #---------------------------------------------------------------------------
    get_true_target_property(include_dirs ${install_data_PACKAGE} INTERFACE_INCLUDE_DIRECTORIES)
    list(APPEND include_dirs ${MPI_CXX_HEADER_DIR})
    list(APPEND include_dirs ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
    list(REMOVE_DUPLICATES include_dirs)
    #---------------------------------------------------------------------------
    #--------------------------Get headers to include---------------------------
    #---------------------------------------------------------------------------
    get_target_property(include_headers ${install_data_PACKAGE} PUBLIC_HEADER)
    get_filename_component(header_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)
    #---------------------------------------------------------------------------
    #-----------------Blacklist headers we do not want to expose----------------
    #---------------------------------------------------------------------------
    list(FILTER include_headers EXCLUDE REGEX ".*linalg_inner_tensors\\.hpp$")
    list(FILTER include_headers EXCLUDE REGEX ".*pow\\.hpp$")
    #---------------------------------------------------------------------------
    #------------Collect the information we need off the target-----------------
    #---------------------------------------------------------------------------
    set(target_lib "$<TARGET_FILE_NAME:${install_data_PACKAGE}>")
    set(output_dir "${CMAKE_BINARY_DIR}/Python/${install_data_PACKAGE}")
    #---------------------------------------------------------------------------
    #----Defines needed by BTAS and Madness at runtime are needed by cppyy------
    #---------------------------------------------------------------------------
    set(python_defines_file "${output_dir}/python_defines.hpp")
    set(python_defines "#define MADNESS_HAS_CEREAL\n")
    if(BTAS_USE_BLAS_LAPACK)
        set(python_defines "#define BTAS_HAS_BLAS_LAPACK\n")
    endif()
    file(GENERATE OUTPUT ${python_defines_file} CONTENT "${python_defines}")
    #---------------------------------------------------------------------------
    #-----------------Generate _init__.py file contents------------------------
    #---------------------------------------------------------------------------
    set(init_file_name "${output_dir}/__init__.py")
    set(init_file "import cppyy\n\n")
    foreach(deppackage ${install_data_DEPPACKAGES})
        set(init_file "${init_file}from ${deppackage} import \*\n")
    endforeach()
    set(init_file "${init_file}import os\n")
    set(init_file "${init_file}paths = list(set(\"${include_dirs}\".split(';')))\n")
    set(init_file "${init_file}for p in paths:\n")
    set(init_file "${init_file}    if p and p!=\"\":\n")
    set(init_file "${init_file}        cppyy.add_include_path(p)\n")
    #---------------------------------------------------------------------------
    #--Temporary band-aid for MADworld MPI threads fixed in future cling/cppyy---
    #---------------------------------------------------------------------------
    if (install_data_MPI)
        set(init_file "${init_file}cppyy.cppdef(\"\"\"\\ \n")
        set(init_file "${init_file}\#define thread_local\n")
        set(init_file "${init_file}\#define is_server_thread \*_cling_is_server_thread()\n")
        set(init_file "${init_file}\#include \"${MADNESS_SOURCE_DIR}/src/madness/world/worldrmi.h\"\n")
        set(init_file "${init_file}\#undef thread_local\n")
        set(init_file "${init_file}\"\"\")\n")
    endif()
    #---------------------------------------------------------------------------
    #--End of temporary band-aid------------------------------------------------
    #---------------------------------------------------------------------------
    set(init_file "${init_file}cppyy.include(\"${python_defines_file}\")\n")
    set(init_file "${init_file}headers = \"${include_headers}\".split(';')\n")
    set(init_file "${init_file}for inc in headers:\n")
    set(init_file "${init_file}    cppyy.include(inc)\n")
    set(init_file "${init_file}\ncppyy.load_library(\"${CMAKE_CURRENT_BINARY_DIR}/${target_lib}\")\n\n")
    foreach(namespace ${install_data_NAMESPACES})
        set(init_file "${init_file}from cppyy.gbl import ${namespace}\n")
    endforeach()
    set(init_file "${init_file}from cppyy.gbl.std import array, vector, make_shared\n\n")
    if(install_data_PYTHONIZE)
        set(init_file "${init_file}def pythonize_class(klass, name):\n")
        set(init_file "${init_file}    def x_init(self, *args, **kwds):\n")
        set(init_file "${init_file}        newargs = list(args)\n")
        set(init_file "${init_file}        for kw, value in kwds.items():\n")
        set(init_file "${init_file}            try:\n")
        set(init_file "${init_file}                newargs.append(getattr(klass, kw)(value))\n")
        set(init_file "${init_file}            except AttributeError as e:\n")
        set(init_file "${init_file}                break\n")
        set(init_file "${init_file}        else:\n")
        set(init_file "${init_file}            return self.__orig_init__(*newargs)\n")
        set(init_file "${init_file}        raise TypeError(\"__init__\(\) got an unexpected keyword argument \'\%s\'\" \% kw)\n\n")
        set(init_file "${init_file}    klass.__orig_init__ = klass.__init__\n")
        set(init_file "${init_file}    klass.__init__ = x_init\n\n")
        set(init_file "${init_file}cppyy.py.add_pythonization(pythonize_class, \"${install_data_PACKAGE}\")\n")
    endif()
    #Write it out
    file(GENERATE OUTPUT ${init_file_name} CONTENT "${init_file}")
endfunction()


#---------------------------------------------------------------------------
#--------Function to recursively find all include directories needed--------
#---------------------------------------------------------------------------
function( get_true_target_property _out _target _property )
  if( TARGET ${_target} )
    get_property( _${_target}_imported TARGET ${_target} PROPERTY IMPORTED )
    if( NOT ${_property} MATCHES "INTERFACE_LINK_LIBRARIES" )
      get_property( _${_target}_property TARGET ${_target} PROPERTY ${_property} )
    endif()
    if( 1 )
      get_property( _${_target}_link TARGET ${_target} PROPERTY INTERFACE_LINK_LIBRARIES )
      foreach( _lib ${_${_target}_link} )
        if( TARGET ${_lib} )
          get_true_target_property( _${_lib}_property ${_lib} ${_property} )
          if( _${_lib}_property )
            list( APPEND _${_target}_property_imported ${_${_lib}_property} )
          endif()
        elseif( ${_property} MATCHES "INTERFACE_LINK_LIBRARIES" )
          list( APPEND _${_target}_property_imported ${_lib} )
        endif()
      endforeach()
      if(_${_target}_property_imported)
        list(APPEND _${_target}_property ${_${_target}_property_imported} )
      endif()
      set( ${_out} ${_${_target}_property} PARENT_SCOPE )
    else()
      set( ${_out} ${_${_target}_property} PARENT_SCOPE )
    endif()
  endif()
endfunction()
