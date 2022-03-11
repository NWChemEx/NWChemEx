import logging
from glob import iglob
from pathlib import Path
import itertools
from typing import List
from ctypes import c_int, c_size_t
import numpy as np
try:
    from nwchemex import chemist, TA, chemcache, simde, nwchemex, pluginplay
    from cppyy.gbl.std import array
except ImportError:
    logging.error('''You need to build nwchemex with -DPYTHON_BINDINGS=ON and the bindings
    to the PYTHONPATH. You can either use `export PYTHONPATH=path_to_bindings` or use `sys.path.append(path_to_bindings)`
    in your Python session.''')
    raise ImportError
import nwxschema as schema


def initialize(debug=False, initialized=False):
    """Initialize NWChemEx runtime
    Parameters
    ----------
    debug : boolean, optional
            Enables verbose output
    Returns
    -------
    world : madness::World object
    """
    if not initialized:
        world = TA.initialize(c_int(0), c_int(0), True)
        initialized = True
        if world.rank() > 0:
            logging.basicConfig(level=logging.ERROR)
    if debug:
        import cppyy
        cppyy.set_debug()
        logging.basicConfig(level=logging.DEBUG)
    return


def get_molecule(symbols, coordinates):
    """Return a chemist.Molecule object for given list of symbols and coordinates
    Parameters
    ----------
    symbols : [str] representing element symbols with n items
    coordinates : [float] or [[float, float,float]] with 3n items
    Returns
    -------
    mol : chemist.Molecule object
    """
    natom = len(symbols)
    pt = chemist.PeriodicTable()
    chemcache.load_elements(pt)
    mol = chemist.Molecule()
    if type(coordinates[0]) == list:
        coordinates = list(itertools.chain(*coordinates))
    for i in range(natom):
        xyz = array([float(x) for x in coordinates[3*i:3*i+3]])
        symbol = symbols[i]
        at = chemist.Atom(symbol, xyz, c_size_t(pt.sym_2_Z(symbol)))
        mol.push_back(at)
    return mol


def get_molecule_from_input(inp):
    """Return a chemist.Molecule object for given list of symbols and coordinates
    Parameters
    ----------
    inp : NWXInput, from nwxschema
    Returns
    -------
    mol : chemist.Molecule object
    """
    return get_molecule(inp.molecule.symbols, inp.molecule.coordinates)


def get_molecule_from_xyz(xyz):
    """Return a chemist.Molecule object for given xyz string
    Parameters
    ----------
    xyz : String in common .xyz format.
    Returns
    -------
    mol : chemist.Molecule object
    """
    pt = chemist.PeriodicTable()
    chemcache.load_elements(pt)
    mol = chemist.Molecule()
    lines = xyz.splitlines()
    natom = int(lines[0])
    symbols = ['']*natom
    coordinates = [[0., 0., 0.]]*natom
    for i, line in enumerate(lines[2:]):
        [s, x, y, z] = line.split()
        symbols[i] = s
        coordinates[3*i:3*i+3] = float(x), float(y), float(z)
    return get_molecule(symbols, coordinates)


def get_symbols(mol):
    """Return the list of symbols for a given chemist.Molecule object
    """
    return [str(mol.at(i).name()) for i in range(mol.size())]


def get_coordinates(mol):
    """Return the coordinates for a given chemist.Molecule object
    """
    return [list(mol.at(i).coords()) for i in range(mol.size())]


def get_nelectrons(mol, charge=0):
    """Return the number of electrons for a given chemist.Molecule object
    """
    ne = 0
    for i in range(mol.size()):
        ne += mol.at(i).Z()
    return ne - charge


def get_input(symbols, coordinates, **kwargs):
    """ Return NWChemEx input as a Python dictionary
    Parameters
    ----------
    symbols : 1-D array of strings representing element symbols with shape(n,)
    coordinates: array of floats representing coordinates can be shape(n,) or shape(3, m)
    kwargs : optional keyword arguments
    Returns
    -------
    inp : dict, representing input schema
    """
    inp = {}
    inp['molecule'] = {'symbols': symbols, 'coordinates': coordinates}
    for key, value in kwargs.items():
        inp['keyword'] = {key: value}
    return inp


def get_hamiltonian(mol, charge=0):
    """Return the molecular Hamiltonian
    Parameters
    ----------
    mol : chemist.Molecule object
    charge : int, optional
    Returns
    -------
    H : chemist.operators.Hamiltonian
    """
    nelectron = get_nelectrons(mol, charge)
    es = chemist.ManyElectrons(nelectron)
    T = chemist.operators.NElectronKinetic(es)
    V = chemist.operators.NElectronNuclearAttraction(es, mol)
    G = chemist.operators.NElectronRepulsion(es)
    Vnn = chemist.operators.NuclearRepulsion(mol)
    H = chemist.operators.Hamiltonian(T, V, G, Vnn)
    return H


def get_total_energy(mol, charge=0, method='SCF', basis='sto-3g', input=None):
    """Return the total energy
    Parameters
    ----------
    mol : chemist.Molecule object
    charge : int, optional
    method : str, optional
    basis : str, optional
    input : NWXinput, optional. 
            Note that coordinates and symbols of input.Molecule is not used.
    Returns
    -------
    E : float
    """
    if input:
        assert(type(input) == schema.NWXInput)
        charge = input.molecule.charge
        method = input.method
        basis = input.basis
    H = get_hamiltonian(mol, charge)
    H_e = simde.type.els_hamiltonian(H)
    basis_name = basis
    aos = nwchemex.apply_basis(basis_name, mol)
    mm = pluginplay.ModuleManager()
    nwchemex.load_modules(mm)
    mod = mm.at(method)
    [phi0] = mod.run_as[simde.CanonicalReference](H_e, aos)
    [E] = mm.at("Total Energy").run_as[simde.TotalCanonicalEnergy](
        phi0, H, phi0)
    return E


def get_total_energy_from_input(inp, mol=None):
    """Return the total energy
    Parameters
    ----------
    input : NWXinput.
    mol : chemist.Molecule, optional 
          If mol is given, coordinates and symbols of input.Molecule will not be used.
    Returns
    -------
    E : float
    """
    assert type(
        inp) == schema.NWXInput, 'Use nwxschema.NWXInput class to create input.'
    if mol is None:
        mol = get_molecule_from_input(inp)
    charge = inp.molecule.charge
    basis = inp.basis
    method = inp.method
    H = get_hamiltonian(mol, charge)
    H_e = simde.type.els_hamiltonian(H)
    basis_name = basis
    aos = nwchemex.apply_basis(basis_name, mol)
    mm = pluginplay.ModuleManager()
    nwchemex.load_modules(mm)
    mod = mm.at(method)
    [phi0] = mod.run_as[simde.CanonicalReference](H_e, aos)
    [E] = mm.at("Total Energy").run_as[simde.TotalCanonicalEnergy](
        phi0, H, phi0)
    return E


def to_optimize(relaxed_coords, *args):
    """ Callable function to pass to scipy optimizer
    relaxed_coords : [float] of size ``n``, 
                      where ``n`` is the number of coordinates to be optimized
    args : positional arguments
           args[0] : [int] of size ``n``,
                    where ``n`` is the number of indices of relaxed_coords
           args[1] : NWXInput
    """
    indices = args[0]
    inp = args[1]
    coords = inp.molecule.coordinates
    symbols = inp.molecule.symbols
    for i, index in enumerate(indices):
        coords[index] = relaxed_coords[i]
    mol = get_molecule(symbols, coords)
    return get_total_energy_from_input(inp)
