import unittest
from NWChemEx import *

class TestSCF(unittest.TestCase):

    def setUp(self):
        self.mm = sde.ModuleManager()
        NWChemEx.load_modules(self.mm)
        self.molecule = libchemist.MoleculeManager().at("water")
        self.basis = libchemist.apply_basis("sto-3g", self.molecule)
        self.mm.at("SCF").change_input("Basis Set", self.basis)

        area, volume = self.mm.run_as["PrismVolume"]("Prism", [1.23, 4.56, 7.89])

    def test_scf_energy(self):
        ref = -74.94208006260429
        self.canonical_mos = property_types.type.canonical_mos["double"]
        self.pt_type = property_types.ReferenceWavefunction["double", self.canonical_mos]
        E, self.C = self.mm.run_as[self.py_type]("SCFDIIS", self.molecule, self.basis)
        self.assertAlmostEqual(ref, E, places=8)


    def test_mp2_energy(self):
        ref = 
        self.mp2_pt = property_types.CorrelationEnergy["double", self.canonical_mos]
        E_MP2 = self.mm.run_as[self.mp2_pt]("SCFDIIS", self.molecule, self.basis, self.C)
        self.assertAlmostEqual(ref, E_mp2, places=8)


if __name__ == '__main__':
    unittest.main(verbosity=2)
