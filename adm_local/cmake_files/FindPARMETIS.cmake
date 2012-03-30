# Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

# ------

MESSAGE(STATUS "Check for parmetis ...")

# ------

IF(PARMETIS_IS_MANDATORY STREQUAL 0)
  SET(PARMETIS_IS_MANDATORY 0)
  SET(PARMETIS_IS_OPTIONAL 1)
ENDIF(PARMETIS_IS_MANDATORY STREQUAL 0)
IF(PARMETIS_IS_OPTIONAL STREQUAL 0)
  SET(PARMETIS_IS_MANDATORY 1)
  SET(PARMETIS_IS_OPTIONAL 0)
ENDIF(PARMETIS_IS_OPTIONAL STREQUAL 0)
IF(NOT PARMETIS_IS_MANDATORY AND NOT PARMETIS_IS_OPTIONAL)
  SET(PARMETIS_IS_MANDATORY 0)
  SET(PARMETIS_IS_OPTIONAL 1)
ENDIF(NOT PARMETIS_IS_MANDATORY AND NOT PARMETIS_IS_OPTIONAL)

# ------

SET(MED_ENABLE_PARMETIS 1)
IF(WITHOUT_PARMETIS OR WITH_PARMETIS STREQUAL 0)
  SET(MED_ENABLE_PARMETIS 0)
  MESSAGE(STATUS "parmetis disabled from command line.")
ENDIF(WITHOUT_PARMETIS OR WITH_PARMETIS STREQUAL 0)

# ------

IF(MED_ENABLE_PARMETIS)
  IF(WITH_PARMETIS)
    SET(PARMETIS_ROOT_USER ${WITH_PARMETIS})
  ENDIF(WITH_PARMETIS)
  IF(NOT PARMETIS_ROOT_USER)
    SET(PARMETIS_ROOT_USER $ENV{PARMETISDIR})
  ENDIF(NOT PARMETIS_ROOT_USER)
  IF(NOT PARMETIS_ROOT_USER)
    SET(PARMETIS_ROOT_USER $ENV{PARMETIS_ROOT})
  ENDIF(NOT PARMETIS_ROOT_USER)
  IF(NOT PARMETIS_ROOT_USER)
    SET(PARMETIS_ROOT_USER $ENV{PARMETISHOME})
  ENDIF(NOT PARMETIS_ROOT_USER)
ENDIF(MED_ENABLE_PARMETIS)

# ------

IF(MED_ENABLE_PARMETIS)
  IF(NOT PARMETIS_ROOT_USER)
    FIND_PROGRAM(PARMETIS_MAINBINARY parmetis)
    IF(PARMETIS_MAINBINARY)
      MESSAGE(STATUS "parmetis found: ${PARMETIS_MAINBINARY}")
      IF(PARMETIS_MAINBINARY STREQUAL /usr/bin/parmetis)
      ELSE(PARMETIS_MAINBINARY STREQUAL /usr/bin/parmetis)
	SET(PARMETIS_ROOT_USER ${PARMETIS_MAINBINARY})
	GET_FILENAME_COMPONENT(PARMETIS_ROOT_USER ${PARMETIS_ROOT_USER} PATH)
	GET_FILENAME_COMPONENT(PARMETIS_ROOT_USER ${PARMETIS_ROOT_USER} PATH)
      ENDIF(PARMETIS_MAINBINARY STREQUAL /usr/bin/parmetis)
    ELSE(PARMETIS_MAINBINARY)
      MESSAGE(STATUS "parmetis not found, try to use WITH_PARMETIS option or PARMETIS_ROOT (or PARMETISHOME or PARMETISDIR) environment variable")
      SET(MED_ENABLE_PARMETIS 0)
    ENDIF(PARMETIS_MAINBINARY)
  ENDIF(NOT PARMETIS_ROOT_USER)
ENDIF(MED_ENABLE_PARMETIS)

# ------

IF(MED_ENABLE_PARMETIS)
  IF(PARMETIS_ROOT_USER)
    SET(PARMETIS_FIND_PATHS_OPTION NO_DEFAULT_PATH)
  ELSE(PARMETIS_ROOT_USER)
    SET(PARMETIS_FIND_PATHS_OPTION)
  ENDIF(PARMETIS_ROOT_USER)
ENDIF(MED_ENABLE_PARMETIS)

# ------

