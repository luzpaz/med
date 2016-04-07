# Copyright (C) 2012-2016  CEA/DEN, EDF R&D
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

SALOME_CONFIGURE_FILE(medcalc_testutils.py.in ${CMAKE_CURRENT_BINARY_DIR}/medcalc_testutils.py)

SET(_test_scripts
    test_qttesting.py    # main entry point
    test_scalarmap.py
)

SET(_test_scenarii
   scenarios/test_scalarmap.xml
)

SET(_test_baselines
    baselines/test_scalarmap.png
)

SALOME_INSTALL_SCRIPTS("${_test_scripts}" ${SALOME_INSTALL_PYTHON}/tests)
SALOME_INSTALL_SCRIPTS(${CMAKE_CURRENT_BINARY_DIR}/medcalc_testutils.py ${SALOME_INSTALL_PYTHON})
 
INSTALL(FILES ${_test_scenarii} DESTINATION ${SALOME_MED_INSTALL_RES_DATA}/tests/scenarios)
INSTALL(FILES ${_test_baselines} DESTINATION ${SALOME_MED_INSTALL_RES_DATA}/tests/baselines)
