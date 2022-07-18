Building the NWChemEx Package
=============================

Prerequisites
-------------

To compile NWChemEx, some packages must be available on your system beforehand:

#. A C++ compiler supporting the C++17 standard, currently we use gcc 9 (10.2 on Intel), clang has not been fully tested
#. CMake, usually the latest version is the best to use, be aware that some Linux distributions (Ubuntu for example) are often behind on CMake versions in the default build
#. BLAS/LAPACK/ScaLAPACK or standards equivalent libraries, partially tested options include OpenBlas, NetLib, Blis, FLAME, MKL
#. Boost, currently 1.59 as a minimum; note that bleeding edge Boost often requires bleeding edge compilers, so be careful here
#. MPI, most of the code requires at least MPI-3 compliance, MPICH 3.4 has been tested
#. Eigen, minimum of 3.4.0; the build will automatically pull this if you don't have it available
#. libint2 - see below for more information
#. Depending on what type of GPU you have available to you, you will need CUDA 11 or higher, SYCL 2.0 (use the latest version of the OneAPI SDK), or HIP 4.5 (use the latest ROCm version)

If necessary, more installation details for a package will be provided in the
subsections below.

NWChemEx also depends on a number of repositories listed below. Please visit 
the repositories and ensure that any additional packages that must be available
before building are installed.

#. `integrals <https://github.com/NWChemEx-Project/Integrals>`__
#. `SCF <https://github.com/NWChemEx-Project/SCF>`__
#. `MP2 <https://github.com/NWChemEx-Project/MP2>`__

BLAS/LAPACK/ScaLAPACK
^^^^^^^^^^^^^^^^^^^^^

**Intel MKL (Intel oneAPI MKL):** Ensure that all required environment 
variables for the Intel MKL are set by executing ``/path/to/intel/mkl/bin/
mklvars.sh <arch>``, where ``<arch>`` must be either ``ia32`` or ``intel64``.
The following command can be added to your ``.bashrc`` to set the required 
environment variables automatically for all new terminal sessions:

.. code-block:: bash

   . /path/to/intel/mkl/bin/mklvars.sh <arch>

**Other BLAS/LAPACK/ScaLAPACK:** If not using the Intel MKL, ensure that 
environment variables for the packages are set up correctly according to the
package-specific instructions.

libint2
^^^^^^^
.. note::
   **Important:** Boost must be installed and in your path prior to building
   libint2. This is not required by libint2, as libint2 will use its own
   internal build of Boost. However, when building NWChemEx, errors related
   to using this internal build of Boost have been observed (by Zach Crandall
   as of April 29, 2021).

The libint2 build will take a long time (probably >2 hrs), and should be 
started well in advance. At the time of writing, libint v2.6.0 can be obtained 
from the `libint v2.6.0 release <https://github.com/evaleev/libint/releases/
tag/v2.6.0>`__ page and built using instructions at `libint Wiki 
<https://github.com/evaleev/libint/wiki>`__ if a custom library is needed.

The following build script can be used to build and install the libint2
pre-generated library "lmax=6 library (standard ints only)", which can be
downloaded from the
`libint v2.6.0 release <https://github.com/evaleev/libint/releases/tag/
v2.6.0>`__ page or directly from `here <https://github.com/evaleev/libint/
releases/download/v2.6.0/libint-2.6.0.tgz>`__. Download and extract the 
pre-generated libint v2.6.0 library. Inside the newly extracted libint-2.6.0 
directory, create a file named ``build.sh`` and paste the below contents, 
modifying everything in angled brackets (<>) to be correct for your system.

.. code-block:: bash

   #!/usr/bin/env bash
    
   cmake -S . \
         -B build \
	 -DCMAKE_INSTALL_PREFIX=<desired_install_location> \
	 -DCMAKE_CXX_COMPILER=<c++_compiler> \
	 -DCMAKE_CXX_FLAGS="-O3" \
	 -DCMAKE_CXX_STANDARD=11 \
	 -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE \
	 2>&1 | tee OUTPUT.GEN
	 
   cmake --build build --target install 2>&1 | tee OUTPUT.BUILD

