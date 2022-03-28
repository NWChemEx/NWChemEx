from dataclasses import dataclass, field, asdict
from typing import List
import json
import rdtools as rd
import itertools


@dataclass
class Molecule:
    symbols: List[str] = field(default_factory=lambda: [])
    coordinates: List[float] = field(default_factory=lambda: [])
    smiles: str = ''
    xyz: str = ''
    charge: int = 0
    multiplicity: int = 1
    name: str = ''
    natom: int = field(init=False, compare=False, default=0)
    nelectron: int = field(init=False, compare=False, default=0)
    units: str = 'All units are atomic units except coordinates in xyz and sxyz'

    def __post_init__(self):
        if self.smiles:
            rdmol = rd.get_mol_from_smiles(self.smiles)
            rdconf = rdmol.GetConformer()
            self.symbols = [atom.GetSymbol() for atom in rdmol.GetAtoms()]
            self.coordinates = rdconf.GetPositions().flatten()
        if self.xyz:
            lines = self.xyz.splitlines()
            natom = int(lines[0])
            self.symbols = ['']*natom
            self.coordinates = [[0., 0., 0.]]*natom
            for i, line in enumerate(lines[2:]):
                [s, x, y, z] = line.split()
                self.symbols[i] = s
                self.coordinates[3*i:3*i+3] = float(x), float(y), float(z)
        else:
            sxyz = '{}\n\n'.format(len(self.symbols))
            sxyz = ''
            if type(self.coordinates[0]) == list:
                self.coordinates = list(itertools.chain(*self.coordinates))
            i = 0
            for symbol in self.symbols:
                sxyz += '{} {} {} {}\n'.format(symbol,
                                               self.coordinates[i], self.coordinates[i+1], self.coordinates[i+2])
                i += 3
        self.natom = len(self.symbols)
        self.sxyz = sxyz
        self.xyz = '{}\n\n{}'.format(self.natom, sxyz)

    def asjson(self):
        return json.dumps(asdict(self), indent=2)


@dataclass
class NWXInput:
    molecule: Molecule = None
    method: str = 'scf'
    basis: str = 'sto-3g'
    auxbasis: str = ''
    schema_name: str = 'NWChemEx schema'
    schema_version: str = '0.1'
    keywords: dict = None
    scf_max_iter: int = 50
    units: str = 'All units are atomic units except coordinates in molecule.xyz and molecule.sxyz'

    def asjson(self):
        return json.dumps(asdict(self))


@dataclass
class Provenance:
    version: str = "NWChemEx 1.0"
#    host: str =


@ dataclass
class NWXOutput:
    input: NWXInput
    provenance: Provenance = Provenance()
    profile: str = ''
    driver: str = ''
    model: dict = field(default_factory=lambda: {})
    success: bool = False
    properties: dict = field(default_factory=lambda: {})
    keywords: dict = field(default_factory=lambda: {})
    timings: dict = field(default_factory=lambda: {})
    return_result: float = 0.
