import unittest
from nwchemex import *

unittest.TestLoader.sortTestMethodsUsing = None

class NWChemExTestCase(unittest.TestCase):

    def test_scf_energy(self):
        ref_scf = -74.9420800576956339

        name  = mokup.molecule.h2o()
        basis = mokup.basis_set.sto3g()
        aos   = mokup.get_bases(name, bs)
        H     = mokup.hamiltonian(name)
        H_e   = simde.type.els_hamiltonian(H)

        mm = pluginplay.ModuleManager()
        nwchemex.load_modules(mm)
        mod = mm.at("SCF")

        phi0 = mod.run_as[simde.CanonicalReference](H_e, aos)
        E    = mm.at("Total Energy").run_as[simde.TotalCanonicalEnergy](phi0, H, phi0)

        print("Total SCF Energy: ", E)
        self.assertAlmostEqual(ref_scf, E[0], places=8)

    def test_mp2_energy(self):
        ref_scf = -75.9897958417729
        ref_mp2 = -0.21434765347797086

        mm = sde.ModuleManager()
        nwchemex.load_modules(mm)
        molecule = libchemist.MoleculeManager().at("water")
        basis = libchemist.apply_basis("cc-pvdz", molecule)
        canonical_mos = property_types.type.canonical_space_t["double"]
        pt_type = property_types.ReferenceWavefunction["double", canonical_mos]
        mp2_pt = property_types.CorrelationEnergy["double", canonical_mos]

        E, C = mm.run_as[pt_type]("SCFDIIS", molecule, basis)
        self.assertAlmostEqual(ref_scf, E, places=8)

        E_MP2 = mm.run_as[mp2_pt]("MP2", molecule, basis, C)
        print("Correlation energy = ", E_MP2[0])
        print("Canonical MP2 energy = ", E_MP2[0] + E)
        self.assertAlmostEqual(ref_mp2, E_MP2[0], places=8)

    def test_dlpno_mp2_energy(self):
        ref_scf = -74.9420800576957   
        ref_dlpno_mp2 = -0.04914970321193614
  
        mm = sde.ModuleManager()
        nwchemex.load_modules(mm)
  
        O   = libchemist.Atom(AtomName="O", Coordinates=[0.0, -0.1432223429807816, 0.0], AtomicNumber=8)
        H_1 = libchemist.Atom(AtomName="H", Coordinates=[1.6380335020342418, 1.1365568803584036, 0.0], AtomicNumber=1)
        H_2 = libchemist.Atom(AtomName="H", Coordinates=[-1.6380335020342418, 1.1365568803584036, 0.0], AtomicNumber=1)
        molecule = libchemist.Molecule(O, H_1, H_2, Charge=0, Multiplicity=1)
        basis = libchemist.apply_basis("sto-3g", molecule)
  
        canonical_mos = property_types.type.canonical_space_t["double"]
        scf_pt = property_types.ReferenceWavefunction["double", canonical_mos]
        dlpno_pt = mp2.pt.canonical_correlation_energy["double"]
  
        E, C = mm.run_as[scf_pt]("SCFDIIS", molecule, basis)
        self.assertAlmostEqual(ref_scf, E, places=8)
  
        orb_map = property_types.type.orbital_map[canonical_mos]({"Occupied": C.at("Occupied")})
        E_MP2_DLPNO = mm.run_as[dlpno_pt]("DLPNO-MP2", molecule, basis, orb_map)
        print("Correlation energy = ", E_MP2_DLPNO[0])
        print("DLPNO MP2 energy = ", E_MP2_DLPNO[0] + E)
        self.assertAlmostEqual(ref_dlpno_mp2, E_MP2_DLPNO[0], places=8)

if __name__ == '__main__':
    unittest.main(verbosity=2)
