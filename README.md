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

# NWChemEx

[Documentation](https://nwchemex-project.github.io/NWChemEx)

## About This Repo

First a tiny-bit of background.  NWChemEx attempts to break from the 
traditional "monolithic" electronic structure package by decomposing the overall
electronic structure package into smaller, independent packages called modules.  
Each module is its own GitHub repository and can be used as is, without the rest
of NWChemEx (aside from its dependencies).  
 
For the most part, modules only depend on NWChemExRuntime and its dependencies 
(which we intend to keep minimal beyond standard dependencies such as BLAS and
MPI).  All other modules get what they need from NWChemExRuntime via abstract 
base classes.  Within NWChemEx these abstract base classes are implemented 
via the various modules; however, users are free to substitute their own 
modules.  For example, all routines needing integrals use the integral API 
defined in NWChemExRuntime; one possible integrals implementation 
is provided by IntegralsEx, but users do not have to use it.  This is similar
to how one can (in theory) substitute various vendor BLAS libraries without 
modifying the code.

The current repo serves as the public face of the NWChemEx software suite.  It
is capable of building the runtime, various plugins, and assembling them.  
Additionally, this repo provides tools to interact with the resulting 
software suite in a number of ways. 

## Building

See
[Build Instructions](https://nwchemex-project.github.io/NWChemEx/installation/building.html#)
in the documentation.

## Running jobs

NWX is really designed to be driven from Python. At the moment the Python API is
a work in progress and NWX can only be reliably driven from C++. The easiest way
to drive NWX from C++ is the "Driving NWX from C++" unit test. This test is
built when you build the test suite. To enable building of the test suite pass
`BUILD_TESTING=TRUE` to CMake.


