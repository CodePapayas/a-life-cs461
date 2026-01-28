import unittest
from biology import *

class TestDefaults(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        """
        Initialize the constructor and read the test genome
        """
        print("==================== Brain Testing ===================")
        #cls.guy = Biology(debug=True)


    def setUp(self):
        print(f"\n==================== {self._testMethodName} ====================\n")
        self.guy = Biology(debug=True)

    def test1(self):
        self.assertEqual(list(self.guy.get_efficiencies().values()),[.5,.8,
                                                                     .5,.4,.9,.1,
                                                         .3,.4,.6,.4,.35])

    def test2(self):
        self.assertAlmostEqual(self.guy.eat_energy(.5), .25)
        self.assertEqual(self.guy.get_energy(), 1)

    def test3(self):
        self.assertAlmostEqual(self.guy.drink_water(.4), .32)
        self.assertEqual(self.guy.get_water(), 1)

    def test4(self):
        self.assertAlmostEqual(self.guy.movement_energy_drain(TERRAIN_1),
                               .2 * TERRAIN_ENERGY_COEFFICIENT)
        self.assertAlmostEqual(self.guy.get_energy(),.8)

    def test5(self):
        self.assertAlmostEqual(self.guy.movement_water_drain(TERRAIN_2),
                               .12 * TERRAIN_WATER_COEFFICIENT)
        self.assertEqual(self.guy.get_water(), .88)
if __name__ == '__main__':
    unittest.main()
