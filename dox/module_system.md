The Module System
=================

ModuleManager
-------------

### Adding Modules


The `ModuleManger` serves as a container for storing all of the modules known to
the SDE. Associated with each module is a unique string known as the module's
key. The module key is then used as a handle for referring to that particular
module instance. The `ModuleManger` is by default empty, thus the first step of
any usage of the SDE is to fill it. While it is possible to do this manually,
*e.g.*,

```
ModuleManger mm;
mm.insert("key 4 module 1", [](){return std::make_shared<Module1>();});
mm.insert("key 4 module 2", ...);
...
```

this is incredibly tedious. Instead, modules are distributed in what are called
supermodules. Conceptually, supermodules are nothing more than modules that a
developer has decided to bundle together. In practice, supermodules typically
provide all of the modules required to solve one set of problems, *e.g.*, in 
NWChemEx the modules required to solve the SCF equations are bundled together as
a supermodule. Supermodule's are loaded *via*:

```
#include <NWChemEx/NWChemEx.hpp>
ModuleManger mm;
nwx::load_supermodule(mm);
```

or in Python:

```
import NWChemEx as nwx
mm = nwx.ModuleManager()
nwx.load_supermodule(mm)
```

This will load all of the NWChemEx modules into the `ModuleManger` instance for
you. If for whatever reason you only want to load, say, the SCF modules, you
can do:

```
#include <NWChemEx/NWChemEx.hpp>
ModuleManger mm;
nwx::scf::load_supermodule(mm);
```

or in Python:

```
import NWChemEx as nwx
mm = nwx.ModuleManger()
nwx.scf.load_supermodule(mm)
```

Additional modules not included with NWChemEX can then be loaded into the
`ModuleManger` in an analogous manner:

```
#include <NWChemEx/NWChemEx.hpp>
#include <OtherPackage/loader.hpp>
ModuleManger mm;
nwx::scf::load_supermodule(mm);
OtherPackage::load_supermodule(mm);

//Add modules not distributed as part of a supermodule
mm.insert("a key", [](){return std::make_shared<ModuleType>();});
```

or in Python:

```
import NWChemEx as nwx
import OtherPackage
mm = nwx.ModuleManger()
nwx.scf.load_supermodule(mm)
OtherPackage.load_supermodule(mm)

mm.insert("a key", lambda : ModuleType())
```

Consult the documentation for the supermodule you are using to determine what
keys it uses.

### Changing Default Parameters

In order for a module to run it typically requires some input parameters. 
Exactly what input parameters a module requires is highly module-specific. For
this reason the SDE introduces the `Parameters` class. The `Parameters` class is
a glorified map from strings to values, with the added perk that the values may
be of basically any type (copy constructable types only?). The module's
documentation is the authoritative source on which parameters a module supports.
Inevitably, there will arise scenarios in which the module's default parameters
are inappropriate (too tight, too loose, *etc.*). Changing the parameters is
easy:

```
// Changes the module with module key, "ModuleKey", "Tolerance" parameter to a
// value of 10E-8
mm.at("ModuleKey").params().change("Tolerance", 10E-8);
``` 

Note that this changes the value of "Tolerance" for all calls to "ModuleKey". If
you only want to change the parameter in a particular instance, you first copy
"ModuleKey":

```
// Makes a new module under the module key "ModifiedModuleKey" that is a copy of
// the module with module key "ModuleKey"
mm.copy("ModuleKey", "ModifiedModuleKey");
```

and then change the copy's parameter:

```
mm.at("ModifiedModuleKey").params().change("Tolerance", 10E-8)
```

### Changing Default Modules

When supermodules are loaded, the call graph has not yet been assembled. That is
to say, each module knows that it will call a set of submodules, but  each 
module does not yet know which specific submodule to call. Assembling this set 
is the `ModuleManager`'s other responsibility. The set is determined when you
request the module. The contents of the set are determined by introspection of
the requested module. For example, if the module says it needs a module 
satisfying the property type `SCF`, then the `ModuleManager` will set it so 
that the module calls whatever module is currently set to be used as the 
default `SCF` module. 

```
// Sets it so that requests for property type "SCF" default to the module with
// module key "SCFKey"
mm.set_default_module<SCF>("SCFKey");
```

