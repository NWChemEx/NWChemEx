Input Infrastructure
====================

The purpose of this document is to prototype/discuss what input to NWChemEx
looks like to an end-user.

Startup and Input/Parameter Instantiation
-----------------------------------------

1. Module manager load all modules. Analogue for Python:

```.python
import NWChemEx as NWX
```

2. Read input, add parameters (including structure, basis set info, etc.) to parameter class.

3. Ensure modules calling submodules do not overwrite user set parameters upfront. Needs an infrastructure.

4. Need for graph class to be able to setup workflow, module execution tree.


Single Point Calculation
------------------------

Debatedly the simplest possible electronic structure computation is a single
point computation.  Under ideal circumstances (no fancy hootenanny) this should
be doable by:

```.python
import NWChemEx as NWX
mol = NWX.make_system("""
                      H 0.0 0.0 0.00
                      H 0.0 0.0 0.89
                      """, "aug-cc-pVDZ")
print(NWX.energy("SCF", mol))
```

The first line is straightforward and required by Python.  This 
line loads the NWChemEx Python module and aliases it to `NWX` for convenience
(the alias is up for discussion, but we should be consistent).  The second line 
is calls a (currently non-existent) convenience function which will take a 
string representation of the molecule (here in Cartesian coordinates) and the 
name of an atomic orbital basis set.  The function could be overloaded to take
other input mechanisms for the molecule, say, a Python file object containing  
the molecule, or the name of the molecule (NWChemEx ships with geometries for
common molecules; could also conceivably use the name to retrieve coordinates
from the internet).  We anticipate the basis set always being requested by the
name used to register it with NWChemEx (again NWChemEx ships with a variety of
basis sets).  Users wanting a custom basis set would need to either add it to
 NWChemEx's basis set repository or manually add it to the returned molecule
 instance.
 
 The final line uses the NWX energy function, which is a (currently 
 non-existent) convenience function that:
 - calls `NWX.init`
   - this function (also non-existent) would wrap setting up files for 
     saving/loading the cache (name's derived from name of Python script),
     initialize MPI, and TAMM
- makes the default NWChemEx ModuleManager instance filled with all the usual
  modules.
- gets the module with the specified key (here `"SCF"`)
- runs said module by forwarding the remaining arguments
- deletes the ModuleManager (thereby triggering the saving of the Cache to disk)
- returns the resulting energy to the user
As written the energy returned by `NWX.Energy` would then be printed ending 
the script (calling of MPI finalize *etc.* would be handled by using `atexit` 
in the NWChemEx module to register callbacks).

Chaining Computations
---------------------

Say we ran the computation in the previous section and then realized we wanted
the gradient.  The gradient for the SCF energy reuses many of the quantities 
computed during the energy and we don't want to recompute them, but rather we
want to use the cached values.  If we modify the previous script so that it 
instead says:
    
```.python
import NWChemEx as NWX
mol = NWX.make_system("""
                      H 0.0 0.0 0.00
                      H 0.0 0.0 0.89
                      """, "aug-cc-pVDZ")
print(NWX.energy("SCF", mol))
print(NWX.deriv(1, "SCF", mol))
```    

we will achieve the desired result (reuse of the cached results) when we rerun
the computation.  There's admittedly several assumptions behind this:
1. We have implemented the `energy` convenience function `deriv` by passing 0 as
   the first argument to a `deriv` function and the latter actually does the
   stuff described above.
2. The SCF gradient module calls the same `UpdateDensity` module as the 
   SCF energy did.  Otherwise we have a Cache miss.
3. The user has not deleted the saved cache, renamed it or the script, or called
   the script from another directory.
   - We by default assume a cache with the same name as the Python script.  
     We could make our `deriv` function take the path to the cache as kwarg.
