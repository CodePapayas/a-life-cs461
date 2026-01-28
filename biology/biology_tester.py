import unittest
from biology import *
#from biology.biology import Biology
#from biology.biology_constants import *

class TestDefaults(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        """
        Initialize the constructor and read the test genome
        """
        print("==================== Brain Testing ===================")
        #cls.guy = Biology(debug=True)


    def setUp(self):
        """
        Setup a new class at each test
        """
        print(f"\n==================== {self._testMethodName} ====================\n")
        print(self._testMethodDoc)
        self.guy = Biology(debug=True)

    def test1(self):
        """
        Test that default genetics loaded in correctly
        """
        self.assertEqual(
            list(self.guy.get_efficiencies().values()),
            [.5,.8,.5,.4,.9,.1,.3,.4,.6,.4,.35])

    def test2(self):
        """
        Test that eating energy returns expected values
        """
        self.assertAlmostEqual(self.guy.eat_energy(.5), .1767, delta=.001)
        self.assertEqual(self.guy.get_energy(), 1)

    def test3(self):
        """
        Test that drinking water returns expected value
        """
        self.assertAlmostEqual(self.guy.drink_water(.4), .32)
        self.assertEqual(self.guy.get_water(), 1)

    def test4(self):
        """
        Test that moving through terrain 1 loses te expected energy amt
        """
        self.assertAlmostEqual(
            self.guy.movement_energy_drain(TERRAIN_1),
            .2 * TERRAIN_ENERGY_COEFFICIENT)
        self.assertAlmostEqual(self.guy.get_energy(),.8)

    def test5(self):
        """
        Test that moving through Terrain 2 reduces water appropriately
        """
        self.assertAlmostEqual(
            self.guy.movement_water_drain(TERRAIN_2),
            .12 * TERRAIN_WATER_COEFFICIENT
            )
        self.assertEqual(self.guy.get_water(), .88)

    def test6(self):
        """
        Test that adding chem 1 affects health as expected
        """
        self.assertAlmostEqual(
            self.guy.add_chemical("Chem 1",.5),
            .2
        )

    def test7(self):
        """
        Test that chem 2 afffects health as expected
        """
        self.assertAlmostEqual(
            self.guy.add_chemical("Chem 2", .4),
            -.16
        )

    def test8(self):
        """
        Test that energy is drained as expected per tick
        """
        self.assertAlmostEqual(
            self.guy.tick_energy_drain(),
            .1485, delta=.001
        )

    def test9(self):
        """
        Test that health is drained appropriately
        """
        self.guy._energy -= .9
        self.assertAlmostEqual(
            self.guy.tick_health_drain(),
            0.16, delta = .001
        )

    def test10(self):
        """
        Test that the organism reports dead apropriately
        """
        self.assertFalse(
            self.guy.check_death()
        )
        self.guy._health -= 1.1
        self.assertTrue(
            self.guy.check_death()
        )
        self.guy._health = 1
        self.guy.health = 0.0001
        self.assertFalse(
            self.guy.check_death()
        )
if __name__ == '__main__':
    unittest.main()
