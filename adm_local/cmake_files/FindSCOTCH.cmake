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

# ------

MESSAGE(STATUS "Check for scotch ...")

# ------

IF(SCOTCH_IS_MANDATORY STREQUAL 0)
  SET(SCOTCH_IS_MANDATORY 0)
  SET(SCOTCH_IS_OPTIONAL 1)
ENDIF(SCOTCH_IS_MANDATORY STREQUAL 0)
IF(SCOTCH_IS_OPTIONAL STREQUAL 0)
  SET(SCOTCH_IS_MANDATORY 1)
  SET(SCOTCH_IS_OPTIONAL 0)
ENDIF(SCOTCH_IS_OPTIONAL STREQUAL 0)
IF(NOT SCOTCH_IS_MANDATORY AND NOT SCOTCH_IS_OPTIONAL)
  SET(SCOTCH_IS_MANDATORY 0)
  SET(SCOTCH_IS_OPTIONAL 1)
ENDIF(NOT SCOTCH_IS_MANDATORY AND NOT SCOTCH_IS_OPTIONAL)

# ------

SET(MED_ENABLE_SCOTCH 1)
IF(WITHOUT_SCOTCH OR WITH_SCOTCH STREQUAL 0)
  SET(MED_ENABLE_SCOTCH 0)
  MESSAGE(STATUS "scotch disabled from command line.")
ENDIF(WITHOUT_SCOTCH OR WITH_SCOTCH STREQUAL 0)

# ------

IF(MED_ENABLE_SCOTCH)
  IF(WITH_SCOTCH)
    SET(SCOTCH_ROOT_USER ${WITH_SCOTCH})
  ENDIF(WITH_SCOTCH)
  IF(NOT SCOTCH_ROOT_USER)
    SET(SCOTCH_ROOT_USER $ENV{SCOTCHDIR})
  ENDIF(NOT SCOTCH_ROOT_USER)
  IF(NOT SCOTCH_ROOT_USER)
    SET(SCOTCH_ROOT_USER $ENV{SCOTCH_ROOT})
  ENDIF(NOT SCOTCH_ROOT_USER)
  IF(NOT SCOTCH_ROOT_USER)
    SET(SCOTCH_ROOT_USER $ENV{SCOTCHHOME})
  ENDIF(NOT SCOTCH_ROOT_USER)
ENDIF(MED_ENABLE_SCOTCH)

# ------

IF(MED_ENABLE_SCOTCH)
  IF(NOT SCOTCH_ROOT_USER)
    FIND_PROGRAM(SCOTCH_MAINBINARY gpart)
    IF(SCOTCH_MAINBINARY)
      MESSAGE(STATUS "scotch found: ${SCOTCH_MAINBINARY}")
      IF(SCOTCH_MAINBINARY STREQUAL /usr/bin/gpart)
      ELSE(SCOTCH_MAINBINARY STREQUAL /usr/bin/gpart)
	SET(SCOTCH_ROOT_USER ${SCOTCH_MAINBINARY})
	GET_FILENAME_COMPONENT(SCOTCH_ROOT_USER ${SCOTCH_ROOT_USER} PATH)
	GET_FILENAME_COMPONENT(SCOTCH_ROOT_USER ${SCOTCH_ROOT_USER} PATH)
      ENDIF(SCOTCH_MAINBINARY STREQUAL /usr/bin/gpart)
    ELSE(SCOTCH_MAINBINARY)
      MESSAGE(STATUS "scotch not found, try to use WITH_SCOTCH option or SCOTCH_ROOT (or SCOTCHHOME or SCOTCHDIR) environment variable")
      SET(MED_ENABLE_SCOTCH 0)
    ENDIF(SCOTCH_MAINBINARY)
  ENDIF(NOT SCOTCH_ROOT_USER)
ENDIF(MED_ENABLE_SCOTCH)

# ------

IF(MED_ENABLE_SCOTCH)
  IF(SCOTCH_ROOT_USER)
    SET(SCOTCH_FIND_PATHS_OPTION NO_DEFAULT_PATH)
  ELSE(SCOTCH_ROOT_USER)
    SET(SCOTCH_FIND_PATHS_OPTION)
  ENDIF(SCOTCH_ROOT_USER)
ENDIF(MED_ENABLE_SCOTCH)

# ------

