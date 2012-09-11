This package contains a set of files that can be used to generate a
SALOME application embedding the XMED module (field manipulation).

The files are created in the directory:

  <installdir>/bin/salome/<module_name>/appligen

Where <installdir> is the installation directory of the XMED
module.

To generate a SALOME application, just change directory to go
where you want to install the SALOME application and type the
following command in a standard shell (the SALOME environment
is not required, all paths are "hard" coded in the script):

  $ <installdir>/bin/salome/<module_name>/appligen/appligen.sh 

This script generates an application in a directory ./appli.
Then type the following command to run a SALOME application
embedding your module:

  $ ./appli/runAppli -k

You may used this appli to test the functions of XMED through the
usage of the GUI module.
