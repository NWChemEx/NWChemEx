# This function will skim a CMake target and create a file _init__.py that
# should be placed next to the shared library created by that target. This
# function assumes the target's:
#
# * public header files are in the ``PUBLIC_HEADER`` property
# * include paths are in the ``INTERFACE_INCLUDE_DIRECTORIES`` property
# * dependencies are targets and in the ``INTERFACE_LINK_LIBRAIRES`` property
#
#
# :Additional Named Arguments:
#     * NAMESPACE - The C++ namespace that your bindings live in. 
#     * DEPNAMESPACES - The C++ namespaceis that your bindings require,
#       i.e. previous Python package builds.
#     * PACKAGE - Package name to used as an alternative to NAMESPACE.  
#     * DEPENDS - List of modules this module depends on.
#     * PYTHONIZE - Add special function to Pythonize class with more complex 
#       arguments. Helps LibChemist.
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
    set(options MPI PYTHONIZE BLAS TA)
    set(oneValueArgs PACKAGE)
    set(multiValueArgs NAMESPACES DEPPACKAGES)
    cmake_parse_arguments(install_data "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
    #---------------------------------------------------------------------------
    #--------------------------Get include directories--------------------------
    #---------------------------------------------------------------------------
    get_target_property(include_dirs ${install_data_PACKAGE} INTERFACE_INCLUDE_DIRECTORIES)
    get_target_property(link_libs ${install_data_PACKAGE} INTERFACE_LINK_LIBRARIES)
    foreach(item ${link_libs})
        get_target_property(include_item ${item} INTERFACE_INCLUDE_DIRECTORIES)
        list(APPEND include_dirs ${include_item})
    endforeach()
    if (install_data_MPI)
        list(APPEND include_dirs ${MPI_CXX_HEADER_DIR})
    endif()
    #---------------------------------------------------------------------------
    #--------------------------Get headers to includer--------------------------
    #---------------------------------------------------------------------------
    get_target_property(include_headers ${install_data_PACKAGE} PUBLIC_HEADER)
    get_filename_component(header_PREFIX ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)
    #---------------------------------------------------------------------------
    #------------Collect the information we need off the target-----------------
    #---------------------------------------------------------------------------
    set(target_lib "$<TARGET_FILE_NAME:${install_data_PACKAGE}>")
    set(output_dir "${CMAKE_BINARY_DIR}/Python/${install_data_PACKAGE}")
    #---------------------------------------------------------------------------
    #-Defines in BTAS, BLAS, LAPACK, and Madness at runtime are needed by cppyy-
    #---------------------------------------------------------------------------
    set(python_defines_file "${output_dir}/python_defines.hpp")
    set(python_defines "#define MADNESS_HAS_CEREAL\n")
    if(BTAS_USE_BLAS_LAPACK)
        set(python_defines "#define BTAS_HAS_BLAS_LAPACK\n")
    endif()
    if(install_data_BLAS)
       list(APPEND include_dirs ${BTAS_SOURCE_DIR})
       list(APPEND include_dirs ${blaspp_BINARY_DIR}/include ${blaspp_SOURCE_DIR}/include)
       list(APPEND include_dirs ${lapackpp_SOURCE_DIR}/include ${lapackpp_BINARY_DIR}/include)
    endif()
    if(install_data_TA)
       list(APPEND include_dirs ${TiledArray_SOURCE_DIR}/src ${TiledArray_BINARY_DIR}/src)
       get_property(EIGEN3_INCLUDE_DIRS TARGET TiledArray_Eigen PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
       list(APPEND include_dirs ${EIGEN3_INCLUDE_DIRS})
       get_property(UMPIRE_INCLUDE_DIRS TARGET TiledArray_UMPIRE PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
       list(APPEND include_dirs ${UMPIRE_INCLUDE_DIRS})
    endif()
    if(ENABLE_SCALAPACK)
       list(APPEND include_dirs ${blacspp_SOURCE_DIR}/include ${scalapackpp_SOURCE_DIR}/include)
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
    set(init_file "${init_file}for h in headers:\n")
    set(init_file "${init_file}    inc = os.path.join(\"${header_PREFIX}\",h)\n")
    set(init_file "${init_file}    cppyy.include(inc)\n")
    set(init_file "${init_file}\ncppyy.load_library(\"${CMAKE_CURRENT_BINARY_DIR}/${target_lib}\")\n\n")
    foreach(namespace ${install_data_NAMESPACES})
        set(init_file "${init_file}from cppyy.gbl import ${namespace}\n")
    endforeach()
    set(init_file "${init_file}from cppyy.gbl.std import array, vector, make_shared\n\n")
    if(install_data_PYTHONIZE)
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
        set(_cmpp_file "${_cmpp_file}cppyy.py.add_pythonization(pythonize_class, \"${namespace}\")\n")
    endif()
    #Write it out
    file(GENERATE OUTPUT ${init_file_name} CONTENT "${init_file}")
endfunction()
