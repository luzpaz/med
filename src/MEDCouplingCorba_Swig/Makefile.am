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

SUBDIRS = Client

lib_LTLIBRARIES = _MEDCouplingCorba.la

salomeinclude_HEADERS = MEDCouplingCorba.i MEDCouplingCorbaTypemaps.i

SWIG_DEF = MEDCouplingCorba.i MEDCouplingCorbaTypemaps.i

SWIG_FLAGS = @SWIG_FLAGS@ -I$(srcdir) -I$(srcdir)/../MEDCouplingCorba -I$(srcdir)/../MEDCoupling -I$(srcdir)/../MEDCoupling_Swig -I$(srcdir)/../INTERP_KERNEL -I$(srcdir)/../INTERP_KERNEL/Bases

dist__MEDCouplingCorba_la_SOURCES   = $(SWIG_DEF)

nodist__MEDCouplingCorba_la_SOURCES = MEDCouplingCorba_wrap.cxx

MEDCouplingCorba.py: MEDCouplingCorba_wrap.cxx

MEDCouplingCorba_wrap.cxx: $(SWIG_DEF)
	$(SWIG) $(SWIG_FLAGS) -o $@  $<

_MEDCouplingCorba_la_CPPFLAGS = $(PYTHON_INCLUDES) \
	$(CORBA_CXXFLAGS) $(CORBA_INCLUDES) -I$(top_builddir)/idl ${KERNEL_CXXFLAGS} @CXXTMPDPTHFLAGS@ \
	-I$(srcdir)/../INTERP_KERNEL -I$(srcdir)/../INTERP_KERNEL/Bases \
	-I$(srcdir)/../MEDCoupling_Swig -I$(srcdir)/../MEDCoupling \
	-I$(srcdir)/../MEDCouplingCorba

_MEDCouplingCorba_la_LDFLAGS  = -module $(PYTHON_LIBS) \
	../MEDCouplingCorba/libmedcouplingcorba.la \
	../MEDCoupling/libmedcoupling.la ../INTERP_KERNEL/libinterpkernel.la \
	../../idl/libSalomeIDLMED.la

CLEANFILES = MEDCouplingCorba_wrap.cxx MEDCouplingCorba.py

dist_salomescript_DATA= MEDCouplingCorba.py

EXTRA_DIST += MEDCouplingCorbaSwigTest.py MEDCouplingCorbaSwigTestClt.py MEDCouplingCorbaSwigTestServ.py \
              TestMEDCouplingCorbaClt.py TestMEDCouplingCorbaServ.py
