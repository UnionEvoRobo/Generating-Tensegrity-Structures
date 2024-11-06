"""DAFASDF"""


class Element:
    """THIS IS A STRUT"""

    ELEM_UNIT_LEN=12
    ELEM_UNIT_RAD=12


    def __init__(self):
        self.top_strings=[]
        self.bot_strings=[]

    def attach_string_to_bottom(self, new_string):
        self.bot_strings.append(new_string)

    def attach_string_to_top(self, new_string):
        self.top_strings.append(new_string)
