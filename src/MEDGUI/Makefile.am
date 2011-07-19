# Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

#  MED MEDGUI : MED component GUI implemetation
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# Libraries targets
lib_LTLIBRARIES= libMEDGUI.la

UIC_FILES = ui_MEDGUIFileContentDial.h ui_MEDGUISelectComponents.h

BUILT_SOURCES = $(UIC_FILES)

EXTRA_DIST+= MEDGUIFileContentDial.ui MEDGUISelectComponents.ui

dist_libMEDGUI_la_SOURCES = MEDGUIFileContentDial.h MEDGUISelection.hxx \
	MEDGUI.h MEDGUI.cxx MEDGUIFileContentDial.cxx MEDGUISelection.cxx \
	MEDGUISelectComponents.h MEDGUISelectComponents.cxx MEDGUIDataBaseDockWidget.hxx \
	MEDGUIDataBaseDockWidget.cxx

MOC_FILES= MEDGUI_moc.cxx MEDGUIFileContentDial_moc.cxx MEDGUISelectComponents_moc.cxx

ui_%.h: %.ui
	${QTDIR}/bin/uic $< -o $@

.hxx_moc.cxx :
	${QTDIR}/bin/moc -p . -o $@ $<

%_moc.cxx: %.hxx
	${QTDIR}/bin/moc $< -o $@

.qrc_qrc.cxx :
	${QTDIR}/bin/rcc -name $(*F)  $< -o $@

clean-local-qt :
	rm -f *_moc.cxx *_qrc.cxx ui_*.h

clean-local: clean-local-qt

# End Of Qt Part

nodist_libMEDGUI_la_SOURCES= $(MOC_FILES)

libMEDGUI_la_CPPFLAGS= $(QT_INCLUDES) $(VTK_INCLUDES) @CAS_CPPFLAGS@ $(PYTHON_INCLUDES) $(BOOST_CPPFLAGS) \
	${KERNEL_CXXFLAGS} ${GUI_CXXFLAGS} \
	@CORBA_INCLUDES@ @CORBA_CXXFLAGS@ \
	-I$(top_builddir)/idl -I$(srcdir)/../MEDCoupling -I$(srcdir)/../INTERP_KERNEL -I$(srcdir)/../INTERP_KERNEL/Bases \
	-I$(srcdir)/../MEDCoupling -I$(srcdir)/../MEDLoader -I$(srcdir)/../MEDCalculator

libMEDGUI_la_LDFLAGS= \
	$(QT_MT_LIBS) $(CAS_KERNEL) $(BOOST_LIBS) \
	../../idl/libSalomeIDLMED.la ../MEDCalculator/libmedcalculator.la \
	$(KERNEL_LDFLAGS) -lSalomeLifeCycleCORBA -lOpUtil -lSALOMELocalTrace \
	$(GUI_LDFLAGS) -lSalomeApp -lCAM -lLightApp -lqtx -lsuit -lSalomeObject

# .po files to transform in .qm
nodist_salomeres_DATA= \
	MED_images.qm \
	MED_msg_fr.qm \
	MED_msg_en.qm
