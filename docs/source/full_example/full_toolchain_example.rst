So you want a toolchain.cmake example?
======================================

Lets say you're like me and you need a good example in order to replicate 
anything that you see? This is what that is. This is like taking CMake and
forcing it to do your bidding not because it wants to, but because cmake is
a bumbling fool of a program and needs all the bumper rails it needs in a
bowling match. We're talking the greatest representation of what it would
look like to hold CMake's hand to every part of the configuration process for
the project in order to have any semblance of a functioning build system.

This is that information.

CMake build command:
--------------------
`cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=./toolchain.cmake`

CMake toolchain.cmake file:
---------------------------

.. code-block:: cmake

   set(BUILD_DOCS ON)
   set(BUILD_FULL_CHEMCACHE ON) # What is considered "Full"
   set(BUILD_PYBIND11_PYBINDINGS ON)
   set(BUILD_TESTING ON)
   set(Boost_INCLUDE_DIR /path/to/boost/include/directory) # Mine is /usr/include
   set(CMAKE_BUILD_TYPE Debug) # Can also be Release
   set(CMAKE_INSTALL_PREFIX /path/to/desired/install/directory) # Mine is /home/jacob/Applications/NWChemEx/install
   set(CMAKE_PREFIX_PATH /path/to/prefix/directories) # Like Libint2, etc. mine is /home/jacob/Libraries/libint-2.9.0/install
   set(NWX_MODULE_DIRECTORY /patt/to/module/install/directory) # Where you would like to install the NWChemEx module, I usually just set mine to the same as the CMAKE_INSTALL_PREFIX
   set(ENABLE_ASE ON) # Adds Atomic Simulation Environment (ASE) support
   set(FETCHCONTENT_BASE_DIR /path/to/base/content/dir) # Where dependencies will be populated, mine is set to the build directory and then _deps, so /home/jacob/Applications/NWChemEx/build/_deps
   set(FETCHCONTENT_FULLY_DISCONNECTED OFF) # Disables any fetching of directories
   set(FETCHCONTENT_QUIET ON)
   
   # This next section is useful to definte if you would like to only use the 
   # local source files available on the machine. This means nothing is fetched 
   # or downloaded for the specified dependency. If you are a NWChemEx developer,
   # it might be useful to grab all the dependencies and point each of the fetch 
   # content variables to their respective paths. /path/to/<dependency>/source 
   # typically refers to the top level directory of the git clone directory, for
   # example, if I clone cereal into the '/home/jacob/dependencies/cereal' path, the top
   # level would be set at /home/jacob/dependencies/cereal.

   set(FETCHCONTENT_SOURCE_DIR_CEREAL /path/to/cereal/source)
   set(FETCHCONTENT_SOURCE_DIR_CHEMCACHE /path/to/chemcache/source)
   set(FETCHCONTENT_SOURCE_DIR_CHEMIST /path/to/chemist/source)
   set(FETCHCONTENT_SOURCE_DIR_CMAIZE /path/to/cmaize/source)
   set(FETCHCONTENT_SOURCE_DIR_CMAKEPP_LAND /path/to/cmakepplang/source)
   set(FETCHCONTENT_SOURCE_DIR_EIGEN /path/to/eigen/source)
   set(FETCHCONTENT_SOURCE_DIR_FRIENDZONE /path/to/friendzone/source)
   set(FETCHCONTENT_SOURCE_DIR_INTEGRALS /path/to/integrals/source)
   set(FETCHCONTENT_SOURCE_DIR_LIBFORT /path/to/libfort/source)
   set(FETCHCONTENT_SOURCE_DIR_NUX /path/to/nux/source)
   set(FETCHCONTENT_SOURCE_DIR_NWX_CMAKE /path/to/nwx_cmake/source)
   set(FETCHCONTENT_SOURCE_DIR_PARALLELZONE /path/to/parallelzone/source)
   set(FETCHCONTENT_SOURCE_DIR_PLUGINPLAY /path/to/pluginplay/source)
   set(FETCHCONTENT_SOURCE_DIR_PYBIND11 /path/to/pybind11/source)
   set(FETCHCONTENT_SOURCE_DIR_SIMDE /path/to/simde/source)
   set(FETCHCONTENT_SOURCE_DIR_SPDLOG /path/to/spdlog/source)
   set(FETCHCONTENT_SOURCE_DIR_TENSORWRAPPER /path/to/tensorwrapper/source)
   set(FETCHCONTENT_SOURCE_DIR_UTILITIES /path/to/utilities/source)
   
   # The referenced python should be from a python virtual environment and not
   # from the global installation.

   set(Python_EXECUTABLE /path/to/python/environment/python/executable) # For example, mine is /home/jacob/Environments/base/bin/python3.13
   set(Python3_EXECUTABLE /path/to/python/environment/pyhon/executable) # Same as above, best to just set it.
   set(Python_INCLUDE_DIR /path/to/python/include/dir) # Mine is automatically set to /usr/include/python3.13
   set(Python_LIBRARY /path/to/python/library) # Mine is automatically set to /usr/lib/libpython3.13.so ..

Building NWChemEx from scratch for Development:
-----------------------------------------------

Here is what I would do:

#. Install or build Libint2

Quick Setup Guide for Libint2

Obtain the source code for Libint2, I've been using version 2.9.0. `libint-2.9.0.tar.gz <https://github.com/evaleev/libint/archive/refs/tags/v2.9.0.tar.gz>`_

`tar xvf libint2-2.9.0.tar.gz`

cd libint-2.9.0

`cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/path/to/desired/install/directory`

`cmake --build build --target install --parallel <number of cores you wish to use>`

This will build and take a while depending on how many cores you use. It took me about 1 hour with 8 cores with an 8th generation Intel CPU on a laptop.

