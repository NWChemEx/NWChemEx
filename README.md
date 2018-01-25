# NWChemEx

## About NWChemEx

NWChemEx attempts to break from the traditional "monolithic" electronic 
structure package by decomposing the overall electronic structure package into
smaller, independent packages called modules.  Each module is its own GitHub 
repository and can be used as is, without the rest of NWChemEx (aside from any
 modules it depends on).  The current module structure of NWChemEx is 
 summarized in the following diagram:
 
 ![](dox/uml/program_structure.png)
 
 Basically all modules only depend on NWChemExRuntime and it's dependencies 
 (which we intend to keep minimal; currently classes for modeling chemical 
 concepts and tensors).  All other modules then get what they need from 
 NWChemExRuntime via abstract base classes (of course these classes must be 
 implemented by something, but users are free to hook-up their own 
 implementations rather than say using the ones in IntegralsEx).
 
 
