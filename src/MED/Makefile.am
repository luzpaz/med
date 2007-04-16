#  MED MED : implemetation of MED idl descriptions
#
#  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#
#

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES= libMEDEngine.la

dist_libMEDEngine_la_SOURCES= Med_Gen_i.cxx
salomeinclude_HEADERS = Med_Gen_i.hxx

libMEDEngine_la_CPPFLAGS= @CAS_CPPFLAGS@ $(MED2_INCLUDES) $(HDF5_INCLUDES) ${KERNEL_CXXFLAGS} @CORBA_CXXFLAGS@ @CORBA_INCLUDES@ \
	-I$(srcdir)/../MEDMEM_I -I$(top_builddir)/salome_adm/unix -I$(top_builddir)/idl \
	-I$(srcdir)/../MEDMEM -I$(srcdir)/../MEDWrapper/V2_1/Core

libMEDEngine_la_LDFLAGS= @CAS_OCAF@ $(HDF5_LIBS) ${KERNEL_LDFLAGS} @CORBA_LIBS@ \
	-lSalomeContainer -lSalomeHDFPersist -lSalomeDS -lSalomeNS -lRegistry -lOpUtil \
	../MEDMEM_I/libMEDMEMImpl.la $(top_builddir)/idl/libSalomeIDLMED.la

dist_salomescript_DATA=  MED_test1.py MED_test2.py Med_Gen_test.py testMedAlliances.py testMedAlliances1.py
