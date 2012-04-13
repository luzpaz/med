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

MESSAGE(STATUS "Check for splitters activation ...")

# ------

IF(SPLITTER_IS_MANDATORY STREQUAL 0)
  SET(SPLITTER_IS_MANDATORY 0)
  SET(SPLITTER_IS_OPTIONAL 1)
ENDIF(SPLITTER_IS_MANDATORY STREQUAL 0)
IF(SPLITTER_IS_OPTIONAL STREQUAL 0)
  SET(SPLITTER_IS_MANDATORY 1)
  SET(SPLITTER_IS_OPTIONAL 0)
ENDIF(SPLITTER_IS_OPTIONAL STREQUAL 0)
IF(NOT SPLITTER_IS_MANDATORY AND NOT SPLITTER_IS_OPTIONAL)
  SET(SPLITTER_IS_MANDATORY 0)
  SET(SPLITTER_IS_OPTIONAL 1)
ENDIF(NOT SPLITTER_IS_MANDATORY AND NOT SPLITTER_IS_OPTIONAL)

# ------

SET(MED_ENABLE_SPLITTER 1)
IF(WITHOUT_SPLITTER OR WITH_SPLITTER STREQUAL 0)
  SET(MED_ENABLE_SPLITTER 0)
  MESSAGE(STATUS "splitter disabled from command line.")
ENDIF(WITHOUT_SPLITTER OR WITH_SPLITTER STREQUAL 0)
