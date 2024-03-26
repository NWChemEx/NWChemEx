.. Copyright 2022 NWChemEx-Project
..
.. Licensed under the Apache License, Version 2.0 (the "License");
.. you may not use this file except in compliance with the License.
.. You may obtain a copy of the License at
..
.. http://www.apache.org/licenses/LICENSE-2.0
..
.. Unless required by applicable law or agreed to in writing, software
.. distributed under the License is distributed on an "AS IS" BASIS,
.. WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
.. See the License for the specific language governing permissions and
.. limitations under the License.

.. _pluginplay_primer:

******************
PluginPlay Primer
******************

The guts of NWChemEx uses the PluginPlay framework. Additional functionality is added 
to the PluginPlay by writing plugins called "modules". Generally speaking each module 
can compute one or more properties. For example a module which runs a 
Hartree-Fock calculation will likely compute the total electronic energy, the
canonical molecular orbitals, and the Fock matrix. Associated with each property
is an API called the "property type". The property types define the hooks that
NWChemEx uses to call plugins. To interface your code with NWChemEx you need to
wrap it in a module, register the properties your module can compute, and 
register the module with NWChemEx. NWChemEx will handle everything else.


Writing a module
================

NWChemEx and the PluginPlay are able to work with modules written in either C++ or in
Python.

C++
---

If you want to write your module in C++ you will need to create a library with
your module. Your library will need to link to the SimDE repo (which 
will automatically also expose your library to PluginPlay and Chemist). For sake of
argument we'll assume your module computes overlap integrals of Gaussian basis
functions using a deep neural network. Given how the module works we decide to 
call it ``DeepOverlap`` (module names are case-sensitive) and we decide to call
the library ``DeepIntegrals``.

``DeepIntegrals`` needs a very simple header file, ``deep_integrals.hpp``:

.. code:: c++

   #pragma once           // Header guard; avoids multiple inclusions
   #include <pluginplay/pluginplay.hpp> // Makes PluginPlay visible to your module

   namespace deep_integrals {

   DECLARE_MODULE(DeepOverlap);
    
   // if your library provides more than one module, other DECLARE_MODULE 
   // statements would show up here 

   // This is the hook NWChemEx uses to enter your library
   void load_modules(pluginplay::ModuleManager& mm);

   } // namespace deep_integrals


Overlap integrals are of type ``simde::EOverlap`` 
The definition of ``DeepOverlap`` will look something like:

.. code:: c++

   #include "path/to/deep_integrals.hpp"
   #include <simde/simde.hpp>

   // Typedef of the Overlap property type for readability
   using pt = simde::EOverlap;

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

   void load_modules(pluginplay::ModuleManager& mm) {
       mm.add_module<DeepOverlap>("Deep Overlap");
   }

   } // namespace deep_integrals
   
Python
------

TODO: Write me!!!!

