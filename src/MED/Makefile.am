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

#  MED MED : implemetation of MED idl descriptions
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES= libMEDEngine.la

dist_libMEDEngine_la_SOURCES= Med_Gen_i.cxx Med_Gen_Driver_i.cxx
salomeinclude_HEADERS = Med_Gen_i.hxx Med_Gen_Driver_i.hxx

libMEDEngine_la_CPPFLAGS= $(MED3_INCLUDES) $(HDF5_INCLUDES) ${KERNEL_CXXFLAGS} @CORBA_CXXFLAGS@ @CORBA_INCLUDES@ \
	-I$(srcdir)/../MEDMEM_I \
	-I$(top_builddir) \
	-I$(top_builddir)/idl \
	-I$(srcdir)/../MEDMEM \
	-I$(srcdir)/../INTERP_KERNEL \
	-I$(srcdir)/../INTERP_KERNEL/Bases \
	-I$(srcdir)/../INTERP_KERNEL/GaussPoints

libMEDEngine_la_LDFLAGS=\
	$(top_builddir)/idl/libSalomeIDLMED.la \
	../MEDMEM_I/libMEDMEMImpl.la \
	${KERNEL_LDFLAGS} -lTOOLSDS -lSalomeHDFPersist -lSalomeContainer
#	${KERNEL_LDFLAGS} \
#	@CORBA_LIBS@ \
#	@CAS_OCAF@ \
#	$(HDF5_LIBS)
# -lOpUtil -lSalomeContainer -lSalomeDS -lSalomeNS -lRegistry
#	
#	



dist_salomescript_DATA=  MED_test1.py MED_test2.py Med_Gen_test.py testMedAlliances.py testMedAlliances1.py
