EnsureSConsVersion( 1, 2 )
EnsurePythonVersion( 2, 5 )

import os
import sys

sys.path.append('tools')
from sconsProject import SConsProject


class Xml2EPub( SConsProject ):
	'''
	The project compilation object.
	'''



#______________________________________________________________________________#

# Create the object available in all SConscripts
project = Xml2EPub()
Export('project')
Export({'libs':project.libs})

# Load SConscript files
project.begin()
project.SConscript()
project.end()
