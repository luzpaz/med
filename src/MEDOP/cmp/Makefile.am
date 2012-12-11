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

mypkgpythondir = $(salomescriptdir)/med
dist_mypkgpython_SCRIPTS = \
	test_medop_components.py

salomeinclude_HEADERS =      \
	MEDOP.hxx            \
	MEDDataManager_i.hxx \
	MEDCalculator_i.hxx  \
	MEDOPFactory_i.hxx

lib_LTLIBRARIES = libMEDOPFactoryEngine.la
libMEDOPFactoryEngine_la_SOURCES = \
	MEDDataManager_i.cxx       \
	MEDCalculator_i.cxx        \
	MEDOPFactory_i.cxx

libMEDOPFactoryEngine_la_LDFLAGS = -no-undefined -version-info=0:0:0

OMNIORB_CXXFLAGS=@CORBA_CXXFLAGS@ @CORBA_INCLUDES@
OMNIORB_LIBS=@OMNIORB_LIBS@

libMEDOPFactoryEngine_la_CPPFLAGS = \
	-I$(top_builddir)/idl \
	-I$(top_srcdir)/src/MEDCoupling \
	-I$(top_srcdir)/src/INTERP_KERNEL/Bases \
	-I$(top_srcdir)/src/INTERP_KERNEL \
	-I$(top_srcdir)/src/MEDLoader \
	$(KERNEL_CXXFLAGS) \
	$(MED3_INCLUDES) \
	$(HDF5_CXXFLAGS) \
	$(OMNIORB_CXXFLAGS)

libMEDOPFactoryEngine_la_LIBADD = \
	$(top_builddir)/idl/libSalomeIDLMED.la  \
	$(KERNEL_LDFLAGS) -lSalomeKernelHelpers  \
	$(top_builddir)/src/MEDCoupling/libmedcoupling.la \
	$(top_builddir)/src/MEDLoader/libmedloader.la \
	$(top_builddir)/src/INTERP_KERNEL/libinterpkernel.la \
	$(MED3_LIBS_C_ONLY) \
	$(HDF5_LIBS) \
	$(OMNIORB_LIBS)