IF(MED_ENABLE_PARMETIS)
  IF(PARMETIS_ROOT_USER)
    SET(PARMETIS_INCLUDE_PATHS ${PARMETIS_ROOT_USER})
  ELSE(PARMETIS_ROOT_USER)
    SET(PARMETIS_INCLUDE_PATHS)
  ENDIF(PARMETIS_ROOT_USER)
  SET(PARMETIS_INCLUDE_TO_FIND parmetis.h)
  FIND_PATH(PARMETIS_INCLUDE_DIR ${PARMETIS_INCLUDE_TO_FIND} PATHS ${PARMETIS_INCLUDE_PATHS} ${PARMETIS_FIND_PATHS_OPTION})
  IF(PARMETIS_INCLUDE_DIR)
    IF(PARMETIS_ROOT_USER)
      SET(PARMETIS_INCLUDES -I${PARMETIS_INCLUDE_DIR})# to remove
      SET(PARMETIS_INCLUDES_DIR ${PARMETIS_INCLUDE_DIR})
    ENDIF(PARMETIS_ROOT_USER)
    SET(PARMETIS_INCLUDES -D_PARMETISUSEDLL_ ${PARMETIS_INCLUDES})# to remove
    SET(PARMETIS_FLAGS "-D_PARMETISUSEDLL_")
    MESSAGE(STATUS "${PARMETIS_INCLUDE_TO_FIND} found in ${PARMETIS_INCLUDE_DIR}")
  ELSE(PARMETIS_INCLUDE_DIR)
    SET(MED_ENABLE_PARMETIS 0)
    IF(PARMETIS_ROOT_USER)
      MESSAGE(STATUS "${PARMETIS_INCLUDE_TO_FIND} not found in ${PARMETIS_INCLUDE_PATHS}, check your PARMETIS installation.")
    ELSE(PARMETIS_ROOT_USER)
      MESSAGE(STATUS "${PARMETIS_INCLUDE_TO_FIND} not found on system, try to use WITH_PARMETIS option or PARMETIS_ROOT (or PARMETISHOME or PARMETISDIR) environment variable.")
    ENDIF(PARMETIS_ROOT_USER)
  ENDIF(PARMETIS_INCLUDE_DIR)
ENDIF(MED_ENABLE_PARMETIS)

# ----

IF(MED_ENABLE_PARMETIS)
  IF(PARMETIS_ROOT_USER)
    IF(WINDOWS)
      SET(PARMETIS_LIB_PATHS ${PARMETIS_ROOT_USER} ${PARMETIS_ROOT_USER})
    ELSE(WINDOWS)
      SET(PARMETIS_LIB_PATHS ${PARMETIS_ROOT_USER})
    ENDIF(WINDOWS)
  ELSE(PARMETIS_ROOT_USER)
    SET(PARMETIS_LIB_PATHS)
  ENDIF(PARMETIS_ROOT_USER)
  IF(WINDOWS)
    IF(CMAKE_BUILD_TYPE STREQUAL Release)
      FIND_LIBRARY(PARMETIS_LIB parmetisdll PATHS ${PARMETIS_LIB_PATHS} ${PARMETIS_FIND_PATHS_OPTION})
      FIND_LIBRARY(PARMETIS_LIB2 metisdll PATHS ${PARMETIS_LIB_PATHS} ${PARMETIS_FIND_PATHS_OPTION})
    ELSE(CMAKE_BUILD_TYPE STREQUAL Release)
      FIND_LIBRARY(PARMETIS_LIB parmetisddll PATHS ${PARMETIS_LIB_PATHS} ${PARMETIS_FIND_PATHS_OPTION})
      FIND_LIBRARY(PARMETIS_LIB2 metisddll PATHS ${PARMETIS_LIB_PATHS} ${PARMETIS_FIND_PATHS_OPTION})
    ENDIF(CMAKE_BUILD_TYPE STREQUAL Release)
  ELSE(WINDOWS)
    FIND_LIBRARY(PARMETIS_LIB parmetis PATHS ${PARMETIS_LIB_PATHS} ${PARMETIS_FIND_PATHS_OPTION})
    FIND_LIBRARY(PARMETIS_LIB2 metis PATHS ${PARMETIS_LIB_PATHS} ${PARMETIS_FIND_PATHS_OPTION})
  ENDIF(WINDOWS)
  SET(PARMETIS_LIBS)
  IF(PARMETIS_LIB)
    SET(PARMETIS_LIBS ${PARMETIS_LIBS} ${PARMETIS_LIB} ${PARMETIS_LIB2})
    MESSAGE(STATUS "parmetis lib found: ${PARMETIS_LIB}")
  ELSE(PARMETIS_LIB)
    SET(MED_ENABLE_PARMETIS 0)
    IF(PARMETIS_ROOT_USER)
      MESSAGE(STATUS "parmetis lib not found in ${PARMETIS_LIB_PATHS}, check your PARMETIS installation.")
    ELSE(PARMETIS_ROOT_USER)
      MESSAGE(STATUS "parmetis lib not found on system, try to use WITH_PARMETIS option or PARMETIS_ROOT (or PARMETISHOME or PARMETISDIR) environment variable.")
    ENDIF(PARMETIS_ROOT_USER)
  ENDIF(PARMETIS_LIB)
ENDIF(MED_ENABLE_PARMETIS)

# ----

IF(MED_ENABLE_PARMETIS)
  SET(PARMETIS_CPPFLAGS ${PARMETIS_INCLUDES} -DMED_ENABLE_PARMETIS)# to remove
  SET(PARMETIS_FLAGS "${PARMETIS_FLAGS} -DMED_ENABLE_PARMETIS")
ELSE(MED_ENABLE_PARMETIS)
  IF(PARMETIS_IS_MANDATORY)
    MESSAGE(FATAL_ERROR "parmetis not found ... mandatory ... abort")
  ELSE(PARMETIS_IS_MANDATORY)
    MESSAGE(STATUS "parmetis not found ... optional ... disabled")
  ENDIF(PARMETIS_IS_MANDATORY)
ENDIF(MED_ENABLE_PARMETIS)

# ------
