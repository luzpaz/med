#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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

SET(MED2HOME $ENV{MED2HOME})
IF(NOT MED2HOME)
  FIND_PROGRAM(MDUMP mdump)
  SET(MED2HOME ${MDUMP})
  GET_FILENAME_COMPONENT(MED2HOME ${MED2HOME} PATH)
  GET_FILENAME_COMPONENT(MED2HOME ${MED2HOME} PATH)
ENDIF(NOT MED2HOME)

FIND_PATH(MED2_INCLUDE_DIR med.h ${MED2HOME}/include)
FIND_LIBRARY(MEDC_LIB medC ${MED2HOME}/lib)
FIND_LIBRARY(MED_LIB med ${MED2HOME}/lib)
FIND_LIBRARY(MEDIMPORTCXX_LIB medimportcxx ${MED2HOME}/lib)

SET(MED2_INCLUDES ${HDF5_INCLUDES} -I${MED2_INCLUDE_DIR} -D${MACHINE})
SET(MED2_LIBS ${MEDC_LIB} ${MED_LIB} ${MEDIMPORTCXX_LIB})
