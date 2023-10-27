#
# Copyright 2023 NWChemEx-Project
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
#

import pluginplay
import chemist
import simde
import nwchemex
import unittest
import time

# Create H2O molecule and the chemical system
a_O1 = chemist.Atom("O", 8, 0.0, 0.0, -0.143222342980786, 0.0)
a_H1 =  chemist.Atom("H", 1, 0.0, 1.638033502034240, 1.136556880358410, 0.0)
a_H2 = chemist.Atom("H", 1, 0.0, -1.638033502034240, 1.136556880358410, 0.0) 
mol_h2o = chemist.Molecule()
mol_h2o.push_back(a_O1)
mol_h2o.push_back(a_H1)
mol_h2o.push_back(a_H2)
chem_sys_h2o = chemist.ChemicalSystem(mol_h2o)

# Create H2 molecule and the chemical system
a_H1 = chemist.Atom("H", 1, 0.0, 0.0, 0.0, 0.0)
a_H2 = chemist.Atom("H", 1, 0.0, 0.0, 0.0, 1.6818473865225443)
mol_h2 = chemist.Molecule()
mol_h2.push_back(a_H1)
mol_h2.push_back(a_H2)
chem_sys_h2 = chemist.ChemicalSystem(mol_h2)

class TestSCF(unittest.TestCase):
 
    def test_scf(self):
        start_time = time.time()
        # Load NWChemEx modules
        mm = pluginplay.ModuleManager()
        nwchemex.load_modules(mm)
        
        # Create the basis set and the AO space
        basis_pt = simde.MolecularBasisSet()
        bs = mm.at("sto-3g").run_as(basis_pt, mol_h2o)
        aos = chemist.AOSpaceD(bs)
        
        # Run the SCF calculation
        aoenergy_pt = simde.AOEnergy()
        energy = mm.at('SCF Energy').run_as(aoenergy_pt, aos, chem_sys_h2o)
        end_time = time.time()
        self.assertAlmostEqual(energy, -74.94208008267056, 8)
        print("SCF Energy with core guess: ", energy, " Time: ", end_time - start_time, " seconds")

    def test_scf_with_sad_guess(self):
        start_time = time.time()
        mm = pluginplay.ModuleManager()
        # Load NWChemEx modules
        nwchemex.load_modules(mm)
        
        # Create the basis set and the AO space
        basis_pt = simde.MolecularBasisSet()
        bs = mm.at("sto-3g").run_as(basis_pt, mol_h2o)
        aos = chemist.AOSpaceD(bs)
        
        # Use SAD guess
        mm.change_submod("SADDensity", "Atomic Density", "sto-3g atomic dm")
        mm.change_submod("SCF Wavefunction", "Guess", "SADGuess")
        
        # Run the SCF calculation
        aoenergy_pt = simde.AOEnergy()
        energy = mm.at('SCF Energy').run_as(aoenergy_pt, aos, chem_sys_h2o)
        end_time = time.time()
        self.assertAlmostEqual(energy, -74.94208008267056, 8)
        print("SCF Energy with SAD guess: ", energy, " Time: ", end_time - start_time, " seconds")

    def test_density_based_scf(self):
        start_time = time.time()
        # Load NWChemEx modules
        mm = pluginplay.ModuleManager()
        nwchemex.load_modules(mm)
        
        # Create the basis set and the AO space
        basis_pt = simde.MolecularBasisSet()
        bs = mm.at("sto-3g").run_as(basis_pt, mol_h2o)
        aos = chemist.AOSpaceD(bs)
        
        # Run the SCF calculation    
        aoenergy_pt = simde.AOEnergy()
        energy = mm.at('SCF Energy From Density').run_as(aoenergy_pt, aos, chem_sys_h2o)
        end_time = time.time()
        self.assertAlmostEqual(energy, -74.94208008267056, 8)
        print("SCF Energy from density with core guess: ", energy, " Time: ", end_time - start_time, " seconds")

    def test_density_based_scf_with_sad_guess(self):
        start_time = time.time()
        # Load NWChemEx modules
        mm = pluginplay.ModuleManager()
        nwchemex.load_modules(mm)
        
        # Create the basis set and the AO space
        basis_pt = simde.MolecularBasisSet()
        bs = mm.at("sto-3g").run_as(basis_pt, mol_h2o)
        aos = chemist.AOSpaceD(bs)
        
        # Use SAD guess
        mm.change_submod("SADDensity", "Atomic Density", "sto-3g atomic dm")
        mm.change_submod("SCF Density Driver", "Guess", "SADGuess");
        
        # Run the SCF calculation
        aoenergy_pt = simde.AOEnergy()
        energy = mm.at('SCF Energy From Density').run_as(aoenergy_pt, aos, chem_sys_h2o)
        end_time = time.time()
        self.assertAlmostEqual(energy, -74.94208008267056, 8)
        print("SCF Energy from density with SAD guess: ", energy, " Time: ", end_time - start_time, " seconds")

    def test_df_scf(self):
        start_time = time.time()
        # Load NWChemEx modules
        mm = pluginplay.ModuleManager()
        nwchemex.load_modules(mm)
        
        # Create the basis set and the AO space
        basis_pt = simde.MolecularBasisSet()
        bs = mm.at("cc-pvtz").run_as(basis_pt, mol_h2)
        aux_bs = mm.at("cc-pvtz-jkfit").run_as(basis_pt, mol_h2)
        aos = chemist.AOSpaceD(bs)
        aux_aos = chemist.AOSpaceD(aux_bs)
        
        # Apply auxiliary set and change J and K build
        mm.change_input("DFJK", "Fitting Basis", aux_aos)
        mm.change_submod("Fock Matrix", "J Builder", "DFJK")
        mm.change_submod("Fock Matrix", "K Builder", "DFJK")
        
        # Run the SCF calculation
        aoenergy_pt = simde.AOEnergy()
        energy = mm.at("SCF Energy").run_as(aoenergy_pt, aos, chem_sys_h2)
        end_time = time.time()
        self.assertAlmostEqual(energy, -1.1201712380602133, 8)
        print("DF-SCF Energy: ", energy, " Time: ", end_time - start_time, " seconds")