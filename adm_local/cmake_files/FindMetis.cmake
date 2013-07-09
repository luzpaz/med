# Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
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

MESSAGE(STATUS "Check for metis ...")

SET(METIS_ROOT_DIR $ENV{METIS_ROOT_DIR} CACHE PATH "Path to the MEDFile.")
IF(METIS_ROOT_DIR)
  LIST(APPEND CMAKE_LIBRARY_PATH "${METIS_ROOT_DIR}")
  LIST(APPEND CMAKE_INCLUDE_PATH "${METIS_ROOT_DIR}/Lib")
ENDIF(METIS_ROOT_DIR)

FIND_LIBRARY(METIS_LIBRARIES metis)
FIND_PATH(METIS_INCLUDE_DIRS metis.h)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Metis REQUIRED_VARS METIS_INCLUDE_DIRS METIS_LIBRARIES)
