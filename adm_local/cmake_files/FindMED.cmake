#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

SET(MED_CXXFLAGS -I${MED_ROOT_DIR}/include/salome)

FIND_LIBRARY(interpkernel interpkernel ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(medmem medmem ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(med_V2_1 med_V2_1 ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(MEDWrapperBase MEDWrapperBase ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(MEDWrapper MEDWrapper ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(MEDWrapper_V2_1 MEDWrapper_V2_1 ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(MEDWrapper_V2_2 MEDWrapper_V2_2 ${MED_ROOT_DIR}/lib/salome)
FIND_LIBRARY(SalomeIDLMED SalomeIDLMED ${MED_ROOT_DIR}/lib/salome)
