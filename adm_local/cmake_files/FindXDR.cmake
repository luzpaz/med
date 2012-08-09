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
#
MESSAGE(STATUS "Check for xdr ...")

# ------

IF(XDR_IS_MANDATORY STREQUAL 0)
  SET(XDR_IS_MANDATORY 0)
  SET(XDR_IS_OPTIONAL 1)
ENDIF(XDR_IS_MANDATORY STREQUAL 0)
IF(XDR_IS_OPTIONAL STREQUAL 0)
  SET(XDR_IS_MANDATORY 1)
  SET(XDR_IS_OPTIONAL 0)
ENDIF(XDR_IS_OPTIONAL STREQUAL 0)
IF(NOT XDR_IS_MANDATORY AND NOT XDR_IS_OPTIONAL)
  SET(XDR_IS_MANDATORY 0)
  SET(XDR_IS_OPTIONAL 1)
ENDIF(NOT XDR_IS_MANDATORY AND NOT XDR_IS_OPTIONAL)

# ------

SET(XDR_STATUS 1)
IF(WITHOUT_XDR OR WITH_XDR STREQUAL 0)
  SET(XDR_STATUS 0)
  MESSAGE(STATUS "xdr disabled from command line.")
ENDIF(WITHOUT_XDR OR WITH_XDR STREQUAL 0)

# ------

IF(XDR_STATUS)
  IF(WITH_XDR)
    SET(XDR_ROOT_USER ${WITH_XDR})
  ELSE(WITH_XDR)
    SET(XDR_ROOT_USER $ENV{XDR_ROOT})
  ENDIF(WITH_XDR)
ENDIF(XDR_STATUS)

# -----

IF(XDR_STATUS)
  IF(XDR_ROOT_USER)
    SET(XDR_FIND_PATHS_OPTION NO_DEFAULT_PATH)
  ELSE(XDR_ROOT_USER)
    SET(XDR_FIND_PATHS_OPTION)
  ENDIF(XDR_ROOT_USER)
ENDIF(XDR_STATUS)

# -----

IF(XDR_STATUS)
  IF(XDR_ROOT_USER)
    SET(XDR_INCLUDE_PATHS ${XDR_ROOT_USER} ${XDR_ROOT_USER}/include)
  ELSE(XDR_ROOT_USER)
    SET(XDR_INCLUDE_PATHS)
  ENDIF(XDR_ROOT_USER)
  SET(XDR_INCLUDE_TO_FIND rpc/xdr.h)
  FIND_PATH(XDR_INCLUDE_DIR ${XDR_INCLUDE_TO_FIND} PATHS ${XDR_INCLUDE_PATHS} ${XDR_FIND_PATHS_OPTION})
  IF(XDR_INCLUDE_DIR)
    IF(XDR_ROOT_USER)
      SET(XDR_INCLUDES -I${XDR_INCLUDE_DIR} -I${XDR_INCLUDE_DIR}/src/msvc)# to remove
      SET(XDR_INCLUDE_DIRS ${XDR_INCLUDE_DIR} ${XDR_INCLUDE_DIR}/src/msvc)
    ENDIF(XDR_ROOT_USER)
    MESSAGE(STATUS "${XDR_INCLUDE_TO_FIND} found in ${XDR_INCLUDE_DIR}")
  ELSE(XDR_INCLUDE_DIR)
    SET(XDR_STATUS 0)
    IF(XDR_ROOT_USER)
      MESSAGE(STATUS "${XDR_INCLUDE_TO_FIND} not found in ${XDR_INCLUDE_PATHS}, check your XDR installation.")
    ELSE(XDR_ROOT_USER)
      MESSAGE(STATUS "${XDR_INCLUDE_TO_FIND} not found on system, try to use WITH_XDR option or XDR_ROOT environment variable.")
    ENDIF(XDR_ROOT_USER)
  ENDIF(XDR_INCLUDE_DIR)
ENDIF(XDR_STATUS)

# ----

IF(XDR_STATUS)
  IF(XDR_ROOT_USER)
    SET(XDR_LIB_PATHS ${XDR_ROOT_USER}/lib)
  ELSE(XDR_ROOT_USER)
    SET(XDR_LIB_PATHS)
  ENDIF(XDR_ROOT_USER)
ENDIF(XDR_STATUS)

IF(XDR_STATUS)
  FIND_LIBRARY(XDR_LIB xdr PATHS ${XDR_LIB_PATHS} ${XDR_FIND_PATHS_OPTION})
  SET(XDR_LIBS)
  IF(XDR_LIB)
    SET(XDR_LIBS ${XDR_LIBS} ${XDR_LIB})
    MESSAGE(STATUS "xdr lib found: ${XDR_LIB}")
  ELSE(XDR_LIB)
    SET(XDR_STATUS 0)
    IF(XDR_ROOT_USER)
      MESSAGE(STATUS "xdr lib not found in ${XDR_LIB_PATHS}, check your XDR installation.")
    ELSE(XDR_ROOT_USER)
      MESSAGE(STATUS "xdr lib not found on system, try to use WITH_XDR option or XDR_ROOT environment variable.")
    ENDIF(XDR_ROOT_USER)
  ENDIF(XDR_LIB)
ENDIF(XDR_STATUS)

# ----

IF(XDR_STATUS)
  SET(XDR_IS_OK 1)
  SET(XDR_CPPFLAGS -DHAS_XDR ${XDR_INCLUDES})# to remove
  SET(XDR_DEFINITIONS "-DHAS_XDR")
ELSE(XDR_STATUS)
  IF(XDR_IS_MANDATORY)
    MESSAGE(FATAL_ERROR "xdr not found ... mandatory ... abort")
  ELSE(XDR_IS_MANDATORY)
    MESSAGE(STATUS "xdr not found ... optional ... disabled")
  ENDIF(XDR_IS_MANDATORY)
ENDIF(XDR_STATUS)

# ------
