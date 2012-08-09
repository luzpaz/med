#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

import sys, os,signal,string,commands
###import runSalome
###import orbmodule
###import TestKiller

# get SALOME environment :

###args, modules_list, modules_root_dir = runSalome.get_config()
###runSalome.set_env(args, modules_list, modules_root_dir)

# launch CORBA naming server

###clt=orbmodule.client()

# launch CORBA logger server

###myServer=runSalome.LoggerServer(args)
###myServer.run()
###clt.waitLogger("Logger")

# execute Unit Test

command = ['TestMEDMEM']
ret = os.spawnvp(os.P_WAIT, command[0], command)

print "MEDMEM General testing done."
print "Run cases, which leads to Segmentation Fault ..."

command = ['MEDMEMTest_Array_fault']
ret = os.spawnvp(os.P_WAIT, command[0], command)

command = ['MEDMEMTest_Connectivity_fault']
ret = os.spawnvp(os.P_WAIT, command[0], command)

command = ['MEDMEMTest_Coordinate_fault']
ret = os.spawnvp(os.P_WAIT, command[0], command)

command = ['MEDMEMTest_Family_fault']
ret = os.spawnvp(os.P_WAIT, command[0], command)

command = ['MEDMEMTest_Field_fault']
ret = os.spawnvp(os.P_WAIT, command[0], command)

command = ['MEDMEMTest_Grid_fault']
ret = os.spawnvp(os.P_WAIT, command[0], command)

command = ['MEDMEMTest_MeshAndMeshing_fault']
ret = os.spawnvp(os.P_WAIT, command[0], command)

command = ['MEDMEMTest_Support_fault']
ret = os.spawnvp(os.P_WAIT, command[0], command)

command = ['MEDMEMTest_VtkFieldDriver_fault']
ret = os.spawnvp(os.P_WAIT, command[0], command)

command = ['MEDMEMTest_VtkMeshDriver_fault']
ret = os.spawnvp(os.P_WAIT, command[0], command)

# kill Test process

###TestKiller.killProcess(runSalome.process_id)