GitHub Personal Access Token
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

A GitHub Personal Access Token (PAT) is necessary since, at the moment, 
NWChemEx and some dependencies are hosted in private repositories. To create a 
PAT, follow the instructions at GitHub's `Creating a personal access token
<https://docs.github.com/en/github/authenticating-to-github/
creating-a-personal-access-token>`_ page. This PAT will be used when prompted 
for a password while cloning repositories.


Building NWChemEx
-----------------

The build instructions are given in the following sections based on a few
assumptions:

#. You are using a sane Unix-like operating system.
#. All components will be installed in the same location.

The following two files will be created to build NWChemEx, with instructions for
each in the sections below:

#. CMake Toolchain File: ``toolchain.cmake``
#. NWChemEx Build Script: ``build_nwx.sh``

CMake Toolchain File
^^^^^^^^^^^^^^^^^^^^

NWChemEx requires knowledge of many packages and tools which may have system-
specific installation locations. Inside the top level directory where NWChemEx
will be built, create a toolchain file named ``toolchain.cmake``. This file
should contain the following information, replacing everything in angled
brackets (<>) for your system.

.. code-block:: cmake

   # Compilers
   set(CMAKE_C_COMPILER   <C compiler>)
   set(CMAKE_CXX_COMPILER <C++ compiler>)
   set(MPI_C_COMPILER     <MPI C compiler>)
   set(MPI_CXX_COMPILER   <MPI CXX compiler>)

   # Token for private repos
   set(CPP_GITHUB_TOKEN <your_super_secret_github_PAT>)

   # Options
   set(CMAKE_POSITION_INDEPENDENT_CODE TRUE)
   set(BUILD_SHARED_LIBS TRUE)
   set(BUILD_TESTING TRUE)
   set(CMAKE_PREFIX_PATH <prefix_directory>) # This is where libint2 is installed
   set(CMAKE_CXX_STANDARD 17)

   # BLAS/LAPACK
   set(ENABLE_SCALAPACK ON)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOMPI_SKIP_MPICXX")


NWChemEx Build Script
^^^^^^^^^^^^^^^^^^^^^

Create a new file named ``build_nwx.sh`` next to ``toolchain.cmake`` and paste
the script below into it. This script will download, build, and install NWChemEx 
and any remaining dependencies. Logs for the build will be generated beside this
build script.

.. code-block:: bash

   # Clone the repo
   git clone https://github.com/NWChemEx-Project/NWChemEx.git 2>&1 | tee "OUTPUT.GITCLONE"
   # <Type login information if prompted.>
   
   # Navigate into the newly created NWChemEx subdirectory
   cd NWChemEx
   
   # Generate project buildsystem
   cmake -H. \
         -Bbuild \
         -DCMAKE_TOOLCHAIN_FILE=`pwd`/../toolchain.cmake \
         -DCMAKE_BUILD_TYPE=Release \
        #-DCMAKE_INSTALL_PREFIX=<where/you/want/to/install> # cannot install right now
         2>&1 | tee "../OUTPUT.GEN"

   # Build the project
   cmake --build build \
        #--target install \ # we cannot actually install yet
         2>&1 | tee "../OUTPUT.BUILD"

   # Run tests
   cd build && ctest 2>&1 | tee "../../OUTPUT.TEST"

   # Return to the top level directory
   cd ../..

.. note::
   For finer-grained control over the build, we direct the reader to the more
   thorough CMaize build instructions located `here 
   <https://cmakepackagingproject.readthedocs.io/en/latest/?badge=latest>`_
   and note that NWChemEx depends on several other projects:
       
   * `Catch2 <https://github.com/catchorg/Catch2>`_ (for testing only)

