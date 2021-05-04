Building the NWChemEx Package
=============================

Prerequisites
-------------

To compile NWChemEx, some packages must be available on your system beforehand:

#. A C++ compiler supporting the C++17 standard
#. CMake
#. BLAS/LAPACK/ScaLAPACK
#. Boost
#. MPI
#. libint2

If necessary, more installation details for a package will be provided in the subsections below.

NWChemEx also depends on a number of repositories listed below. Please visit the repositories and ensure that any additional packages that must be available before building are installed.

#. `integrals <https://github.com/NWChemEx-Project/Integrals>`
#. `SCF <https://github.com/NWChemEx-Project/SCF>`
#. `MP2 <https://github.com/NWChemEx-Project/MP2>`

BLAS/LAPACK/ScaLAPACK
^^^^^^^^^^^^^^^^^^^^^

Intel MKL (Intel oneAPI MKL)
""""""""""""""""""""""""""""

Ensure that all required environment variables for the Intel MKL are set by executing `/path/to/intel/mkl/bin/mklvars.sh <arch>`, where `<arch>` must be either `ia32` or `intel64`. The following command can be added to your `.bashrc` to set the required environment variables automatically for all new terminal sessions:

.. code-block:: bash
    . /path/to/intel/mkl/bin/mklvars.sh <arch>

Other BLAS/LAPACK/ScaLAPACK
"""""""""""""""""""""""""""

If not using the Intel MKL, closely study the BLAS/LAPACK section of the NWChemEx `toolchain.cmake` file below to ensure all paths are correct for your BLAS/LAPACK/ScaLAPACK installations.

libint2
^^^^^^^

**Important:** Boost must be installed and in your path prior to building libint2. This is not required by libint2, as libint2 will use its own internal build of Boost. However, when building NWChemEx, errors related to using this internal build of Boost have been observed (by Zach Crandall as of April 29, 2021).

The libint2 build will take a long time (probably >2 hrs), and should be started well in advance. At the time of writing, libint v2.6.0, the latest release, can be obtained from `Download of libint v2.6.0 <https://github.com/evaleev/libint/releases/download/v2.6.0/libint-2.6.0.tgz>` and built using instructions at `libint Wiki <https://github.com/evaleev/libint/wiki#compiling-libint-library>`, although the instructions are not 100% accurate at the moment so some modifications are needed.
**Note:** For some reason, the download on the `v2.6.0 release <https://github.com/evaleev/libint/archive/refs/tags/v2.6.0.tar.gz>` does not have the necessary CMakeLists.txt file.

The following build script can be used to build and install libint2. Download and extract the above linked libint v2.6.0. Inside the newly extracted libint-2.6.0 directory, create a file named `build.sh` and paste the below contents, modifying everything in angled brackets (<>) to be correct for your system.

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

A GitHub Personal Access Token (PAT) is necessary since, at the moment, NWChemEx and some dependencies are hosted in private repositories. To create a PAT, follow the instructions at GitHub's `Creating a personal access token <https://docs.github.com/en/github/authenticating-to-github/creating-a-personal-access-token>` page. This PAT will be used when prompted for a password while cloning repositories.


Building NWChemEx
-----------------

The build instructions are given in the following sections based on a few assumptions:

#. You are using a sane Unix-like operating system.
#. All components will be installed in the same location.

The following two files will be created to build NWChemEx, with instructions for each in the sections below:

#. CMake Toolchain File: `toolchain.cmake`
#. NWChemEx Build Script: `build_nwx.sh`

CMake Toolchain File
^^^^^^^^^^^^^^^^^^^^

NWChemEx requires knowledge of many packages and tools which may have system-specific installation locations. Inside the top level directory where NWChemEx will be built, create a toolchain file named `toolchain.cmake`. This file should contain the following information, replacing everything in angled brackets (<>) for your system.

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
   set(MKLROOT $ENV{MKLROOT}) # Environment variable set through your MKL install or a script
   set(LAPACK_LIBRARIES "-L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lpthread -lm -ldl")
   set(LAPACK_INCLUDE_DIRS "${MKLROOT}/include")
   set(lapack_LIBRARIES ${LAPACK_LIBRARIES})
   set(SCALAPACK_LIBRARIES  -L${MKLROOT}/lib/intel64 -Wl,--no-as-needed -lmkl_scalapack_lp64 -lmkl_intel_lp64 -lmkl_sequential -lmkl_core -lmkl_blacs_openmpi_lp64 -lpthread -lm -ldl)
   set(blacs_LIBRARIES ${SCALAPACK_LIBRARIES})
   set(scalapack_LIBRARIES ${SCALAPACK_LIBRARIES})
   set(lapack_LIBRARIES ${LAPACK_LIBRARIES})
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOMPI_SKIP_MPICXX")


NWChemEx Build Script
^^^^^^^^^^^^^^^^^^^^^

Create a new file named `build_nwx.sh` next to `toolchain.cmake` and paste the script below into it. This script will download, build, and install NWChemEx and any remaining dependencies. Logs for the build will be generated beside this build script.

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

