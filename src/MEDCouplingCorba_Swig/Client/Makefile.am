#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
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
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES = _libMEDCouplingClient_Swig.la

salomeinclude_HEADERS = libMEDCouplingClient_Swig.i

SWIG_DEF = libMEDCouplingClient_Swig.i

SWIG_FLAGS = @SWIG_FLAGS@ -I$(srcdir) -I$(srcdir)/../../MEDCouplingClient -I$(srcdir)/../../MEDCoupling -I$(srcdir)/../../MEDCoupling_Swig -I$(srcdir)/../../INTERP_KERNEL -I$(srcdir)/../../INTERP_KERNEL/Bases

dist__libMEDCouplingClient_Swig_la_SOURCES   = $(SWIG_DEF)

nodist__libMEDCouplingClient_Swig_la_SOURCES = libMEDCouplingClient_Swig_wrap.cxx

libMEDCouplingClient_Swig.py: libMEDCouplingClient_Swig_wrap.cxx

libMEDCouplingClient_Swig_wrap.cxx: $(SWIG_DEF)
	$(SWIG) $(SWIG_FLAGS) -o $@  $<

_libMEDCouplingClient_Swig_la_CPPFLAGS = $(PYTHON_INCLUDES) \
	$(CORBA_CXXFLAGS) $(CORBA_INCLUDES) -I$(top_builddir)/idl ${KERNEL_CXXFLAGS} @CXXTMPDPTHFLAGS@ \
	-I$(srcdir)/../../INTERP_KERNEL -I$(srcdir)/../../INTERP_KERNEL/Bases \
	-I$(srcdir)/../../MEDCoupling_Swig -I$(srcdir)/../../MEDCoupling \
	-I$(srcdir)/../../MEDCouplingCorba/Client -I$(srcdir)/..

_libMEDCouplingClient_Swig_la_LDFLAGS  = -module $(PYTHON_LIBS) \
	../../MEDCouplingCorba/Client/libmedcouplingclient.la \
	../../MEDCoupling/libmedcoupling.la ../../INTERP_KERNEL/libinterpkernel.la \
	../../../idl/libSalomeIDLMED.la

CLEANFILES = libMEDCouplingClient_Swig_wrap.cxx libMEDCouplingClient_Swig.py

nodist_salomescript_DATA= libMEDCouplingClient_Swig.py
