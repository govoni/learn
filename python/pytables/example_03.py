# reading data from a table

from numpy import *
from tables import *

h5file = openFile ("tutorial1.h5", "a")
print h5file

print '\n---- ---- ---- ---- ---- ---- ---- \n'

for node in h5file:
    print node

print '\n---- ---- ---- ---- ---- ---- ---- \n'

for group in h5file.walkGroups () :
    print group

print '\n---- ---- ---- ---- ---- ---- ---- \n'

for group in h5file.walkGroups ("/") :
    for array in h5file.listNodes (group, classname='Array') :
        print array

print '\n---- ---- ---- ---- ---- ---- ---- \n'

for array in h5file.walkNodes ("/", "Array") :
    print array

print '\n---- ---- ---- ---- ---- ---- ---- \n'

