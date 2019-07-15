Building the NWChemEx Package
=============================

Before any attempts at compiling NWChemEx can be successful there are
a few packages that need to be available. 

First of all, NWChemEx depends on the TAMM library for the tensor
operation infrastructure. In turn compiling TAMM requires the CMakeBuild
library. More details on downloading and installing these components will
be provided below.

Secondly, NWChemEx builds using `CMakePP 
<https://github.com/CMakePackagingProject/CMakePackagingProject.git>`_. 
CMakePP extends
the CMake package with advanced dependency management capabilities. 
This package enables checking for, downloading, and building dependencies
automatically. A side effect of using this capability is that running CMake
will do more than just configuring the build and generating the makefiles. 
The reason is that CMakePP has to compile all the missing dependencies to
successfully configure the build. 
Hence, a first run of CMake can take considerably longer than
would be expected from a normal configuration run. When all 
dependencies are in place, subsequent CMake runs should be much faster.

Below the build instructions are given based on a few assumptions:
1. You are using a sane Unix-like operating system
2. You have already installed CMakePP
3. You have Python installed and the installation supports numpy, i.e.
the command `import numpy` must succeed
4. You are going to install all components in the same location

First, create a toolchain file in the top-level directory (`toolchain.cmake`). This
file should contain the following information

.. code-block:: cmake

   set(CMAKE_PREFIX_PATH      <prefix directory>)
   set(CMAKE_C_COMPILER       <C compiler>)
   set(CMAKE_CXX_COMPILER     <C++ compiler>)
   set(CMAKE_Fortran_COMPILER <Fortran compiler>)
   set(PYTHON_EXECUTABLE      <Python interpreter>)
   set(PYTHON_LIBRARY         <Python library (libpythonX.X)>)


The following script will then build and install CMakeBuild, TAMM and NWChemEx.

.. code-block:: bash

   git clone https://github.com/NWChemEx-Project/CMakeBuild.git
   cd CMakeBuild
   cmake -H. \
         -Bbuild \
         -DCMAKE_TOOLCHAIN_FILE=`pwd`/../toolchain.cmake \
         -DCMAKE_INSTALL_PREFIX=<where/you/want/to/install>
   cd build
   cmake --build .
   #May need to run as an admin depending on where you are installing CMakeBuild to
   cmake --build . --target install
   cd ../..

   git clone https://github.com/NWChemEx-Project/TAMM.git
   cd TAMM
   cmake -H. \
         -Bbuild \
         -DCMAKE_TOOLCHAIN_FILE=`pwd`/../toolchain.cmake \
         -DCMAKE_PREFIX_PATH=<where/you/want/to/install> \
         -DCMAKE_INSTALL_PREFIX=<where/you/want/to/install>
   cd build
   cmake --build .
   #May need to run as an admin depending on where you are installing TAMM to
   cmake --build . --target install
   cd ../..

   git clone https://github.com/NWChemEx-Project/NWChemEx.git
   cd NWChemEx
   cmake -H. \
         -Bbuild \
         -DCPP_GITHUB_TOKEN=<your super-secret token> \
         -DCMAKE_TOOLCHAIN_FILE=`pwd`/../toolchain.cmake \
         -DCMAKE_PREFIX_PATH=<where/you/want/to/install> \
         -DCMAKE_INSTALL_PREFIX=<where/you/want/to/install>
   cd build
   cmake --build .
   #May need to run as an admin depending on where you are installing NWChemEx to
   cmake --build . --target install
   cd ../..

.. note::

    The GitHub token is only necessary because, at the moment, various
    dependencies are hosted in
    private repositories (instructions for generating a token are `here
    <https://help.github.com/articles/creating-a-personal-access-token-for-the-command-line>`_).

.. note::

    The `CMAKE_PREFIX_PATH` need to be declared both in the `toolchain.cmake`
    file as well as on the CMake command line to propagate 
    properly to a module and all of its dependencies.

For finer-grained control over the build we direct the reader to the more
thorough CMakePP build instructions located `here 
<https://cmakepackagingproject.readthedocs.io/en/latest/end_user/quick_start.html>`_
and note that NWChemEx depends on several other projects:

* `TAMM <https://github.com/NWChemEx-Project/TAMM>`_

  + `CMakeBuild <https://github.com/NWChemEx-Project/CMakeBuild>`_

* `bphash <https://github.com/bennybp/BPHash>`_

* `cereal <https://github.com/USCiLab/cereal>`_

* `Catch2 <https://github.com/catchorg/Catch2>`_ (for testing only)