IF(MED_ENABLE_SCOTCH)
  IF(SCOTCH_ROOT_USER)
    SET(SCOTCH_INCLUDE_PATHS ${SCOTCH_ROOT_USER}/include)
  ELSE(SCOTCH_ROOT_USER)
    SET(SCOTCH_INCLUDE_PATHS)
  ENDIF(SCOTCH_ROOT_USER)
  SET(SCOTCH_INCLUDE_TO_FIND scotch.h)
  FIND_PATH(SCOTCH_INCLUDE_DIR ${SCOTCH_INCLUDE_TO_FIND} PATHS ${SCOTCH_INCLUDE_PATHS} ${SCOTCH_FIND_PATHS_OPTION})
  IF(SCOTCH_INCLUDE_DIR)
    IF(SCOTCH_ROOT_USER)
      SET(SCOTCH_INCLUDES -I${SCOTCH_INCLUDE_DIR})#to remove
      SET(SCOTCH_INCLUDES_DIR ${SCOTCH_INCLUDE_DIR})
    ENDIF(SCOTCH_ROOT_USER)
    SET(SCOTCH_INCLUDES -D_SCOTCHUSEDLL_ ${SCOTCH_INCLUDES})#to remove
    SET(SCOTCH_FLAGS "-D_SCOTCHUSEDLL_")
    MESSAGE(STATUS "${SCOTCH_INCLUDE_TO_FIND} found in ${SCOTCH_INCLUDE_DIR}")
  ELSE(SCOTCH_INCLUDE_DIR)
    SET(MED_ENABLE_SCOTCH 0)
    IF(SCOTCH_ROOT_USER)
      MESSAGE(STATUS "${SCOTCH_INCLUDE_TO_FIND} not found in ${SCOTCH_INCLUDE_PATHS}, check your SCOTCH installation.")
    ELSE(SCOTCH_ROOT_USER)
      MESSAGE(STATUS "${SCOTCH_INCLUDE_TO_FIND} not found on system, try to use WITH_SCOTCH option or SCOTCH_ROOT (or SCOTCHHOME or SCOTCHDIR) environment variable.")
    ENDIF(SCOTCH_ROOT_USER)
  ENDIF(SCOTCH_INCLUDE_DIR)
ENDIF(MED_ENABLE_SCOTCH)

# ----

IF(MED_ENABLE_SCOTCH)
  IF(SCOTCH_ROOT_USER)
    IF(WINDOWS)
      SET(SCOTCH_LIB_PATHS ${SCOTCH_ROOT_USER}/dll ${SCOTCH_ROOT_USER}/lib)
    ELSE(WINDOWS)
      SET(SCOTCH_LIB_PATHS ${SCOTCH_ROOT_USER}/lib)
    ENDIF(WINDOWS)
  ELSE(SCOTCH_ROOT_USER)
    SET(SCOTCH_LIB_PATHS)
  ENDIF(SCOTCH_ROOT_USER)
  IF(WINDOWS)
    IF(CMAKE_BUILD_TYPE STREQUAL Release)
      FIND_LIBRARY(SCOTCH_LIB scotchdll PATHS ${SCOTCH_LIB_PATHS} ${SCOTCH_FIND_PATHS_OPTION})
      FIND_LIBRARY(SCOTCH_LIB2 scotcherrdll PATHS ${SCOTCH_LIB_PATHS} ${SCOTCH_FIND_PATHS_OPTION})
    ELSE(CMAKE_BUILD_TYPE STREQUAL Release)
      FIND_LIBRARY(SCOTCH_LIB scotchddll PATHS ${SCOTCH_LIB_PATHS} ${SCOTCH_FIND_PATHS_OPTION})
      FIND_LIBRARY(SCOTCH_LIB2 scotcherrddll PATHS ${SCOTCH_LIB_PATHS} ${SCOTCH_FIND_PATHS_OPTION})
    ENDIF(CMAKE_BUILD_TYPE STREQUAL Release)
  ELSE(WINDOWS)
    FIND_LIBRARY(SCOTCH_LIB scotch PATHS ${SCOTCH_LIB_PATHS} ${SCOTCH_FIND_PATHS_OPTION})
    FIND_LIBRARY(SCOTCH_LIB2 scotcherr PATHS ${SCOTCH_LIB_PATHS} ${SCOTCH_FIND_PATHS_OPTION})
  ENDIF(WINDOWS)
  SET(SCOTCH_LIBS)
  IF(SCOTCH_LIB)
    SET(SCOTCH_LIBS ${SCOTCH_LIBS} ${SCOTCH_LIB} ${SCOTCH_LIB2})
    MESSAGE(STATUS "scotch lib found: ${SCOTCH_LIB}")
  ELSE(SCOTCH_LIB)
    SET(MED_ENABLE_SCOTCH 0)
    IF(SCOTCH_ROOT_USER)
      MESSAGE(STATUS "scotch lib not found in ${SCOTCH_LIB_PATHS}, check your SCOTCH installation.")
    ELSE(SCOTCH_ROOT_USER)
      MESSAGE(STATUS "scotch lib not found on system, try to use WITH_SCOTCH option or SCOTCH_ROOT (or SCOTCHHOME or SCOTCHDIR) environment variable.")
    ENDIF(SCOTCH_ROOT_USER)
  ENDIF(SCOTCH_LIB)
ENDIF(MED_ENABLE_SCOTCH)

# ----

IF(MED_ENABLE_SCOTCH)
  SET(SCOTCH_CPPFLAGS ${SCOTCH_INCLUDES} -DMED_ENABLE_SCOTCH)# to remove
  SET(SCOTCH_FLAGS "${SCOTCH_FLAGS} -DMED_ENABLE_SCOTCH")
ELSE(MED_ENABLE_SCOTCH)
  IF(SCOTCH_IS_MANDATORY)
    MESSAGE(FATAL_ERROR "scotch not found ... mandatory ... abort")
  ELSE(SCOTCH_IS_MANDATORY)
    MESSAGE(STATUS "scotch not found ... optional ... disabled")
  ENDIF(SCOTCH_IS_MANDATORY)
ENDIF(MED_ENABLE_SCOTCH)

# ------
