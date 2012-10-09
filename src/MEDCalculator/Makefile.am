# Copyright (C) 2007-2012  CEA/DEN, EDF R&D
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
# Author : Anthony Geay (CEA/DEN)

include $(top_srcdir)/adm_local/unix/make_common_starter.am

MEDCALCULATOR_SUBDIRS =

if !MED_ENABLE_MICROMED
   MEDCALCULATOR_SUBDIRS += . Swig
if CPPUNIT_IS_OK
   MEDCALCULATOR_SUBDIRS += Test
endif
endif

SUBDIRS = $(MEDCALCULATOR_SUBDIRS)

lib_LTLIBRARIES = libmedcalculator.la

salomeinclude_HEADERS= \
MEDCalculatorBrowserMesh.hxx                 \
MEDCalculatorBrowserField.hxx                \
MEDCalculatorBrowserStep.hxx                 \
MEDCalculatorBrowserLiteStruct.hxx           \
MEDCalculatorDBRangeSelection.hxx            \
MEDCalculatorDBSliceField.hxx                \
MEDCalculatorDBField.hxx                     \
MedCalculatorDefines.hxx

dist_libmedcalculator_la_SOURCES= \
MEDCalculatorBrowserMesh.cxx                 \
MEDCalculatorBrowserField.cxx                \
MEDCalculatorBrowserStep.cxx                 \
MEDCalculatorBrowserLiteStruct.cxx           \
MEDCalculatorDBRangeSelection.cxx            \
MEDCalculatorDBSliceField.cxx                \
MEDCalculatorDBField.cxx

libmedcalculator_la_CPPFLAGS= $(MED3_INCLUDES) $(HDF5_INCLUDES) @CXXTMPDPTHFLAGS@ \
	-I$(srcdir)/../INTERP_KERNEL \
	-I$(srcdir)/../INTERP_KERNEL/Geometric2D \
	-I$(srcdir)/../INTERP_KERNEL/Bases \
	-I$(srcdir)/../MEDCoupling \
	-I$(srcdir)/../MEDLoader \
	-I$(srcdir)/../MEDCouplingCorba \
	$(CORBA_CXXFLAGS) $(CORBA_INCLUDES) -I$(top_builddir)/idl @KERNEL_CXXFLAGS@

libmedcalculator_la_LDFLAGS= ../MEDLoader/libmedloader.la ../MEDCouplingCorba/libmedcouplingcorba.la $(KERNEL_LDFLAGS) -lSALOMELocalTrace -lSalomeNS -lOpUtil
