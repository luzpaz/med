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

# Author : Guillaume Boulant (EDF) 

include $(top_srcdir)/adm_local/unix/make_common_starter.am

mypkgpythondir = $(salomepythondir)/xmed

# Modules that are strictly required for the fields operations in
# SALOME:
mainpkg = \
	__init__.py \
	fieldproxy.py \
	fieldtools.py \
	fieldguide.py \
	cmdtools.py

# For the control of PARAVIS and VISU
mainpkg+= \
	driver_pvis.py \
	driver_visu.py

# For the medop.sh executable program
mainpkg+= \
	medop.py

# Modules that are required for test or tutorial purposes only:
testpkg = \
	properties.py

mypkgpython_PYTHON = \
	$(mainpkg) \
	$(testpkg)

mybindir = $(salomescriptdir)/med
testscripts=\
	tests/test_xmed_fieldOperations.py \
	tests/test_xmed_uiEventListener.py \
	tests/test_xmed_visualisation.py

dist_mybin_SCRIPTS=\
	$(testscripts)
