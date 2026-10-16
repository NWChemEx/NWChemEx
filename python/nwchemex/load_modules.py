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

import chemcache
import friendzone
import integrals
import nux
import scf


def load_modules(mm):
    """Loads the NWChemEx plugins

    :param mm: The ModuleManager that the all Modules will be loaded into.
    :type mm: pluginplay.ModuleManager
    """

    mm.load_plugin(chemcache.ChemCache())
    mm.load_plugin(friendzone.FriendZone())
    mm.load_plugin(integrals.Integrals())
    mm.load_plugin(nux.NUX())
    mm.load_plugin(scf.SCF())

    # Wiring between plugins. A plugin's default submodules may come from the
    # other plugins, so this runs only after everything is loaded.
    mm.wire_defaults()
