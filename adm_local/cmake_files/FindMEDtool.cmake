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


# Find MEDtool installation
#
# The following variable are set:
#   MEDTOOL_INCLUDE_DIRS
#   INTERPKERNEL_LIBRARY
#   MEDCOUPLING_LIBRARY
#   MEDLOADER_LIBRARY
#   MEDTOOL_LIBRARIES
#   MEDTOOL_PYTHON_DIRS
#
#  The CMake (or environment) variable MEDTOOL_ROOT_DIR can be set to
#  guide the detection and indicate a root directory to look into.
#

MESSAGE(STATUS "Check for MEDtool ...")

SET(MEDTOOL_ROOT_DIR $ENV{MEDTOOL_ROOT_DIR} CACHE PATH "Path to the MEDtool.")
IF(MEDTOOL_ROOT_DIR)
  LIST(APPEND CMAKE_PREFIX_PATH "${MEDTOOL_ROOT_DIR}")
ENDIF(MEDTOOL_ROOT_DIR)

FIND_PATH(MEDTOOL_INCLUDE_DIRS MEDCoupling.hxx) # mandatory

FIND_LIBRARY(INTERPKERNEL_LIBRARY NAMES interpkernel) # mandatory
FIND_LIBRARY(MEDCOUPLING_REMAPPER_LIBRARY NAMES medcouplingremapper) # mandatory
FIND_LIBRARY(MEDCOUPLING_LIBRARY NAMES medcoupling) # mandatory
FIND_LIBRARY(MEDLOADER_LIBRARY NAMES medloader) # optional
FIND_LIBRARY(MEDPARTITIONERCPP_LIBRARY NAMES medpartitionercpp) # optional
FIND_LIBRARY(RENUMBERCPP_LIBRARY NAMES renumbercpp) # optional

FIND_PACKAGE(PythonInterp QUIET)
FIND_PATH(MEDTOOL_PYTHON_DIRS NAMES MEDCoupling.py MEDLoader.py PATH_SUFFIXES lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages) # optional

SET(MEDTOOL_LIBRARIES
  ${INTERPKERNEL_LIBRARY}
  ${MEDCOUPLING_REMAPPER_LIBRARY}
  ${MEDCOUPLING_LIBRARY}
  ${MEDLOADER_LIBRARY}
  ${MEDPARTITIONERCPP_LIBRARY}
  ${RENUMBERCPP_LIBRARY}
  )

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MEDtool REQUIRED_VARS MEDTOOL_INCLUDE_DIRS MEDTOOL_LIBRARIES)
