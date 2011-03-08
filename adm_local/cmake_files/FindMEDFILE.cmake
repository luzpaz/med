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

# ------

MESSAGE(STATUS "Check for medfile ...")

# ------

SET(MED_ENABLE_MED2 ON)

# ------

SET(MED2HOME $ENV{MED2HOME})
IF(NOT MED2HOME)
  FIND_PROGRAM(MDUMP mdump)
  IF(MDUMP)
    MESSAGE(STATUS "mdump found: ${MDUMP}")
    SET(MED2HOME ${MDUMP})
    GET_FILENAME_COMPONENT(MED2HOME ${MED2HOME} PATH)
    GET_FILENAME_COMPONENT(MED2HOME ${MED2HOME} PATH)
  ELSE(MDUMP)
    MESSAGE(STATUS "mdump not found, try to use MED2HOME environment variable or check your PATH")
    SET(MED_ENABLE_MED2 OFF)
  ENDIF(MDUMP)
ENDIF(NOT MED2HOME)

IF(MED_ENABLE_MED2)
  MESSAGE(STATUS "Searching medfile in ${MED2HOME}")
ENDIF(MED_ENABLE_MED2)

IF(MED_ENABLE_MED2)
  FIND_PATH(MED2_INCLUDE_DIR med.h ${MED2HOME}/include)
  IF(MED2_INCLUDE_DIR)
    SET(MED2_INCLUDES ${HDF5_INCLUDES} -I${MED2_INCLUDE_DIR} -D${MACHINE})
    MESSAGE(STATUS "med.h found in ${MED2_INCLUDE_DIR}")
  ELSE(MED2_INCLUDE_DIR)
    MESSAGE(STATUS "med.h not found in ${MED2HOME}/include, check your installation.")
    SET(MED_ENABLE_MED2 OFF)
  ENDIF(MED2_INCLUDE_DIR)
ENDIF(MED_ENABLE_MED2)

SET(MED2_LIBS)
FOREACH(name medC med medimport)
  IF(MED_ENABLE_MED2)
    FIND_LIBRARY(${name}_LIB ${name} ${MED2HOME}/lib)
    IF(${name}_LIB)
      MESSAGE(STATUS "${name} lib found: ${${name}_LIB}")
      SET(MED2_LIBS ${MED2_LIBS} ${${name}_LIB})
    ELSE(${name}_LIB)
      MESSAGE(STATUS "${name} lib not found in ${MED2HOME}/lib, check your installation.")
      SET(MED_ENABLE_MED2 OFF)
    ENDIF(${name}_LIB)
  ENDIF(MED_ENABLE_MED2)
ENDFOREACH(name medC med medimport)

IF(MED_ENABLE_MED2)
ELSE(MED_ENABLE_MED2)
  MESSAGE(STATUS "medfile not found ... optional ... disabled")
ENDIF(MED_ENABLE_MED2)
