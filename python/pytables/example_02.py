# first example on how to fill in a table

from numpy import *
from tables import *

class Particle(IsDescription):
    name      = StringCol(16)   # 16-character String
    idnumber  = Int64Col()      # Signed 64-bit integer
    ADCcount  = UInt16Col()     # Unsigned short integer
    TDCcount  = UInt8Col()      # unsigned byte
    grid_i    = Int32Col()      # 32-bit integer
    grid_j    = Int32Col()      # 32-bit integer
    pressure  = Float32Col()    # float  (single-precision)
    energy    = Float64Col()    # double (double-precision)

h5file = openFile ("tutorial1.h5", mode = "w", title = "Test file")
group = h5file.createGroup ("/", 'detector', 'Detector information')
table = h5file.createTable (group, 'readout', Particle, "Readout example")

# to see what's inside now
# print h5file
# h5file

# The time has come to fill this table with some values. 
# First we will get a pointer to the Row (see The Row class) 
# instance of this table instance:
particle = table.row

# Below is an example of how to write rows:
for i in xrange(10):
    particle['name']  = 'Particle: %6d' % (i)
    particle['TDCcount'] = i % 256
    particle['ADCcount'] = (i * 256) % (1 << 16)
    particle['grid_i'] = i
    particle['grid_j'] = 10 - i
    particle['pressure'] = float(i*i)
    particle['energy'] = float(particle['pressure'] ** 4)
    particle['idnumber'] = i * (2 ** 34)
    # Insert a new particle record
    particle.append()

# write on disk
table.flush ()

table = h5file.root.detector.readout
pressure = [x['pressure'] for x in table.iterrows() if x['TDCcount'] > 3 and 20 <= x['pressure'] < 50]
print pressure

names = [ x['name'] for x in table.where("""(TDCcount > 3) & (20 <= pressure) & (pressure < 50)""") ]
print names

# In order to separate the selected data from the mass of detector data, 
# we will create a new group columns branching off the root group. 
# Afterwards, under this group, we will create two arrays 
# that will contain the selected data. 
# First, we create the group:

gcolumns = h5file.createGroup(h5file.root, "columns", "Pressure and Name")
h5file.createArray (gcolumns, 'pressure', array (pressure), "Pressure column selection")

print h5file

h5file.close ()