# Copyright (C) 2015  CEA/DEN, EDF R&D, OPEN CASCADE
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


# Find MEDCoupling installation
#
# The following variable are set:
#   MEDCOUPLING_INCLUDE_DIRS
#   INTERPKERNEL_LIBRARY
#   MEDCOUPLING_LIBRARY
#   MEDLOADER_LIBRARY
#   PARAMEDMEM_LIBRARY
#   PARAMEDLOADER_LIBRARY
#   MEDCOUPLING_LIBRARIES
#   MEDCOUPLING_PYTHON_DIRS
#
#  The CMake (or environment) variable MEDCOUPLING_ROOT_DIR can be set to
#  guide the detection and indicate a root directory to look into.
#

MESSAGE(STATUS "Check for MEDCoupling ...")

SET(MEDCOUPLING_ROOT_DIR $ENV{MEDCOUPLING_ROOT_DIR} CACHE PATH "Path to the MEDCoupling.")
IF(MEDCOUPLING_ROOT_DIR)
  LIST(APPEND CMAKE_PREFIX_PATH "${MEDCOUPLING_ROOT_DIR}")
ENDIF(MEDCOUPLING_ROOT_DIR)

FIND_PATH(MEDCOUPLING_INCLUDE_DIRS MEDCoupling.hxx) # mandatory

FIND_LIBRARY(INTERPKERNEL_LIBRARY NAMES interpkernel) # mandatory
FIND_LIBRARY(MEDCOUPLING_REMAPPER_LIBRARY NAMES medcouplingremapper) # mandatory
FIND_LIBRARY(MEDCOUPLING_LIBRARY NAMES medcoupling) # mandatory
FIND_LIBRARY(MEDLOADER_LIBRARY NAMES medloader) # optional
FIND_LIBRARY(PARAMEDMEM_LIBRARY NAMES paramedmem) # optional
FIND_LIBRARY(PARAMEDLOADER_LIBRARY NAMES paramedloader) # optional
FIND_LIBRARY(MEDPARTITIONERCPP_LIBRARY NAMES medpartitionercpp) # optional
FIND_LIBRARY(RENUMBERCPP_LIBRARY NAMES renumbercpp) # optional

FIND_PACKAGE(PythonInterp QUIET)
FIND_PATH(MEDCOUPLING_PYTHON_DIRS NAMES MEDCoupling.py MEDLoader.py PATH_SUFFIXES lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages) # optional

SET(MEDCOUPLING_LIBRARIES
  ${INTERPKERNEL_LIBRARY}
  ${MEDCOUPLING_REMAPPER_LIBRARY}
  ${MEDCOUPLING_LIBRARY}
  ${MEDLOADER_LIBRARY}
  ${MEDPARTITIONERCPP_LIBRARY}
  ${RENUMBERCPP_LIBRARY}
  )

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MEDCoupling REQUIRED_VARS MEDCOUPLING_INCLUDE_DIRS MEDCOUPLING_LIBRARIES)
