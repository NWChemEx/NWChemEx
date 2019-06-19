Building the NWChemEx Package
=============================

NWChemEx is built using `CMakePP <https://github.com
/CMakePackagingProject/CMakePackagingProject.git>`_.
Further TAMM depends on CMakeBuild., where CMakeBuild requires at least
CMake 3.14.2 or higher to build.
TAMM also depends on GCC compilers of version 7.2 or higher.
Assuming you have already installed CMakePP, that you are on a sane Unix-like
computer, and you are willing to let NWChemEx build all dependencies, then the
following will suffice to build NWChemEx.

First, create a toolchain file in the top-level directory (`toolchain.cmake`). This
file should contain the following information

.. code-block:: cmake

   set(CMAKE_PREFIX_PATH      <prefix directory>)
   set(CMAKE_INSTALL_PREFIX   <install directory>)
   set(CMAKE_C_COMPILER       <C compiler>)
   set(CMAKE_CXX_COMPILER     <C++ compiler>)
   set(CMAKE_Fortran_COMPILER <Fortran compiler>)
   set(PYTHON_EXECUTABLE      <Python interpreter>)
   set(PYTHON_LIBRARY         <Python library (libpythonX.X)>)

.. note::

   The Python interpreter must support Numpy. I.e. after invoking Python 
   `import numpy` must succeed.

.. code-block:: bash

   git clone https://github.com/NWChemEx-Project/CMakeBuild.git
   cd CMakeBuild
   cmake -H. \
         -Bbuild \
         -DCMAKE_INSTALL_PREFIX=<where/you/want/to/install/CMakeBuild>
   cd build
   cmake --build .
   #May need to run as an admin depending on where you are installing CMakeBuild to
   cmake --build . --target install
   cd ../..

   git clone https://github.com/NWChemEx-Project/TAMM.git
   cd TAMM
   cmake -H. \
         -Bbuild \
         -DCMAKE_INSTALL_PREFIX=<where/you/want/to/install/TAMM>
   cd build
   cmake --build .
   #May need to run as an admin depending on where you are installing TAMM to
   cmake --build . --target install
   cd ../..

   git clone https://github.com/NWChemEx-Project/SCF.git
   cd SCF
   cmake -H. \
         -Bbuild \
         -DCPP_GITHUB_TOKEN=<your super-secret token> \
         -DCMAKE_TOOLCHAIN_FILE=`pwd`/../toolchain.cmake \
   cd build
   cmake --build .
   #May need to run as an admin depending on where you are installing SCF to
   cmake --build . --target install
   cd ../..

.. note::

    The GitHub token is only necessary because, at the moment, utilities is a
    private repository (instructions for generating a token are `here
    <https://help.github.com/articles/creating-a-personal-access-token-for
    -the-command-line>`_).

For finer-grained control over the build we direct the reader to the more
thorough CMakePP build instructions located `here <https://cmakepackagingproject
.readthedocs.io/en/latest/end_user/quick_start.html>`_ and note that NWChemEx
depends on several other projects:

* `TAMM <https://github.com/NWChemEx-Project/TAMM`_
  * `CMakeBuild <https://github.com/NWChemEx-Project/CMakeBuild`_

* `utilities <https://github.com/NWChemEx-Project/utilities>`_
* `bphash <https://github.com/bennybp/BPHash>`_
* `cereal <https://github.com/USCiLab/cereal>`_
* `Catch2 <https://github.com/catchorg/Catch2>`_ (for testing only)
