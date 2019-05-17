import unittest
from NWChemEx import *

class TestSCF(unittest.TestCase):

    def setUp(self):
        self.mm = NWChemEx.default_mm()
        self.molecule = libchemist.MoleculeManager().at("water")
        self.basis = libchemist.apply_basis("sto-3g", self.molecule)
        self.mm.at("SCF").change_input("Basis Set", self.basis)

    def test_rhf_energy(self):
        ref = -74.94208006260429
        egy = scf.rhf_energy(self.mm, self.molecule)
        self.assertAlmostEqual(ref, egy, places=8)

    def test_diis_rhf_energy(self):
        ref = -74.94208006260429
        diisSCF = self.mm.at("SCF").unlocked_copy()
        NWChemEx.change_module(self.mm, diisSCF, "F Builder", "FockDIIS")
        egy = scf.rhf_energy(diisSCF, self.molecule)
        self.assertAlmostEqual(ref, egy, places=8)        


if __name__ == '__main__':
    unittest.main(verbosity=2)
