class Tens_String:

    def __init__ (self, elem1, elem2,num):
        self.elem1=elem1
        self.elem2=elem2
        #self.to_loc=to_loc
        #self.from_loc=from_loc
        self.node_stub=None
        self.rod_number=num

    def set_rod_number(self, label):
        self.node_stub=label

    def get_from(self):
        return self.elem1
    
    def get_to(self):
        return self.elem2