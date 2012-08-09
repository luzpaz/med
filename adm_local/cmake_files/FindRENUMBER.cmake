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

MESSAGE(STATUS "Check for renumber activation ...")

# ------

IF(RENUMBER_IS_MANDATORY STREQUAL 0)
  SET(RENUMBER_IS_MANDATORY 0)
  SET(RENUMBER_IS_OPTIONAL 1)
ENDIF(RENUMBER_IS_MANDATORY STREQUAL 0)
IF(RENUMBER_IS_OPTIONAL STREQUAL 0)
  SET(RENUMBER_IS_MANDATORY 1)
  SET(RENUMBER_IS_OPTIONAL 0)
ENDIF(RENUMBER_IS_OPTIONAL STREQUAL 0)
IF(NOT RENUMBER_IS_MANDATORY AND NOT RENUMBER_IS_OPTIONAL)
  SET(RENUMBER_IS_MANDATORY 0)
  SET(RENUMBER_IS_OPTIONAL 1)
ENDIF(NOT RENUMBER_IS_MANDATORY AND NOT RENUMBER_IS_OPTIONAL)

# ------

SET(MED_ENABLE_RENUMBER 1)
IF(WITHOUT_RENUMBER OR WITH_RENUMBER STREQUAL 0)
  SET(MED_ENABLE_RENUMBER 0)
  MESSAGE(STATUS "renumber disabled from command line.")
ENDIF(WITHOUT_RENUMBER OR WITH_RENUMBER STREQUAL 0)
