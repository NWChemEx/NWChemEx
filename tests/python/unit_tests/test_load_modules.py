# Copyright 2026 NWChemEx-Project
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

from nwchemex import load_modules
from pluginplay import ModuleManager


class TestLoadModules(unittest.TestCase):
    def test_loads_plugins(self):
        for name in ["chemcache", "friendzone", "integrals", "nux", "scf"]:
            self.assertTrue(self.mm.has_plugin(name), name)

    def test_wires_defaults(self):
        # One default from each plugin SCF gets submodules from
        submods = [
            ("SCF Driver", "Hamiltonian"),
            ("SCF integral driver", "Fundamental matrices"),
            ("SAD guess", "SAD Density"),
        ]
        for module, submod in submods:
            request = self.mm.at(module).submods()[submod]
            self.assertTrue(request.has_module(), (module, submod))

        # NUX gets its atomic data from ChemCache
        submods = self.mm.at("XYZ To Molecule").submods()
        self.assertTrue(submods["Z from symbol"].has_module())

    def setUp(self):
        self.mm = ModuleManager()
        load_modules(self.mm)
