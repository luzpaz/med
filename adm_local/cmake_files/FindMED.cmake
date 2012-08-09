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

SET(MED_CXXFLAGS -I${MED_ROOT_DIR}/include/salome)

FIND_LIBRARY(interpkernel interpkernel ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(InterpKernelTest InterpKernelTest ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(MEDClientcmodule MEDClientcmodule ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(medcouplingclient medcouplingclient ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(medcouplingcorba medcouplingcorba ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(medcouplingremapper medcouplingremapper ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(medcoupling medcoupling ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(MEDEngine MEDEngine ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(medloader medloader ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(MEDMEMCppTest MEDMEMCppTest ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(MEDMEMImpl MEDMEMImpl ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(medmem medmem ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(MED MED ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(medsplitter medsplitter ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(MEDSPLITTERTest MEDSPLITTERTest ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(med_V2_1 med_V2_1 ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(MEDWrapperBase MEDWrapperBase ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(MEDWrapper MEDWrapper ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(MEDWrapper_V2_1 MEDWrapper_V2_1 ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(MEDWrapper_V2_2 MEDWrapper_V2_2 ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(paramedcouplingcorba paramedcouplingcorba ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(paramedloader paramedloader ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(paramedmemcompo paramedmemcompo ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(paramedmem paramedmem ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(ParaMEDMEMTest ParaMEDMEMTest ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(SalomeIDLMED SalomeIDLMED ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(SalomeIDLMEDTests SalomeIDLMEDTests ${MED_ROOT_DIR}/lib/salome)
