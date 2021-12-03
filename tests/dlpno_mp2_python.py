import unittest
from nwchemex import *

unittest.TestLoader.sortTestMethodsUsing = None

from ctypes import c_int
world = TA.initialize(c_int(0),c_int(0),True)


class NWChemExTestCase(unittest.TestCase):

    def test_scf_energy(self):
        ref_dip = 0.0
   
        ham_pt   = simde.SystemHamiltonian
        local_pt = simde.NoncanonicalToLocal

        mm = pluginplay.ModuleManager()
        nwchemex.load_modules(mm)

        # Basic input
        name       = mokup.molecule.h2o
        mol        = mokup.get_molecule(name)
        basis      = mokup.basis_set.sto3g
        nelectrons = 10
        aos        = mokup.get_bases(name, basis)

        # Make the electronic Hamiltonian
        sys = simde.type.chemical_system(mol, nelectrons)
        H   = mm.at("SystemHamiltonian").run_as[ham_pt](sys)
        He  = simde.type.els_hamiltonian(H)

        # Compute SCF wavefunction w/ electronic Hamiltonian
        scf_wf_mod  = mm.at("SCF")
        [ref] = scf_wf_mod.run_as[simde.CanonicalReference](H_e, aos)

        # Unpack the canonical occupied orbitals, and converged Fock operator
        occ   = ref.basis_set().occupied_orbitals()
        f_hat = ref.basis_set().fock_operator()

        # Localize occupied orbitals and make a local wavefunction
        # TODO: Actually localize the orbitals
        lmos  = simde.type.derived_space(occ)
        empty = simde.type.derived_space()
        local_ref = chemist.wavefunction.make_wavefunction(lmos, empty, f_hat)

        # Use the localized wf to make an orbital-dependent LMO/PAO wavefunction
        # N.B. This line bypasses contraction of PAOs to atoms
        local_mod = mm.at("LMO-PAO Wavefunction")
        local_mod.change_submod("L(i->a)", mm.at("L(LMO->LMO) (prescreening)"))
        [sparse_ref] = local_mod.run_as[local_pt](local_ref)

        # Unpack the oribtal-dependent spaces
        lmos_i = sparse_ref.basis_set().occupied_orbitals()
        paos_i = sparse_ref.basis_set().virtual_orbitals()

        # Run dipoles w/ unpacked spaces
        dip_mod     = mm.at("MP2 Dipole")
        [eij] = dip_mod.run_as[simde::SparsePairEnergy](lmos_i, paos_i)
        print("MP2 Dipole: ", eij)
        self.assertAlmostEqual(ref_dip, eij, places=8)

if __name__ == '__main__':
    unittest.main(verbosity=2)
    TA.finalize()
