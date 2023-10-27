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

class TestSCF(unittest.TestCase):

    def test_scf(self):
        start_time = time.time()
        mm = pluginplay.ModuleManager()
        nwchemex.load_modules(mm)
        mod = mm.at('SCF Energy')
        a_O1 = chemist.Atom("O", 8, 0.0, 0.0, -0.143222342980786, 0.0)
        a_H1 =  chemist.Atom("H", 1, 0.0, 1.638033502034240, 1.136556880358410, 0.0)
        a_H2 = chemist.Atom("H", 1, 0.0, -1.638033502034240, 1.136556880358410, 0.0)
        m1 = chemist.Molecule()
        m1.push_back(a_O1)
        m1.push_back(a_H1)
        m1.push_back(a_H2)
        aoenergy_pt = simde.AOEnergy()
        basis_pt = simde.MolecularBasisSet()
        aos = mm.at("sto-3g").run_as(basis_pt, m1)
        aos = chemist.AOSpaceD(aos)
        cs1 = chemist.ChemicalSystem(m1)
        energy = mod.run_as(aoenergy_pt, aos, cs1)
        end_time = time.time()
        self.assertAlmostEqual(energy, -74.94208008267056, 8)
        print("SCF Energy: ", energy, " Time: ", end_time - start_time, " seconds")

    def test_scf_with_sad_guess(self):
        start_time = time.time()
        mm = pluginplay.ModuleManager()
        nwchemex.load_modules(mm)
        mod = mm.at('SCF Energy')
        a_O1 = chemist.Atom("O", 8, 0.0, 0.0, -0.143222342980786, 0.0)
        a_H1 =  chemist.Atom("H", 1, 0.0, 1.638033502034240, 1.136556880358410, 0.0)
        a_H2 = chemist.Atom("H", 1, 0.0, -1.638033502034240, 1.136556880358410, 0.0)
        m1 = chemist.Molecule()
        m1.push_back(a_O1)
        m1.push_back(a_H1)
        m1.push_back(a_H2)
        aoenergy_pt = simde.AOEnergy()
        basis_pt = simde.MolecularBasisSet()
        aos = mm.at("sto-3g").run_as(basis_pt, m1)
        aos = chemist.AOSpaceD(aos)
        cs1 = chemist.ChemicalSystem(m1)
        mm.change_submod("SADDensity", "Atomic Density", "sto-3g atomic dm")
        mm.change_submod("SCF Wavefunction", "Guess", "SADGuess")
        energy = mod.run_as(aoenergy_pt, aos, cs1)
        end_time = time.time()
        self.assertAlmostEqual(energy, -74.94208008267056, 8)
        print("SCF Energy: ", energy, " Time: ", end_time - start_time, " seconds")

    def test_density_based_scf(self):
        start_time = time.time()
        mm = pluginplay.ModuleManager()
        nwchemex.load_modules(mm)
        mod = mm.at('SCF Energy From Density')
        a_O1 = chemist.Atom("O", 8, 0.0, 0.0, -0.143222342980786, 0.0)
        a_H1 =  chemist.Atom("H", 1, 0.0, 1.638033502034240, 1.136556880358410, 0.0)
        a_H2 = chemist.Atom("H", 1, 0.0, -1.638033502034240, 1.136556880358410, 0.0)
        m1 = chemist.Molecule()
        m1.push_back(a_O1)
        m1.push_back(a_H1)
        m1.push_back(a_H2)
        aoenergy_pt = simde.AOEnergy()
        basis_pt = simde.MolecularBasisSet()
        aos = mm.at("sto-3g").run_as(basis_pt, m1)
        aos = chemist.AOSpaceD(aos)
        cs1 = chemist.ChemicalSystem(m1)
        energy = mod.run_as(aoenergy_pt, aos, cs1)
        end_time = time.time()
        self.assertAlmostEqual(energy, -74.94208008267056, 8)
        print("SCF Energy: ", energy, " Time: ", end_time - start_time, " seconds")

    def test_scf_with_sad_guess(self):
        start_time = time.time()
        mm = pluginplay.ModuleManager()
        nwchemex.load_modules(mm)
        mod = mm.at('SCF Energy From Density')
        a_O1 = chemist.Atom("O", 8, 0.0, 0.0, -0.143222342980786, 0.0)
        a_H1 =  chemist.Atom("H", 1, 0.0, 1.638033502034240, 1.136556880358410, 0.0)
        a_H2 = chemist.Atom("H", 1, 0.0, -1.638033502034240, 1.136556880358410, 0.0)
        m1 = chemist.Molecule()
        m1.push_back(a_O1)
        m1.push_back(a_H1)
        m1.push_back(a_H2)
        aoenergy_pt = simde.AOEnergy()
        basis_pt = simde.MolecularBasisSet()
        aos = mm.at("sto-3g").run_as(basis_pt, m1)
        aos = chemist.AOSpaceD(aos)
        cs1 = chemist.ChemicalSystem(m1)
        mm.change_submod("SADDensity", "Atomic Density", "sto-3g atomic dm")
        mm.change_submod("SCF Wavefunction", "Guess", "SADGuess")
        energy = mod.run_as(aoenergy_pt, aos, cs1)
        end_time = time.time()
        self.assertAlmostEqual(energy, -74.94208008267056, 8)
        print("SCF Energy: ", energy, " Time: ", end_time - start_time, " seconds")
