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
will automatically also expose your library to SDE and LibChemist). For sake of
argument we'll assume your module computes overlap integrals of Gaussian basis
functions using a deep neural network. Given how the module works we decide to 
call it ``DeepOverlap`` (module names are case-sensitive) and we decide to call
the library ``DeepIntegrals``.

``DeepIntegrals`` needs a very simple header file, ``deep_integrals.hpp``:

.. code:: c++

   #pragma once           // Header guard; avoids multiple inclusions
   #include <sde/sde.hpp> // Makes SDE visible to your module

   namespace deep_integrals {

   DECLARE_MODULE(DeepOverlap);
    
   // if your library provides more than one module, other DECLARE_MODULE 
   // statements would show up here 

   // This is the hook NWChemEx uses to enter your library
   void load_modules(sde::ModuleManager& mm);

   } // namespace deep_integrals


Overlap integrals are of type ``property_types::ao_integrals::Overlap<T>`` 
where ``T`` is a floating-point type (**e.g.**, ``float`` or ``double``). The 
definition of ``DeepOverlap`` will look something like:

.. code:: c++

   #include "path/to/deep_integrals.hpp"
   #include <property_types/ao_integrals/overlap.hpp>

   // Typedef of the Overlap property type for readability
   using pt = property_types::ao_integrals::Overlap<double>;

   namespace deep_integrals {
    
   MODULE_CTOR(DeepOverlap) {

       // Register that your module satisfies the Overlap property type
       satisfies_property_type<pt>();
   }
    
   MODULE_RUN(DeepOverlap) {

       // This is boilerplate for unwrapping the inputs provided to the module
       const auto& [bra, ket] = pt::unwrap_inputs(inputs);

       // Step 1: Translate the AO basis sets for the bra and ket to your 
       // library's native format
         
       // Step 2: Compute the overlap integral tensor

       // Step 3: Convert the overlap integral tensor from your library's format 
       // to NWChemEx's tensor format 
       auto S = get_overlap_integral_in_NWX_format();  

       // This is boilerplate for wrapping and returning the results
       auto rv = results();
       return pt::wrap_results(rv, S);
   }

   } // namespace your_library_namespace

Finally we need to define the ``load_modules`` function, which we do in its own
source file:

.. code:: c++

   #include "path/to/deep_integrals.hpp"

   namespace deep_integrals {

   void load_modules(chemist::ModuleManager& mm) {
       mm.add_module<DeepOverlap>("Deep Overlap");
   }

   } // namespace deep_integrals
   
Python
------

TODO: Write me!!!!

