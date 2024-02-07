import unittest
from nwchemex import compute_energy

class TestComputeEnergy(unittest.TestCase):

    def test_with_string(self):
        e = compute_energy('water', 'NWChem : SCF', 'sto-3g')
        print(e)
