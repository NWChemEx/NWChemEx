<!--
  ~ Copyright 2022 NWChemEx-Project
  ~
  ~ Licensed under the Apache License, Version 2.0 (the "License");
  ~ you may not use this file except in compliance with the License.
  ~ You may obtain a copy of the License at
  ~
  ~ http://www.apache.org/licenses/LICENSE-2.0
  ~
  ~ Unless required by applicable law or agreed to in writing, software
  ~ distributed under the License is distributed on an "AS IS" BASIS,
  ~ WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  ~ See the License for the specific language governing permissions and
  ~ limitations under the License.
-->

Source Files for NWChemEx Documentation
=======================================

This directory contains the source files for generating the documentation for
NWChemEx.

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
command for your operating system (commands are assumed to be run in the same
directory as this README file).

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
