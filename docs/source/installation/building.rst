Building the SDE Library
========================

SDE is built using `CPP <https://github.com
/CMakePackagingProject/CMakePackagingProject.git>`_.
Assuming you have already installed CPP, that you are on a sane Unix-like
computer, and you are willing to let SDE build all dependencies, then the
following will suffice to build SDE:

.. code-block:: bash

   git clone https://github.com/NWChemEx-Project/SDE.git
   cd SDE
   cmake -H. \
         -Bbuild \
         -DCPP_GITHUB_TOKEN=<your super-secret token> \
         -DCMAKE_PREFIX_PATH=<where/you/installed/CPP> \
         -DCMAKE_INSTALL_PREFIX=<where/you/want/to/install/SDE>
   cd build
   cmake --build .
   #May need to run as an admin depending on where you are installing SDE to
   cmake --build . --target install

.. note::

    The GitHub token is only necessary because, at the moment, utilities is a
    private repository (instructions for generating a token are `here
    <https://help.github.com/articles/creating-a-personal-access-token-for
    -the-command-line>`_).

For finer-grained control over the build we direct the reader to the more
thorough CPP build instructions located `here <https://cmakepackagingproject
.readthedocs.io/en/latest/end_user/quick_start.html>`_ and note that SDE
depends on several other projects:

* `utilities <https://github.com/NWChemEx-Project/utilities>`_
* `bphash <https://github.com/bennybp/BPHash>`_
* `cereal <https://github.com/USCiLab/cereal>`_
* `Catch2 <https://github.com/catchorg/Catch2>`_ (for testing only)
