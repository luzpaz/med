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

#  MED MEDMEM_I : MED idl descriptions implementation based on the classes of MEDMEM
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES= libMEDMEMImpl.la

salomeinclude_HEADERS= \
  MEDMEM_convert.hxx \
  MEDMEM_Med_i.hxx \
  MEDMEM_Family_i.hxx \
  MEDMEM_FieldTemplate_i.hxx \
  MEDMEM_TraitsForFields.hxx \
  MEDMEM_Field_i.hxx \
  MEDMEM_Group_i.hxx \
  MEDMEM_Mesh_i.hxx \
  MEDMEM_Support_i.hxx \
  MEDMEM_I.hxx \
  MEDMEM_GMesh_i.hxx

dist_libMEDMEMImpl_la_SOURCES= \
	MEDMEM_Med_i.cxx \
	MEDMEM_Family_i.cxx \
	MEDMEM_Field_i.cxx \
	MEDMEM_Group_i.cxx \
	MEDMEM_Mesh_i.cxx \
	MEDMEM_Support_i.cxx \
	MEDMEM_convert.cxx \
	MEDMEM_GMesh_i.cxx

libMEDMEMImpl_la_CPPFLAGS= \
	$(CORBA_CXXFLAGS) $(CORBA_INCLUDES) $(MED3_INCLUDES) $(HDF5_INCLUDES) ${KERNEL_CXXFLAGS} \
	-I$(srcdir)/../MEDMEM \
	-I$(top_builddir)/idl \
	-I$(srcdir)/../INTERP_KERNEL/Bases \
	-I$(srcdir)/../INTERP_KERNEL/GaussPoints \
	-I$(srcdir)/../INTERP_KERNEL

libMEDMEMImpl_la_LDFLAGS= \
	../../idl/libSalomeIDLMED.la \
	../MEDMEM/libmedmem.la \
	${KERNEL_LDFLAGS} -lSalomeLifeCycleCORBA -lSALOMELocalTrace -lSalomeCommunication -lSalomeGenericObj -lSalomeNS -lOpUtil \
	$(MED3_LIBS_C_ONLY) \
	$(HDF5_LIBS)
