# Copyright (C) 2015  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

SET(COMPONENT_NAME MEDCOUPLING)

ADD_SUBDIRECTORY(INTERP_KERNELTest)
ADD_SUBDIRECTORY(MEDCalculator)
ADD_SUBDIRECTORY(MEDCoupling)
#ADD_SUBDIRECTORY(MEDCouplingCorba)
#ADD_SUBDIRECTORY(MEDCouplingCorba_Swig)
ADD_SUBDIRECTORY(MEDCoupling_Swig)
ADD_SUBDIRECTORY(MEDLoader)
ADD_SUBDIRECTORY(MEDLoader/Swig)
#ADD_SUBDIRECTORY(MEDOP)
ADD_SUBDIRECTORY(MEDPartitioner)
#ADD_SUBDIRECTORY(ParaMEDMEM_Swig)
#ADD_SUBDIRECTORY(ParaMEDMEMTest)
ADD_SUBDIRECTORY(MEDPartitioner_Swig)
ADD_SUBDIRECTORY(RENUMBER_Swig)
