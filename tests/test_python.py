import unittest
from PyNWChemEx import *

unittest.TestLoader.sortTestMethodsUsing = None
world = TA.initialize(c_int(0),c_int(0),True)

class NWChemExTestCase(unittest.TestCase):

    def test_scf_energy(self):
        ref_scf = -74.9420800576956339

        mm = sde.ModuleManager()
        nwx.load_modules(mm)
        molecule = libchemist.MoleculeManager().at("water")
        basis = libchemist.apply_basis("sto-3g", molecule)
        canonical_mos = property_types.type.canonical_mos["double"]
        pt_type = property_types.ReferenceWavefunction["double", canonical_mos]

        E, C = mm.run_as[pt_type]("SCFDIIS", molecule, basis)
        self.assertAlmostEqual(ref_scf, E, places=8)

    def test_mp2_energy(self):
        ref_scf = -75.9897958417729
        ref_mp2 = -0.21434765347797086

        mm = sde.ModuleManager()
        nwx.load_modules(mm)
        molecule = libchemist.MoleculeManager().at("water")
        basis = libchemist.apply_basis("cc-pvdz", molecule)
        canonical_mos = property_types.type.canonical_mos["double"]
        pt_type = property_types.ReferenceWavefunction["double", canonical_mos]
        mp2_pt = property_types.CorrelationEnergy["double", canonical_mos]

        E, C = mm.run_as[pt_type]("SCFDIIS", molecule, basis)
        self.assertAlmostEqual(ref_scf, E, places=8)

        E_MP2 = mm.run_as[mp2_pt]("MP2", molecule, basis, C)
        self.assertAlmostEqual(ref_mp2, E_MP2[0], places=8)

    def test_dlpno_mp2_energy(self):
        ref_scf = -75.9897958417729
        ref_dlpno_mp2 = -0.21434765347797086
 
        mm = sde.ModuleManager()
        nwx.load_modules(mm)
        O   = libchemist.Atom("O", array([0.0, -0.07579039945857, 0.0]), libchemist.Atom.AtomicNumber(8))
        H_1 = libchemist.Atom("H", array([0.86681456860648, 0.60144316994806, 0.0]), libchemist.Atom.AtomicNumber(1))
        H_2 = libchemist.Atom("H", array([-0.86681456860648, 0.60144316994806, 0.0]), libchemist.Atom.AtomicNumber(1))
        molecule = libchemist.Molecule(O, H_1, H_2, libchemist.Molecule.Charge(0), libchemist.Molecule.Multiplicity(1))
        basis = libchemist.apply_basis("cc-pvdz", molecule)
        canonical_mos = property_types.type.canonical_mos["double"]
        pt_type = property_types.ReferenceWavefunction["double", canonical_mos]
        orthogonal_mos = property_types.type.orthogonal_orbs["double"]
        dlpno_pt = property_types.CorrelationEnergy["double", orthogonal_mos]
 
        E, C = mm.run_as[pt_type]("SCFDIIS", molecule, basis)
        self.assertAlmostEqual(ref_scf, E, places=8)
  
        orb_map = property_types.type.orbital_map[orthogonal_mos]({"Occupied": C.at("Occupied")})
        E_MP2_DLPNO= mm.run_as[dlpno_pt]("DLPNO", molecule, basis, orb_map)
        self.assertAlmostEqual(ref_dlpno_mp2, E, places=8)

if __name__ == '__main__':
    unittest.main(verbosity=2)
