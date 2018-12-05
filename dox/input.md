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

2. Read input, add parameters (including structure, basis set info, etc.) to parameter class that gets instantiated upfront.

3. Ensure modules calling submodules do not overwrite user set parameters upfront. Needs an infrastructure.

4. Need for graph class to be able to setup workflow, module execution tree.

5. We call the main modules with a parameters class included.

Mock-up from Ryan:

```.cpp
class ModuleBase {
    virtual void register_submodule_(ModuleManager& mm) = 0;
    virtual void register_parameters_(Paramters& params) = 0;
    virtual void register_returns_(Returns& returns) = 0;
    
    Returns run_(const Parameters& params) = 0;
    
};

struct DummySCF : ModuleBase {
    void register_submodule_(ModuleManager& mm) {
        mm.register_submodule<FockBuilder>("FockBuilder");
        mm.register_submodule<MOBuilder>("MOBuilder");
    }
    
    void register_parameters_(Parameters& params) {
        params.add(Option<Molecule>("System"));
        params.add(Option<AOBasisSet>("Basis"));
        params.add(Option<double>("Threshold", 10E-6));
        params.add(Option<int>("Derivative", 0));
    }
    
    void register_returns_(Returns& returns) {
        Returns.add(Return<Tensor>("Energy"));
        Returns.add(Return<OrbitalSpace>("MOs"));
    }
    
    Returns run(const Parameters& params) {
        //Grab our parameters
        auto mol = params.at<Molecule>("System");
        auto bs = params.at<AOBasisSet>("Basis");
        
        // Do stuff...
        
        //(example of calling a submodule)
        auto rv = 
            run_submodule_as<FockBuilder>("FockBuilder", bs, bs, mos);
        
        // Done doing stuff
        
        //Set returns
        Returns returns;
        returns.add(Return<Tensor>("Energy", energy));
        returns.add(Return<OrbitalSpace>("MOs", mos));
        return returns;
    }
};

struct EnergyPropertyType {
    
    Tensor run(Module& mod, Moleucle mol, int deriv) {
        Parameters params = mod.params();
        params.change("System", mol);
        params.change("Derivative", deriv);
        auto rv = mod.run(params);
        return rv.at<Tensor>("Energy");
    }
};

struct SCFPropertyType {
    
    auto run(Module& mod, Molecule mol, AOBasisSet bs, int deriv) {
        Parameters params = mod.params;
        params.change("System", mol);
        params.change("Basis", bs);
        params.change("Derivative", deriv);
        
        auto rv = mod.run(params);
        return std::make_tuple(rv.at<Tensor>("Energy"), 
                               rv.at<OrbitalSpace>("MOs"));
    }
};


auto scf = mm.at("DummySCF"); 
scf.run_as<SCFPropertyType>(mol, bs, deriv);
```


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