This is great for setting a broad swath of modules quickly, but sometimes you
need more control. For this reason the `ModuleManager` also allows you to 
specify exactly which submodule a module should call; the syntax is similar to
setting the parameters:

```
// change the submodule the module, "ModuleKey", calls when it calls 
// "SubmoduleKey" to the module with key "ModifiedModuleKey"
mm.at("ModuleKey").submodules().change("SubmoduleKey", "ModifiedModuleKey");
```

Running a Module
----------------

Once the module you want to run is suitably set up, you run it by:

```
auto mod = mm.at("ModuleKey");
auto rv = mod.run_as<PropertyType>(arg1, arg2);
```

Here we are requesting that the module with module key "ModuleKey" is run as a
module of property type `PropertyType`. For sake of argument, we've assumed that
`PropertyType` is defined such that it takes two arguments (`arg1` and `arg2` in
the present example) and returns a single value, `rv`. As far as static analysis
is concerned, every module can be run as every property type, thus we can not
rely on the compiler to deduce if "ModuleKey" is actually of type 
`PropertyType`. The SDE will still implement such a check, but it will happen at
 runtime. 

A similar procedure is done to run submodules. 

Modules
-------

When a user wants to implement a new algorithm they do so by creating a new
module. To do this the user inherits from `ModuleBase` and implements the 
required API. For example, let's implement an SCF module, the code:

```
class OurSCF : public ModuleBase {
private:
    void register_submodules_(ModuleManager& mm) {...}
    void register_parameters_(Parameters& params){...}
    void register_returns_(Returns& returns){...}
    Returns run_(const Parameters& params, const Submodules& submods){...}
};
```

For ease of presentation we have elided the definitions for the time being. The
first function, `register_submodules_`, is used to tell the `ModuleManager`
what types of submodules your module will call. `register_parameters_` defines
the input API to your module. `register_returns_` defines the output API to
your module, and finally `run_` actually executes your module's code. 

### register_submodules_

Within the framework of the SDE modules encapsulate the details pertaining to
how a quantity is computed. Often when writing a module you need a quantity that
was not part of your input. You have two options compute the quantity yourself,
or ask another module to do it for you. In this scenario, your module typically
doesn't care how the quantity is computed, only that it is. It falls to the
`ModuleManager` to provide your module with the submodules it should call. This
requires your module to tell the `ModuleManager` what submodules your module 
will need. This is the purpose of the `register_submodules_` function.

For our sample SCF code, this looks something like: 

```
    void register_submodules_(ModuleManager& mm) {
        // Tell the mm that we need:
        
        // a module satisfying property type "MOGuess"
        mm.register_submodule<MOGuess>("Initial Guess");
        
        // a module satisfying property type "FockBuilder"
        mm.register_submodule<FockBuilder>("FBuilder");
    
        // a module satisfying property type "MOBuilder"
        mm.register_submodule<MOBuilder>("MOBuilder");
        
    }
```

The syntax is `mm.register_submodule<X>("Y", Z)`, where `X` is the C++ type of
the property type the module must satisfy and `"Y"` is the submodule key your 
module will use to refer to it (*vide infra*). `Z` is an optional module key, 
not used in our example, that allows your module to request that a specific 
module (*i.e.*, the one with the specified module key) be used to satisfy the 
submodule dependency. 

The actual process by which the `ModuleManager` assigns modules is somewhat
complicated and it does not guarantee that your module will use any 
particular module (even if you specify the key). This is because the user has
the final say in which submodule a module calls. If your module only works with
a very specific submodule it is best if your module directly calls that module   
instead of registering it with the `ModuleManager`. 

### register_parameters_

Each module needs a series of input parameters in order 
to perform its task. These parameters are module specific, although they should
be designed such that the module satisfies as many relevant property types as
possible. Regardless, it is the purpose of the `register_parameters_` function
to tell the SDE what parameters your module accepts. This is done by filling in
an empty `Parameters` instance.

Returning to our example SCF module:

