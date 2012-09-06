
# This file is a basic demo of what can be done using the proxy
# pattern.

# See the proxy pattern to manage the 
class FieldProxy:
    def __init__( self, field ):
        self.__field = field
        
    def __getattr__( self, name ):
        return getattr( self.__field, name ) 

    def __add__(self, otherFieldProxy):
        print "FieldProxy - addition"
        rfield = self.__field + otherFieldProxy.__field
        return FieldProxy(rfield)

    def __mul__(self, otherFieldProxy):
        print "FieldProxy - multiplication"
        rfield = self.__field * otherFieldProxy.__field
        return FieldProxy(rfield)

    def getField(self):
        return self.__field


class SALOMEMED_FieldProxy:
    pass

#
# ===================================================================
# use case functions
# ===================================================================
#
class Field:
    def __init__( self, value ):
        self.__value = value

    def __add__(self, otherField):
        print "Field - addition"
        result = self.__value + otherField.__value
        return Field(result)

    def __mul__(self, otherField):
        print "Field - multiplication"
        result = self.__value * otherField.__value
        return Field(result)

    def __str__(self):
        return str(self.__value)
    
    def view(self):
        print self.__value
        
        

def TEST_fieldproxy_numeric():
    a=4
    b=5
    fpa=FieldProxy(a)
    fpb=FieldProxy(b)

    fpr = fpa + fpb
    print fpr.getField()

    fpr = fpa * fpb
    print fpr.getField()
    

def TEST_fieldproxy_object():
    a=Field(4)
    b=Field(5)
    fpa=FieldProxy(a)
    fpb=FieldProxy(b)

    # We can invoke the addition
    fpr = fpa + fpb
    # Then print the result (thank to __str__())
    print fpr.getField()
    # And even use the Field class function throw the proxy
    fpr.view()

    fpr = fpa * fpb
    fpr.view()

    

    
# ===================================================================
if __name__ == "__main__":
    TEST_fieldproxy_numeric()
    TEST_fieldproxy_object()






