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

lib_LTLIBRARIES= libparamedmemcompo.la

salomeinclude_HEADERS= MPIMEDCouplingFieldDoubleServant.hxx ParaMEDMEMComponent_i.hxx

dist_libparamedmemcompo_la_SOURCES= MPIMEDCouplingFieldDoubleServant.cxx ParaMEDMEMComponent_i.cxx

#libmedmem_la_LDFLAGS= -L$(top_builddir)/lib@LIB_LOCATION_SUFFIX@/salome 
libparamedmemcompo_la_CPPFLAGS= $(KERNEL_CXXFLAGS) $(CORBA_CXXFLAGS) $(CORBA_INCLUDES) $(MPI_INCLUDES) @CXXTMPDPTHFLAGS@ \
	-I$(srcdir)/../ParaMEDCouplingCorba -I$(srcdir)/../ParaMEDMEM -I$(srcdir)/../INTERP_KERNEL -I$(srcdir)/../INTERP_KERNEL/Bases -I$(srcdir)/../INTERP_KERNEL/Geometric2D -I$(srcdir)/../MEDCoupling -I$(srcdir)/../MEDCouplingCorba -I$(top_builddir)/idl

# change motivated by the bug KERNEL4778.
libparamedmemcompo_la_LDFLAGS= ../ParaMEDCouplingCorba/libparamedcouplingcorba.la ../ParaMEDMEM/libparamedmem.la $(MPI_LIBS)

LDADD= -lm

