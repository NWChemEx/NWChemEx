# NWChemEx

## About This Repo

First a tiny-bit of background.  NWChemEx attempts to break from the 
traditional "monolithic" electronic structure package by decomposing the overall
electronic structure package into smaller, independent packages called modules.  
Each module is its own GitHub repository and can be used as is, without the rest
of NWChemEx (aside from its dependencies).  The current module structure of 
NWChemEx is summarized in the following diagram:
 
![](dox/uml/program_structure.png)
 
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
is capable of building all of the various submodules and assembling them.  
Additionally, this repo provides tools to interact with the resulting 
software suite in a number of ways. 
