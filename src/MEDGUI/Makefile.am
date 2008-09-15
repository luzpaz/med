#  MED MEDGUI : MED component GUI implemetation
#
#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#

include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets
lib_LTLIBRARIES= libMED.la
dist_libMED_la_SOURCES = MedGUI_Selection.h MedGUI.h MedGUI.cxx MedGUI_Selection.cxx
MOC_FILES= MedGUI_moc.cxx
nodist_libMED_la_SOURCES= $(MOC_FILES)

libMED_la_CPPFLAGS= $(QT_INCLUDES) $(VTK_INCLUDES) @CAS_CPPFLAGS@ $(PYTHON_INCLUDES) \
	${KERNEL_CXXFLAGS} ${GUI_CXXFLAGS} ${BOOST_CPPFLAGS} \
	@CORBA_INCLUDES@ @CORBA_CXXFLAGS@ \
	-I$(top_builddir)/idl -I$(top_builddir)/salome_adm/unix

libMED_la_LDFLAGS= \
	../../idl/libSalomeIDLMED.la \
	$(KERNEL_LDFLAGS) -lSalomeLifeCycleCORBA \
	$(GUI_LDFLAGS) -lSalomeApp

# .po files to transform in .qm
nodist_salomeres_DATA= \
	MED_images.qm \
	MED_msg_fr.qm \
	MED_msg_en.qm
