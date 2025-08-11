# Copyright 2024 NWChemEx-Project
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

import numpy as np
import tensorwrapper
from chemist import Atom, ChemicalSystem, Molecule
from nwchemex import compute_energy
from pluginplay import ModuleBase, ModuleManager
from simde import TotalEnergy


class DummyEnergyModule(ModuleBase):
    def __init__(self):
        ModuleBase.__init__(self)
        self.satisfies_property_type(TotalEnergy())
        self.add_input("basis set")

    def run_(self, inputs, submods):
        rv = self.results()
        e = tensorwrapper.Tensor(np.array(-3.14))
        return TotalEnergy().wrap_results(rv, e)


class TestComputeEnergy(unittest.TestCase):
    def test_with_string_molecule(self):
        e = compute_energy("water", "NWChem : SCF", "sto-3g")
        self.assertAlmostEqual(e, np.array(-74.942080058523), places=5)

    def test_with_non_string_molecule(self):
        h2 = Molecule()
        h2.push_back(Atom("H", 1, 1.0079, 0.0, 0.0, 0.0))
        h2.push_back(Atom("H", 1, 1.0079, 0.0, 0.0, 0.98))

        e = compute_energy(ChemicalSystem(h2), "NWChem : SCF", "sto-3g")
        e_np = np.array(e)
        self.assertAlmostEqual(e_np[()], -1.058335676822, places=5)

    def test_with_mm(self):
        """Can use user-supplied ModuleManager

        To ensure compute_energy is not restricted to using NWX's default
        ModuleManger we create one just for this test and ask ``compute_energy``
        to use it. The test will fail if ``compute_energy`` tries to use
        NWX's default ModuleManger (unless the default ModuleManager ever gets
        a module satisfying TotalEnergy loaded under the key
        ``'Dummy Energy'``).
        """

        mm = ModuleManager()
        mm.add_module("Dummy Energy", DummyEnergyModule())
        mol = ChemicalSystem()
        e = compute_energy(mol, "Dummy Energy", "sto-3g", mm)
        e_np = np.array(e)
        self.assertAlmostEqual(e_np[()], -3.14, places=5)