```
void register_parameters_(Parameters& params) {
    // Declare a parameter, allowing the user to set:

    // the initial molecular system, of type Molecule
    params.add(Option<Molecule>("System"));
    
    // the initial AO basis set, of type AOBasisSet
    params.add(Option<AOBasisSet>("Basis"));
    
    // the energy convergence criteria, of type double
    params.add(Option<double>("Threshold", 10E-6));
    
    // the deriviative order of the SCF energy to compute 
    params.add(Option<int>("Derivative", 0));
}
```

The syntax is `params.add(Option<X>("Y", Z))`, where `X` is the C++ type of the
parameter, `"Y"` is the key that will be used to retrieve the parameter, and `Z`
is the default value. The `Option` class supports other additional arguments
including a description and a callback for bounds checking the value omitted
here for clarity. 


### register_returns_

Since modules are opaque and because modules may satisfy multiple property 
types, the only way for the SDE to know all of the quantities that your module
computes is for your module to also register its returns. This is done *via* the
`register_returns_` function. This function works similar to the 
`register_parameters_` function in that your module is provided an empty 
`Returns` object and is charged with filling it.

For our example SCF module this looks something like:

```
void register_returns_(Returns& returns) {
    returns.add(Return<Tensor>("Energy"));
    returns.add(Return<OrbitalSpace>("MOs"));
}
```

Here the syntax is `returns.add(Return<X>("Y"))`, where `X` is the C++ type
of the return and `"Y"` is the key for the value.

### run_

Conceptually this is the simplest function of the Module API. The `run_` 
function is where you actually implement your module. Regardless of what your
module does, the `run_` function always has the same API: 
`Returns run_(const Parameters&, const Submodules&)`. This API takes a 
`Parameters` instance and a `Submodules` instance (basically a map from 
submodule keys to the actual submodules) and returns a `Returns` instance.

With respect to our SCF module this looks something like:

```
Returns run_(const Parameters& params, const Submodules& submods) {
    auto mol = params.at<Molecule>("System");
    auto bs = params.at<AOBasisSet>("Basis");
    auto mos = //get a guess for the mos
    auto h = //get the core Hamiltonian
    auto egy = 0;
    do{
    
        auto F = submods["FockBuilder"].run_as<FockBuilder>(bs, bs, mos);
        mos = submods["MOBuilder"].run_as<MOBuilder>(F);
        egy = //compute energy
        
    }while(egy >= params.at<double>("Threshold"));
    Returns rv;
    rv.add(Return<Tensor>("Energy",egy));
    rv.add(Return<OrbitalSpace>("MOs",mos));
    return rv; 
}
```

Property Types
--------------

If your module requires a quantity `X` (and your module can/will not make `X` 
itself) there's only two choices: take `X` as input or obtain `X` by calling 
another module. Either way there are API ramifications for users of your module.
Since property types codify the APIs for modules, creating a property type for 
every potential set of input parameters has the potential to lead to a 
very large number of property types. For example, one can imagine the following 
variants of a module for building **J** and **K** for SCF:
 
 ```
auto [J] = JBuilder(mol, bs, bs, mos);
auto [K] = KBuilder(mol, bs, bs, mos);
auto [Ka, Kb] = UKBuilder(mol, bs, bs, mos); 
auto [J, K] = JKBuilder(mol, bs, bs, mos);
auto [J, Ka, Kb] = UJKBuilder(mol, bs, bs, mos);
auto [J] = DFJBuilder(mol, bs, bs, fitting, mos);
auto [K] = DFKBuilder(mol, bs, bs, fitting, mos);
...
```

As you can see there's a combinatorial explosion of possible property types
obtained by considering whether we want **J**, **K**, or both, whether we have a
closed shell-system or an unrestricted system, whether we have a fitting basis
or not, *etc.* The aforementioned module system is such that some of these
property types are intrinsically convertible among each other (*e.g.*, the
property type `JKBuilder` would automatically be usable as a `JBuilder` or a
`KBuilder`, albeit at additional cost since it would build both matrices).

While it's hard to say upfront how many property types we need, and what the
APIs for those property types should look like, our goal is to come up with some
spanning set of property types for all of electronic structure theory. Ideally 
this set should be as small as possible, while still maintaining some level 
of generality so that the modules can be used in multiple scenarios. This will
no doubt be an iterative process.
