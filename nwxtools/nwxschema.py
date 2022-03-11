from dataclasses import dataclass, field, asdict
from typing import List
import json


@dataclass
class Molecule:
    symbols: List[str]
    coordinates: List[float]
    charge: int = 0
    multiplicity: int = 1
    name: str = ''
    natom: int = field(init=False, compare=False, default=0)
    nelectron: int = field(init=False, compare=False, default=0)

    def __post_init__(self):
        self.natom = len(self.symbols)

    def asjson(self):
        return json.dumps(asdict(self), indent=2)


@dataclass
class NWXInput:
    molecule: Molecule = None
    method: str = 'scf'
    basis: str = 'sto-3g'
    schema_name: str = 'NWChemEx schema'
    schema_version: str = '0.1'
    keywords: dict = None

    def asjson(self):
        return json.dumps(asdict(self))


@dataclass
class Provenance:
    version: dict
    host: str
    profile: str


@dataclass
class NWXOutput:
    input: NWXInput
    provenance: Provenance
