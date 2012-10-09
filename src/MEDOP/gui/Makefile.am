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

# author: Guillaume Boulant (EDF/R&D, 2012)

include $(top_srcdir)/adm_local/unix/make_common_starter.am

SUBDIRS = dialogs

#
# ---------------------------------------------------------
# Generation rules to create moc files from QObject headers
# and form source files from ui files
# ---------------------------------------------------------
# WRN: this rules could be integrated in the make_common_starter.am.
# For that, we first have to make the naming convention of Q_OBJECT
# files be the same between MEDGUI (old GUI) and MEDOP/gui (new GUI).
%_moc.cxx: %.hxx
	$(MOC) $< -o $@

%.qm: %.ts
	$(LRELEASE) $< -qm $@

EXTRA_DIST+=$(MOC_FILES:%_moc.cxx=%.hxx) $(nodist_salomeres_DATA:%.qm=%.ts) test

mostlyclean-local:
	rm -f @builddir@/*_moc.cxx
	rm -f @builddir@/*.qm

# Libraries targets
lib_LTLIBRARIES = libMEDOPGUI.la

salomeinclude_HEADERS =          \
	MEDOPFactoryClient.hxx   \
	XmedConsoleDriver.hxx    \
	WorkspaceController.hxx  \
	XmedDataModel.hxx        \
	MEDEventListener_i.hxx   \
	MEDOPModule.hxx          \
	DatasourceController.hxx \
	DatasourceConstants.hxx \
	MEDOPGUI.hxx

dist_libMEDOPGUI_la_SOURCES =    \
	MEDOPFactoryClient.cxx   \
	XmedConsoleDriver.cxx    \
	WorkspaceController.cxx  \
	XmedDataModel.cxx        \
	MEDEventListener_i.cxx   \
	MEDOPModule.cxx          \
	DatasourceController.cxx \
	factory.cxx

# MOC pre-processing
MOC_FILES = \
	MEDOPModule_moc.cxx          \
	MEDEventListener_i_moc.cxx   \
	WorkspaceController_moc.cxx  \
	DatasourceController_moc.cxx

nodist_libMEDOPGUI_la_SOURCES = $(MOC_FILES)

OMNIORB_CXXFLAGS=@CORBA_CXXFLAGS@ @CORBA_INCLUDES@
OMNIORB_LIBS=@OMNIORB_LIBS@

QT_CXXFLAGS  = @QT_INCLUDES@ @QT_MT_INCLUDES@
QT_LDFLAGS = $(QT_MT_LIBS)

CAS_CXXFLAGS = @CAS_CPPFLAGS@ @CAS_CXXFLAGS@

BOOST_CXXFLAGS=@BOOST_CPPFLAGS@

libMEDOPGUI_la_CPPFLAGS =     \
	$(QT_CXXFLAGS)        \
	$(CAS_CXXFLAGS)       \
	$(OMNIORB_CXXFLAGS)   \
	$(BOOST_CXXFLAGS)     \
	$(KERNEL_CXXFLAGS)    \
	$(GUI_CXXFLAGS)       \
	$(MED_CXXFLAGS)       \
	-I$(srcdir)/dialogs   \
	-Idialogs \
	-I$(srcdir)/../cmp    \
	-I$(top_builddir)/idl \
	-I$(top_builddir)

libMEDOPGUI_la_LDFLAGS  = \
	-no-undefined -version-info=0:0:0                              \
	$(KERNEL_LDFLAGS) -lSalomeLifeCycleCORBA -lSalomeKernelHelpers \
	$(GUI_LDFLAGS) -lSalomeApp -lSalomeGuiHelpers -lSalomeTreeData \
	$(QT_LDFLAGS)                                                  \
	$(BOOST_LIBS)                                                  \
	$(OMNIORB_LIBS)

# Specify MED internal libraries to be linked in
libMEDOPGUI_la_LIBADD  =                         \
	$(top_builddir)/idl/libSalomeIDLMED.la   \
	dialogs/libMEDOPGUI_dialogs.la           \
	../cmp/libMEDOPFactoryEngine.la

if WITH_MEDMEMGUI
libMEDOPGUI_la_CPPFLAGS += "-D__WITH_MEDMEMGUI__" -I$(top_srcdir)/src/MEDGUI
libMEDOPGUI_la_LIBADD += $(top_builddir)/src/MEDGUI/libMEDGUI.la
endif

# resources files
nodist_salomeres_DATA = \
	MEDOP_msg_en.qm \
	MEDOP_msg_fr.qm
