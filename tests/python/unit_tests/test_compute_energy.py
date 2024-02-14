import unittest
from nwchemex import compute_energy, load_modules
from pluginplay import ModuleBase, ModuleManager
from simde import TotalEnergy
from chemist import Atom, Molecule, ChemicalSystem

class DummyEnergyModule(ModuleBase):

    def __init__(self):
        ModuleBase.__init__(self)
        self.satisfies_property_type(TotalEnergy())
        self.add_input('basis set')

    def run_(self, inputs, submods):
        rv = self.results()
        return TotalEnergy().wrap_results(rv, -3.14)


class TestComputeEnergy(unittest.TestCase):

    def test_with_string_molecule(self):
        e = compute_energy('water', 'NWChem : SCF', 'sto-3g')
        self.assertAlmostEqual(e, -74.942080058523, places=5)

    def test_with_non_string_molecule(self):
        h2 = Molecule()
        h2.push_back(Atom('H', 1, 1.0079, 0.0, 0.0, 0.0))
        h2.push_back(Atom('H', 1, 1.0079, 0.0, 0.0, 0.98))

        e = compute_energy(ChemicalSystem(h2), 'NWChem : SCF', 'sto-3g')
        self.assertAlmostEqual(e, -1.058335676822, places=5)

    def test_with_mm(self):
        """ Can use user-supplied ModuleManager

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
        e = compute_energy(mol, 'Dummy Energy', 'sto-3g', mm)
        self.assertAlmostEqual(e, -3.14, places=5)
