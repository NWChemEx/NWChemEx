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

import unittest
from nwchemex import *

unittest.TestLoader.sortTestMethodsUsing = None

from ctypes import c_int
world = TA.initialize(c_int(0),c_int(0),True)

class NWChemExTestCase(unittest.TestCase):

    def test_scf_energy(self):
        ref_scf = -74.94208005807283  

        name  = mokup.molecule.h2o
        basis = mokup.basis_set.sto3g
        aos   = mokup.get_bases(name, basis)
        H     = mokup.hamiltonian(name)
        H_e   = simde.type.els_hamiltonian(H)

        mm = pluginplay.ModuleManager()
        nwchemex.load_modules(mm)
        mod = mm.at("SCF Driver")

        [phi0] = mod.run_as[simde.CanonicalReference](H_e, aos)
        [E]    = mm.at("Total Energy").run_as[simde.TotalCanonicalEnergy](phi0, H, phi0)

        print("Total SCF/STO-3G Energy: ", E)
        self.assertAlmostEqual(ref_scf, E, places=8)

    # def test_mp2_energy(self):
    #     ref_mp2 = -0.04915043599533923

    #     name  = mokup.molecule.h2o
    #     basis = mokup.basis_set.sto3g
    #     aos   = mokup.get_bases(name, basis)
    #     H     = mokup.hamiltonian(name)
    #     H_e   = simde.type.els_hamiltonian(H)
     
    #     mm = pluginplay.ModuleManager()
    #     nwchemex.load_modules(mm)
    #     scf_wf_mod = mm.at("SCF Driver")
    #     mp1_wf_mod = mm.at("MP1 Wavefunction")

    #     scf_wf_pt = simde.CanonicalReference
    #     mp1_wf_pt = simde.CanonicalManyBodyWf
    #     mp2_e_pt  = simde.CanonicalCorrelationEnergy

    #     [scf_wf]   = scf_wf_mod.run_as[scf_wf_pt](H_e, aos)
    #     [mp1_wf]   = mp1_wf_mod.run_as[mp1_wf_pt](H_e, scf_wf)
    #     [E_MP2]    = mm.run_as[mp2_e_pt]("MP2", scf_wf, H_e, mp1_wf)

    #     print("MP2/STO-3G Correlation energy = ", E_MP2)
    #     self.assertAlmostEqual(ref_mp2, E_MP2, places=8)

    # def test_f12_mp2_energy(self):
    #     ref_f12_mp2 = -0.03401604283558277

    #     name   = mokup.molecule.h2
    #     basis  = mokup.basis_set.ccpvdz
    #     aux_bs = mokup.basis_set.ccpvdzf12optri
    #     aos    = mokup.get_bases(name, basis)
    #     aux    = mokup.get_bases(name, aux_bs)
    #     H      = mokup.hamiltonian(name)
    #     H_e    = simde.type.els_hamiltonian(H)
     
    #     mm = pluginplay.ModuleManager()
    #     nwchemex.load_modules(mm)
    #     mm.change_input("CABS", "F12 fitting basis", aux);
    #     scf_wf_mod = mm.at("SCF Driver")
    #     mp1_wf_mod = mm.at("MP1 Wavefunction")
    #     mp2_f12_e_mod = mm.at("Dense MP2-F12")
     
    #     scf_wf_pt    = simde.CanonicalReference
    #     mp1_wf_pt    = simde.CanonicalManyBodyWf
    #     mp2_f12_e_pt = simde.CanonicalCorrelationEnergy

    #     [scf_wf] = scf_wf_mod.run_as[scf_wf_pt](H_e, aos);
    #     [mp1_wf] = mp1_wf_mod.run_as[mp1_wf_pt](H_e, scf_wf);
    #     [E_F12]  = mp2_f12_e_mod.run_as[mp2_f12_e_pt](scf_wf, H_e, mp1_wf);
  
    #     print("MP2-F12/STO-3G Correlation energy = ", E_F12)
    #     self.assertAlmostEqual(ref_f12_mp2, E_F12, places=8)

if __name__ == '__main__':
    unittest.main(verbosity=2)
