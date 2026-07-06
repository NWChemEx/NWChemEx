# Copyright 2024 NWChemEx Community
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

import chemist
import pluginplay
import simde

from .load_modules import load_modules


def compute_energy(mol, method, basis, mm=None):
    """A simplified API for computing the energy of a system more aligned with
    traditional experience.

    TODO: This should probably use meta modules/driver modules from PluginPlay
    once they are implemented.
    """

    if not mm:
        mm = pluginplay.ModuleManager()
        load_modules(mm)

    if isinstance(mol, str):
        mol = mm.run_as(simde.MoleculeFromString(), "NWX Molecules", mol)
        mol = chemist.ChemicalSystem(mol)

    # Assume TotalEnergy PT for now
    mm.change_input(method, "basis set", basis)

    return mm.run_as(simde.TotalEnergy(), method, mol)
