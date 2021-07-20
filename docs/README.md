Source Files for NWChemEx Documentation
=======================================

This directory contains the source files for generating the documentation for  NWChemEx.

Obtaining the Documentation's Dependencies
------------------------------------------

The documentation has several dependencies. The dependencies can all be
installed via Python's `pip` command. To install the dependencies follow the
instructions for your operating system.

Linux:

~~~.sh
#These first two steps are strongly recommended, but not required
python3 -m venv venv
. venv/bin/activate
pip3 install -r requirements.txt
~~~

Building the Documentation
--------------------------

With the dependencies installed the documentation is built by running the build
command for your operating system (commands are assumed to be run in the same directory as this README file).

Linux:

~~~.sh
make html BUILDDIR=${BUILDDIR}
~~~

where `${BUILDDIR}` is the directory where you want the resulting HTML to be
placed.

After building the main index of the resulting HTML will be located at
`${BUILDDIR}/html/index.html` and can be viewed by directing your web browser of
choice to that file (this is done by using `file:///path/to/index.html` as the
URL).
