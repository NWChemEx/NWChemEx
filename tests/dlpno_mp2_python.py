# Copyright 2022 NWChemEx-Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# TODO: Re-enable once sparse map error is solved

# import unittest
# from nwchemex import *

# unittest.TestLoader.sortTestMethodsUsing = None

# from ctypes import c_int
# world = TA.initialize(c_int(0),c_int(0),True)


# class NWChemExTestCase(unittest.TestCase):

#     def test_dlpno_mp2_dipole(self):
#         ref_dip = vector([0.0, -0.007523994711814412, -0.01072856548544765, 
#                          -349.5486858006644, -1.821754826367641e-12, -0.007523994711814411, 
#                          0.0, -8.738757592355654, -209.80179502398897, -7.524616896753292e-29, 
#                          -0.01072856548544765, -8.738757592355652, 0.0, -76.87376972253533, 
#                          -1.3707081148084588e-28, -349.5486858006644, -209.80179502398897, 
#                          -76.87376972253533, 0.0, -4.3532995386659215e-24, -1.821754826367641e-12, 
#                          -7.524616896753292e-29, -1.3707081148084588e-28, -4.3532995386659215e-24, 0.0])
   
#         ham_pt   = simde.SystemHamiltonian
#         local_pt = simde.NoncanonicalToLocal

#         mm = pluginplay.ModuleManager()
#         nwchemex.load_modules(mm)

#         # Basic input
#         name       = mokup.molecule.h2o
#         mol        = mokup.get_molecule(name)
#         basis      = mokup.basis_set.sto3g
#         nelectrons = 10
#         aos        = mokup.get_bases(name, basis)

#         # Make the electronic Hamiltonian
#         sys = simde.type.chemical_system(mol, nelectrons)
#         [H] = mm.at("SystemHamiltonian").run_as[ham_pt](sys)
#         H_e = simde.type.els_hamiltonian(H)

#         # Compute SCF wavefunction w/ electronic Hamiltonian
#         scf_wf_mod  = mm.at("SCF Driver")
#         [ref] = scf_wf_mod.run_as[simde.CanonicalReference](H_e, aos)

#         # Unpack the canonical occupied orbitals, and converged Fock operator
#         occ   = ref.basis_set().occupied_orbitals()
#         f_hat = ref.basis_set().fock_operator()

#         # Localize occupied orbitals and make a local wavefunction
#         # TODO: Actually localize the orbitals
#         lmos  = simde.type.derived_space(occ)
#         empty = simde.type.derived_space()
#         local_ref = chemist.wavefunction.make_wavefunction(lmos, empty, f_hat)

#         # Use the localized wf to make an orbital-dependent LMO/PAO wavefunction
#         # N.B. This line bypasses contraction of PAOs to atoms
#         local_mod = mm.at("LMO-PAO Wavefunction")
#         local_mod.change_submod("L(i->a)", mm.at("L(LMO->LMO) (prescreening)"))
#         [sparse_ref] = local_mod.run_as[local_pt](local_ref)

#         # Unpack the oribtal-dependent spaces
#         lmos_i = sparse_ref.basis_set().occupied_orbitals()
#         paos_i = sparse_ref.basis_set().virtual_orbitals()

#         # Run dipoles w/ unpacked spaces
#         dip_mod     = mm.at("MP2 Dipole")
#         [eij] = dip_mod.run_as[simde.SparsePairEnergy](lmos_i, paos_i)
#         print("MP2 Dipole: ", eij)
#         eij_vec = chemist.tensor.to_vector(eij)
#         [self.assertAlmostEqual(ref_dip[i], eij_vec[i], places=8) for i in range(25)]

# if __name__ == '__main__':
#     unittest.main(verbosity=2)
#     TA.finalize()
