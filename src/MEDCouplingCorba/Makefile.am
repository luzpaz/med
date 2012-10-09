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

if CPPUNIT_IS_OK
  SUBDIRS = . Client Test
else
  SUBDIRS = . Client
endif

lib_LTLIBRARIES= libmedcouplingcorba.la

salomeinclude_HEADERS=                        \
	MEDCouplingCorba.hxx                  \
	MEDCouplingRefCountServant.hxx        \
	MEDCouplingMeshServant.hxx            \
	MEDCouplingPointSetServant.hxx        \
	MEDCouplingExtrudedMeshServant.hxx    \
	MEDCouplingCMeshServant.hxx           \
	MEDCouplingUMeshServant.hxx           \
	MEDCouplingFieldServant.hxx           \
	MEDCouplingFieldDoubleServant.hxx     \
	MEDCouplingFieldTemplateServant.hxx   \
	MEDCouplingMultiFieldsServant.hxx     \
	MEDCouplingFieldOverTimeServant.hxx   \
	DataArrayDoubleServant.hxx            \
	DataArrayIntServant.hxx               \
	DataArrayServant.hxx

dist_libmedcouplingcorba_la_SOURCES=         \
	MEDCouplingRefCountServant.cxx       \
	MEDCouplingMeshServant.cxx           \
	MEDCouplingPointSetServant.cxx       \
	MEDCouplingExtrudedMeshServant.cxx   \
	MEDCouplingCMeshServant.cxx          \
	MEDCouplingUMeshServant.cxx          \
	MEDCouplingFieldServant.cxx          \
	MEDCouplingFieldDoubleServant.cxx    \
	MEDCouplingFieldTemplateServant.cxx  \
	MEDCouplingMultiFieldsServant.cxx    \
	MEDCouplingFieldOverTimeServant.cxx  \
	DataArrayDoubleServant.cxx           \
	DataArrayIntServant.cxx              \
	DataArrayServant.cxx

libmedcouplingcorba_la_CPPFLAGS= $(CORBA_CXXFLAGS) $(CORBA_INCLUDES) -I$(top_builddir)/idl ${KERNEL_CXXFLAGS} \
	-I$(srcdir)/../MEDCoupling -I$(srcdir)/../INTERP_KERNEL/Bases -I$(srcdir)/../INTERP_KERNEL

libmedcouplingcorba_la_LDFLAGS= \
	../../idl/libSalomeIDLMED.la \
	../MEDCoupling/libmedcoupling.la 
