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

MESSAGE(STATUS "Check for XDR ...")
SET(XDR_ROOT_DIR $ENV{XDR_ROOT_DIR} CACHE PATH "Path to the XDR.")
IF(XDR_ROOT_DIR)
  LIST(APPEND CMAKE_LIBRARY_PATH "${XDR_ROOT_DIR}/lib")
  LIST(APPEND CMAKE_INCLUDE_PATH "${XDR_ROOT_DIR}/include")
ENDIF(XDR_ROOT_DIR)

FIND_LIBRARY(XDR_LIBRARIES xdr)
FIND_PATH(XDR_INCLUDE_DIRS rpc/xdr.h)
SET(XDR_DEFINITIONS "-DHAS_XDR")

IF(WIN32)
  LIST(APPEND XDR_INCLUDE_DIRS "${XDR_ROOT_DIR}/include/src/msvc")
ENDIF(WIN32)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(XDR REQUIRED_VARS XDR_INCLUDE_DIRS)
