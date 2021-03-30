**********
SDE Primer
**********

The guts of NWChemEx uses the SDE framework. Additional functionality is added 
to the SDE by writing plugins called "modules". Generally speaking each module 
can compute one or more properties. For example a module which runs a 
Hartree-Fock calculation will likely compute the total electronic energy, the
canonical molecular orbitals, and the Fock matrix. Associated with each property
is an API called the "property type". The property types define the hooks that
NWChemEx uses to call plugins. To interface your code with NWChemEx you need to
wrap it in a module, register the properties your module can compute, and 
register the module with NWChemEx. NWChemEx will handle everything else.


Writing a module
================

NWChemEx and the SDE are able to work with modules written in either C++ or in
Python.

C++
---

If you want to write your module in C++ you will need to create a library with
your module. Your library will need to link to the PropertyTypes repo (which 
will automatically also expose your library to SDE and LibChemist also). Your
library will need a very simple header file:

.. code:: c++

    #pragma once           // Header guard; avoids multiple inclusions
    #include <sde/sde.hpp> // Makes SDE visible to your module

    namespace your_librarys_namespace {

    void load_modules(sde::ModuleManager& mm);

    } // namespace your_librarys_namespace



If your library will include multiple modules its best to declare them all in
one header file like:

.. code:: c++

   #pragma once
   #include <sde/sde.hpp> 

   namespace your_library_namespace {

   DECLARE_MODULE(Module0); 
   DECLARE_MODULE(Module1);
   
   //Possibly even more modules
   
   } // namespace your_librarys_namespace

Python
------

TODO: Write me!!!!

