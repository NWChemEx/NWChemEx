import unittest
from NWChemEx import *

class TestSCF(unittest.TestCase):

    def setUp(self):
        self.mm = NWChemEx.default_mm()
        self.molecule = LibChemist.MoleculeManager().at("water")
        self.basis = LibChemist.apply_basis("sto-3g", self.molecule)

    def test_rhf_energy(self):
        ref = -74.94208006260429
        egy = scf.rhf_energy(self.mm, self.molecule, self.basis)
        self.assertAlmostEqual(ref, egy, places=8)

if __name__ == '__main__':
    unittest.main(verbosity=2)
