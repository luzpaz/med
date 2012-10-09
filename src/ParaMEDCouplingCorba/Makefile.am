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

include $(top_srcdir)/adm_local/unix/make_common_starter.am

###
# MEDLoader and Test sub-folders are available only if MED3 is detected ok
### 

lib_LTLIBRARIES= libparamedcouplingcorba.la

salomeinclude_HEADERS= ParaMEDCouplingFieldDoubleServant.hxx ParaMEDCouplingUMeshServant.hxx

dist_libparamedcouplingcorba_la_SOURCES= ParaMEDCouplingFieldDoubleServant.cxx ParaMEDCouplingUMeshServant.cxx

libparamedcouplingcorba_la_CPPFLAGS= $(KERNEL_CXXFLAGS) $(CORBA_CXXFLAGS) $(CORBA_INCLUDES) $(MPI_INCLUDES) \
	-I$(srcdir)/../MEDCoupling -I$(srcdir)/../MEDCouplingCorba -I$(top_builddir)/idl -I$(srcdir)/../INTERP_KERNEL/Bases -I$(srcdir)/../INTERP_KERNEL

libparamedcouplingcorba_la_LDFLAGS= ../MEDCouplingCorba/libmedcouplingcorba.la ../MEDCoupling/libmedcoupling.la $(MPI_LIBS)

LDADD= -lm

