import logging
from glob import iglob
from pathlib import Path
import itertools
from typing import List
from ctypes import c_int, c_size_t
import numpy as np
import time
from datetime import timedelta
try:
    from nwchemex import chemist, TA, chemcache, simde, nwchemex, pluginplay
    import cppyy
    from cppyy.gbl.std import array
except ImportError:
    logging.error('''You need to build nwchemex with -DPYTHON_BINDINGS=ON and add the path to bindings
    to the PYTHONPATH. You can either run `export PYTHONPATH=path_to_bindings` before starting Python, 
    or use `sys.path.append(path_to_bindings)` within your Python session.''')
    from glob import glob
    import os
    cwd = os.getcwd()
    logging.info(
        'Searching current directory {} for Python bindings'.format(cwd))
    bindingpaths = glob('**/Python/nwchemex', recursive=True)
    if len(bindingpaths) == 0:
        logging.error('Cannot find Python bindings in {}.'.format(cwd))
    else:
        logging.info('Found python bindings in these paths:')
        for nwxpath in bindingpaths:
            logging.info(os.path.dirname(nwxpath))
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
        fiel = cppyy.__file__
        if world.rank() > 0:
            logging.basicConfig(level=logging.ERROR)
    if debug:
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
    angs2bohr = 1.8897259886
    mol = chemist.Molecule()
    if type(coordinates[0]) == list:
        coordinates = list(itertools.chain(*coordinates))
    for i in range(natom):
        xyz = array([float(x)*angs2bohr for x in coordinates[3*i:3*i+3]])
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


def get_scf_energy(mol, charge=0, basis='sto-3g', max_iter=50, inp=None):
    """Return the SCF energy
    Parameters
    ----------
    mol : chemist.Molecule object
    charge : int, optional
    basis : str, optional
    input : NWXinput, optional.
            Note that coordinates and symbols of input.Molecule is not used.
    Returns
    -------
    E : float
    """
    if inp:
        assert(type(inp) == schema.NWXInput)
        charge = inp.molecule.charge
        basis = inp.basis
        max_iter = inp.scf_max_iter
    H = get_hamiltonian(mol, charge)
    H_e = simde.type.els_hamiltonian(H)
    aos = nwchemex.apply_basis(basis, mol)
    mm = pluginplay.ModuleManager()
    nwchemex.load_modules(mm)
    scf_mod = mm.at('scf')
    scf_mod.change_input("MaxIt", c_size_t(max_iter))
    [scf_wf] = scf_mod.run_as[simde.CanonicalReference](H_e, aos)
    [scf_energy] = mm.at("Total Energy").run_as[simde.TotalCanonicalEnergy](
        scf_wf, H, scf_wf)
    return scf_energy


def get_scf_energy_from_input(nwxinp):
    """Return the total energy
    Parameters
    ----------
    nwxinp : NWXInput.
    Returns
    -------
    scf_energy : float
    """
    assert type(
        nwxinp) == schema.NWXInput, 'Use nwxschema.NWXInput class to create input.'
    mol = get_molecule_from_input(nwxinp)
    return get_scf_energy(mol, inp=nwxinp)


def run_scf(nwxinp):
    """Run NWChemEx SCF module
    Parameters
    ----------
    nwxinp : NWXInput.
    Returns
    -------
    nwxout : NWXOutput
    """
    t0 = time.time()
    timings = {}
    properties = {}
    nwxout = schema.NWXOutput(nwxinp)
    nwxmol = get_molecule_from_input(nwxinp)
    t = time.time()
    scfenergy = get_scf_energy(nwxmol, inp=nwxinp)
    timings['scf_energy'] = timedelta(seconds=time.time()-t)
    properties['scf_total_energy'] = scfenergy
    nwxout.return_value = scfenergy
    nwxout.success = True
    nwxout.properties = properties
    timings['total_time'] = timedelta(seconds=time.time()-t0)
    nwxout.timings = timings
    return nwxout


def get_mp2_energy(mol, charge=0, basis='sto-3g', inp=None):
    """Return the MP2 energy
    Parameters
    ----------
    mol : chemist.Molecule object
    charge : int, optional
    basis : str, optional
    input : NWXinput, optional.
            Note that coordinates and symbols of input.Molecule is not used.
    Returns
    -------
    E : float
    """
    if inp:
        assert(type(inp) == schema.NWXInput)
        charge = inp.molecule.charge
        basis = inp.basis
    H = get_hamiltonian(mol, charge)
    H_e = simde.type.els_hamiltonian(H)
    aos = nwchemex.apply_basis(basis, mol)
    mm = pluginplay.ModuleManager()
    nwchemex.load_modules(mm)
    scf_mod = mm.at('SCF')
    mp1_wf_mod = mm.at("MP1 Wavefunction")
    [scf_wf] = scf_mod.run_as[simde.CanonicalReference](H_e, aos)
    [scf_energy] = mm.at("Total Energy").run_as[simde.TotalCanonicalEnergy](
        scf_wf, H, scf_wf)
    [mp1_wf] = mp1_wf_mod.run_as[simde.CanonicalManyBodyWf](H_e, scf_wf)

    [mp2_corr] = mm.run_as[simde.CanonicalCorrelationEnergy](
        "MP2", scf_wf, H_e, mp1_wf)

    return scf_energy + mp2_corr


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
    return get_scf_energy_from_input(inp)
