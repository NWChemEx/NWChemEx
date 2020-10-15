import unittest
from PyNWChemEx import *

class TestSCF(unittest.TestCase):

    def setUp(self):
        self.world = TA.initialize(c_int(0),c_int(0),True)
        self.mm = sde.ModuleManager()
        self.mm = nwx.load_modules(mm)
        self.molecule = libchemist.MoleculeManager().at("water")
        self.basis = libchemist.apply_basis("cc-pvdz", self.molecule)

    def test_scf_energy(self):
        ref_scf = -75.9897958417729
        self.canonical_mos = property_types.type.canonical_mos["double"]
        self.pt_type = property_types.ReferenceWavefunction["double", self.canonical_mos]
        E, self.C = self.mm.run_as[self.pt_type]("SCFDIIS", self.molecule, self.basis)
        self.assertAlmostEqual(ref_scf, E, places=8)

    def test_mp2_energy(self):
        ref_mp2 = -0.21434765347797086
        self.mp2_pt = property_types.CorrelationEnergy["double", self.canonical_mos]
        E_MP2 = self.mm.run_as[self.mp2_pt]("SCFDIIS", self.molecule, self.basis, self.C)
        self.assertAlmostEqual(ref_mp2, E_mp2[0], places=8)

    def test_mp2_energy(self):
        ref_scf = -5.588377138718967
        ref_dlpno_mp2 = 0.0
        He_1 = libchemist.Atom("He", array([0.0, 0.0, 0.0]), libchemist.Atom.AtomicNumber(2))
        He_2 = libchemist.Atom("He", array([0.0, 0.0, 2.0]), libchemist.Atom.AtomicNumber(2))
        self.molecule = libchemist.Molecule(He_1, He_2, libchemist.Molecule.Charge(0), libchemist.Molecule.Multiplicity(1))
        self.basis = libchemist.apply_basis("cc-pvdz", self.molecule)
        E, self.C = self.mm.run_as[self.pt_type]("SCFDIIS", self.molecule, self.basis)
        self.assertAlmostEqual(ref_scf, E, places=8)

        self.orthogonal_mos = property_types.type.orthogonal_orbs["double"]
        self.dlpno_pt = property_types.CorrelationEnergy["double", self.orthogonal_mos]
        self.orb_map = property_types.type.orbital_map[orthogonal_mos]({"Occupied": self.C.at("Occupied")})
        E_MP2_DLPNO= self.mm.run_as[self.dlpno_pt]("DLPNO", self.molecule, self.basis, self.orb_map)
        self.assertAlmostEqual(ref_dlpno_mp2, E, places=8)

if __name__ == '__main__':
    unittest.main(verbosity=2)
