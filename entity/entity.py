class Entity:
    """
    A class that represents an individual in the simulation. Coordinates function between biology and decision centers, as well as with the actual simulation
    """
    def __init__(self):
        #self._id = GenerateID() # Replace with desired Id generation tool
        self._biology   = None
        self._brain     = None
        self._location  = None

    # General setters and getters
    
    def set_biology(self, biology):
        """
        Sets the organisms biology container. Houses the genetic code for the body as well as the organs and chemicals
        """
        ################################
        # Uncomment this section once certain of method. I imagine the id of the associated biology, or the actual object
        #if not isinstance(biology, str):
        #    return False
        ################################
        self._biology = biology

    def set_brain(self, brain):
        """
        Sets the organisms decision center.
        """
        ################################
        # Uncomment this section once certain of method. I imagine the id of the associated brain obj
        #if not isinstance(biology, str):
        #    return False
        ################################
        self._brain = brain

    def set_location(self, location):
        """
        Sets the organisms present location
        """
        ################################
        # Uncomment this section once certain of method. I imagine the id of the associated cell in the sim
        #if not isinstance(biology, str):
        #    return False
        ################################
        self._location = location

    def get_id(self):
        """
        Returns the entities id
        """
        return self._id
        
    def get_biology(self):
        """
        Returns the organisms biology.
        """
        return self._biology
    
    def get_brain(self):
        """
        Returns the organisms brain.
        """
        return self._brain

    def get_location(self):
        """
        Returns the organisms location
        """
        return self._location

    # Brain and biology related functions

    def brain_get_decision(self):
        """
        Calls upon the brain to make a decision as to what to do.
        """
        return self._brain.make_decision()

    def biology_add_chemical(self, chem, amount):
        """
        Adds a quantity of a given chemical to the biology
        :param chem: The id of the chemical
        :param amount: the amount to be added
        """
        self._biology.add_chem(chem, amount)

    def biology_rem_chemical(self, chem, amount):
        """
        Removes a quantity of a given chemical from the biology.
        :param chem: The id of the chemical
        :param amount: the amount to be removed
        """
        self._biology.add_chem(chem, amount * -1)

    def biology_add_energy(self, amount):
        """
        Adds a quantity of energy to the biology
        :param amount: the amount to be added
        """
        self._biology.add_energy(amount)

    def biology_rem_energy(self, amount):
        """
        Removes a quantity of energy from the biology.
        :param amount: the amount to be removed
        """
        self._biology.add_energy(amount * -1)

    def biology_add_health(self, amount):
        """
        Adds a quantity of health to the biology
        :param amount: the amount to be added
        """
        self._biology.add_health(amount)

    def biology_rem_health(self, amount):
        """
        Removes a quantity of health from the biology.
        :param amount: the amount to be removed
        """
        self._biology.add_health(amount * -1)

    def biology_check_death(self):
        """
        Checks if the individual should have died.
        :return: True if the organism should die, false otherwise
        """
        if self._biology.check_death():
            return True
        return False

    def update_biology(self):
        """
        Requests the biology to update itself for a tick.
        """
        self._biology.update()


    # Alias methods
    def __getattr__(self, name):
        """
        Alias get methods as attributes of the object. Prevent reassignment of private attributes.
        For example, calling Entity.id instead of Entity.get_id()
        """
        if name == "id" or name == "ID":
            return self.get_id()
        if name == "brain" or name == "decision_center":
            return self.get_brain()
        if name == "body" or name == "biology":
            return self.get_biology()
        if name == 'dead' or name == "death":
            return self.biology_check_death()
        if name == 'cell' or name == "location":
            return self.get_location()
