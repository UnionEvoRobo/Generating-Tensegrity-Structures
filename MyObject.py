class My_Object:
    
    def __init__(self):
        self.modID=None
        self.body=None
        self.geom=None
        self.rc=None
        self.gc=None
        self.bc=None
        self.refcount=None
        
    def set_modID(self, modID):
        self.modID=modID

    def set_body(self, body):
        self.body=body

    def set_geom(self, geom):
        self.geom=geom

    def set_rc(self, rc):
        self.rc=rc

    def set_gc(self, gc):
        self.gc=gc

    def set_bc(self, bc):
        self.bc=bc

    def set_refcount(self, refcount):
        self.refcount=refcount
