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

# This Makefile is responsible of generating the client and server
# implementation of IDL interfaces for both C++ and python usage.
# The building process of the C++ files is in charge of each source
# package and then is not manage here.
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

BASEIDL_FILES = \
	MED.idl MED_Gen.idl Compo1Py.idl \
	MEDCouplingCorbaServant.idl      \
	MEDCouplingCorbaServantTest.idl  \
	ParaMEDCouplingCorbaServant.idl

MEDOPIDL_FILES =             \
	MEDDataManager.idl   \
	MEDCalculator.idl    \
	MEDEventListener.idl \
	MEDOPFactory.idl

MPIIDL_FILES = ParaMEDMEMComponent.idl

IDL_FILES = $(BASEIDL_FILES)
if MPI_IS_OK
IDL_FILES += $(MPIIDL_FILES)
endif
IDL_FILES += $(MEDOPIDL_FILES)

IDL_FILES_PY=$(IDL_FILES:%.idl=%_idl.py)

# This variable defines the files to be installed
dist_salomeidl_DATA = $(IDL_FILES)

# GUI idl common library
lib_LTLIBRARIES = libSalomeIDLMED.la libSalomeIDLMEDTests.la

# Sources built from idl files
BASEIDL_SOURCES = \
	MEDSK.cc MED_GenSK.cc Compo1PySK.cc \
	MEDCouplingCorbaServantSK.cc        \
	ParaMEDCouplingCorbaServantSK.cc

MEDOPIDL_SOURCES =            \
	MEDDataManagerSK.cc   \
	MEDCalculatorSK.cc    \
	MEDEventListenerSK.cc \
	MEDOPFactorySK.cc

MPIIDL_SOURCES = ParaMEDMEMComponentSK.cc

IDL_SOURCES = $(BASEIDL_SOURCES)
if MPI_IS_OK
  IDL_SOURCES += $(MPIIDL_SOURCES)
endif
IDL_SOURCES += $(MEDOPIDL_SOURCES)

nodist_libSalomeIDLMED_la_SOURCES = $(IDL_SOURCES)

# header files must be exported: other modules have to use this library
nodist_salomeinclude_HEADERS =         \
	MED.hh MED_Gen.hh Compo1Py.hh  \
	MEDCouplingCorbaServant.hh     \
	MEDCouplingCorbaServantTest.hh \
	ParaMEDCouplingCorbaServant.hh

if MPI_IS_OK
  nodist_salomeinclude_HEADERS += ParaMEDMEMComponent.hh
endif

nodist_salomeinclude_HEADERS += \
	MEDDataManager.hh       \
	MEDCalculator.hh        \
	MEDEventListener.hh     \
	MEDOPFactory.hh

libSalomeIDLMED_la_CPPFLAGS =			\
	-I$(top_builddir)/idl			\
	@CORBA_CXXFLAGS@ @CORBA_INCLUDES@ @KERNEL_CXXFLAGS@
libSalomeIDLMED_la_LDFLAGS = -no-undefined -version-info=0:0:0
libSalomeIDLMED_la_LIBADD  = @CORBA_LIBS@ $(KERNEL_LDFLAGS) -lSalomeIDLKernel

nodist_libSalomeIDLMEDTests_la_SOURCES = MEDCouplingCorbaServantTestSK.cc
libSalomeIDLMEDTests_la_CPPFLAGS = $(libSalomeIDLMED_la_CPPFLAGS)
libSalomeIDLMEDTests_la_LIBADD = libSalomeIDLMED.la

# These variables defines the building process of CORBA files
OMNIORB_IDL         = @OMNIORB_IDL@
OMNIORB_IDLCXXFLAGS = @OMNIORB_IDLCXXFLAGS@
OMNIORB_IDLPYFLAGS  = @OMNIORB_IDLPYFLAGS@ -I$(top_builddir)/idl/salome -I$(KERNEL_ROOT_DIR)/idl/salome 
IDLCXXFLAGS = -bcxx @IDLCXXFLAGS@ -I$(top_builddir)/idl/salome -I$(KERNEL_ROOT_DIR)/idl/salome
IDLPYFLAGS  = @IDLPYFLAGS@ -I$(KERNEL_ROOT_DIR)/idl/salome

SUFFIXES = .idl .hh SK.cc

%SK.cc %.hh : %.idl
	$(OMNIORB_IDL) $(IDLCXXFLAGS) $(OMNIORB_IDLCXXFLAGS) $<

# MEDSK.cc : MED.idl
#	$(OMNIORB_IDL) $(IDLCXXFLAGS) $(OMNIORB_IDLCXXFLAGS) $<
# MED.hh : MEDSK.cc

install-exec-local: $(IDL_FILES:%=$(top_srcdir)/idl/%)
	$(INSTALL) -d  $(DESTDIR)$(salomepythondir)
	ls $^ | while read file; do \
	  $(OMNIORB_IDL) $(IDLPYFLAGS) -C$(DESTDIR)$(salomepythondir) $$file ; \
	done

# we want to remove only staff generated for IDL files and nothing more
uninstall-local:
	@for modulen in SALOME_MED SALOME_TEST Compo1Py_ORB MEDOP ; do \
	  test -d $(DESTDIR)$(salomepythondir)/$${modulen} && echo "Removing $(DESTDIR)$(salomepythondir)/$${modulen}" && rm -rf $(DESTDIR)$(salomepythondir)/$${modulen} ; \
	  test -d $(DESTDIR)$(salomepythondir)/$${modulen}__POA && echo "Removing $(DESTDIR)$(salomepythondir)/$${modulen}__POA" && rm -rf $(DESTDIR)$(salomepythondir)/$${modulen}__POA ; \
	done ; \
	for filen in $(IDL_FILES_PY) ; do \
	  echo "Removing $(DESTDIR)$(salomepythondir)/$${filen}" && rm -f $(DESTDIR)$(salomepythondir)/$${filen}* ; \
	done

mostlyclean-local: clean-idls

clean-idls:
	-rm -f *.hh *.cc .depidl

# we use cpp to generate dependencies between idl files.
# option x c tells the preprocessor to consider idl as a c file.
# if an idl is modified, all idl dependencies are rebuilt

.depidl: $(BASEIDL_FILES)
	@echo "" > $@
	@for dep in $^ dummy; do \
	  if [ $$dep != "dummy" ]; then \
	    echo Building dependencies for $$dep; \
	    $(CPP) $(C_DEPEND_FLAG) -x c -I$(srcdir) -I$(KERNEL_ROOT_DIR)/idl/salome $$dep 2>/dev/null | \
	    sed 's/\.o/\SK.cc/' >>$@; \
	  fi; \
	done ;

-include .depidl
