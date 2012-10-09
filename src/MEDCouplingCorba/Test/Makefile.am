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

bin_PROGRAMS= TestMEDCouplingCorbaServ TestMEDCouplingCorbaClt

TestMEDCouplingCorbaServ_CPPFLAGS=$(CORBA_CXXFLAGS) $(CORBA_INCLUDES) @CPPUNIT_INCLUDES@ -I$(srcdir)/.. -I$(srcdir)/../../INTERP_KERNEL/Bases -I$(srcdir)/../../INTERP_KERNELTest -I$(srcdir)/../../INTERP_KERNEL \
	-I$(srcdir)/../../INTERP_KERNEL/Geometric2D -I$(srcdir)/../../MEDCoupling -I$(top_builddir)/idl ${KERNEL_CXXFLAGS}

TestMEDCouplingCorbaServ_LDFLAGS = @CPPUNIT_LIBS@ ../libmedcouplingcorba.la ../Client/libmedcouplingclient.la ../../../idl/libSalomeIDLMED.la ../../../idl/libSalomeIDLMEDTests.la ../../MEDCoupling/libmedcoupling.la @CORBA_LIBS@ ../../INTERP_KERNEL/libinterpkernel.la

dist_TestMEDCouplingCorbaServ_SOURCES = TestMEDCouplingCorbaServ.cxx MEDCouplingMeshFieldFactoryComponent.cxx MEDCouplingMeshFieldFactoryComponentServ.cxx \
	MEDCouplingMeshFieldFactoryComponent.hxx MEDCouplingMeshFieldFactoryComponentServ.hxx

TestMEDCouplingCorbaClt_CPPFLAGS=$(PTHREAD_CFLAGS) $(CORBA_CXXFLAGS) $(CORBA_INCLUDES) @CPPUNIT_INCLUDES@ -I$(srcdir)/../Client -I$(srcdir)/../../INTERP_KERNEL/Bases -I$(srcdir)/../../INTERP_KERNELTest -I$(srcdir)/../../INTERP_KERNEL \
	-I$(srcdir)/../../INTERP_KERNEL/Geometric2D -I$(srcdir)/../../MEDCoupling -I$(top_builddir)/idl ${KERNEL_CXXFLAGS}

TestMEDCouplingCorbaClt_LDFLAGS = $(PTHREAD_LIBS) @CPPUNIT_LIBS@ ../Client/libmedcouplingclient.la ../../../idl/libSalomeIDLMED.la ../../../idl/libSalomeIDLMEDTests.la ../../MEDCoupling/libmedcoupling.la @CORBA_LIBS@ ../../INTERP_KERNEL/libinterpkernel.la -L${KERNEL_ROOT_DIR}/lib/salome/ -lSalomeIDLKernel

dist_TestMEDCouplingCorbaClt_SOURCES = TestMEDCouplingCorbaClt.cxx MEDCouplingMeshFieldFactoryComponent.cxx MEDCouplingMeshFieldFactoryComponentClt.cxx \
	MEDCouplingMeshFieldFactoryComponent.hxx MEDCouplingMeshFieldFactoryComponentClt.hxx
