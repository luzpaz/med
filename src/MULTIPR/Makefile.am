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
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES= libMULTIPR_API.la

salomeinclude_HEADERS= \
MULTIPR_API.hxx \
MULTIPR_DecimationAccel.hxx \
MULTIPR_DecimationFilter.hxx \
MULTIPR_Elements.hxx \
MULTIPR_Exceptions.hxx \
MULTIPR_Family.hxx \
MULTIPR_Field.hxx \
MULTIPR_GaussLoc.hxx \
MULTIPR_Globals.hxx \
MULTIPR_MeshDis.hxx \
MULTIPR_Mesh.hxx \
MULTIPR_Nodes.hxx \
MULTIPR_PointOfField.hxx \
MULTIPR_Profil.hxx \
MULTIPR_Utils.hxx \
MULTIPR_Obj.hxx \
MULTIPR_ProgressCallback.hxx

dist_libMULTIPR_API_la_SOURCES= \
MULTIPR_API.cxx \
MULTIPR_Mesh.cxx \
MULTIPR_Profil.cxx \
MULTIPR_GaussLoc.cxx \
MULTIPR_Field.cxx \
MULTIPR_Nodes.cxx \
MULTIPR_Elements.cxx \
MULTIPR_Family.cxx \
MULTIPR_MeshDis.cxx \
MULTIPR_DecimationFilter.cxx \
MULTIPR_DecimationAccel.cxx \
MULTIPR_Utils.cxx \
MULTIPR_Obj.cxx

libMULTIPR_API_la_CPPFLAGS= $(MED2_INCLUDES) @CXXTMPDPTHFLAGS@ -Wall -pthread \
	-I$(srcdir)/../MEDWrapper/Factory \
	-I$(srcdir)/../MEDWrapper/Base \
	-I$(srcdir)/../MEDMEM \
	-I$(srcdir)/../MEDSPLITTER \
	-I$(top_builddir)/salome_adm/unix
libMULTIPR_API_la_LDFLAGS= $(MED2_LIBS) -lm -lmed \
	../MEDWrapper/Factory/libMEDWrapper.la \
	../MEDWrapper/Base/libMEDWrapperBase.la \
	../MEDMEM/libmedmem.la \
	../MEDSPLITTER/libmedsplitter.la

if MED_ENABLE_KERNEL
  libMULTIPR_API_la_CPPFLAGS+= ${KERNEL_CXXFLAGS}
  libMULTIPR_API_la_LDFLAGS+= ${KERNEL_LDFLAGS} -lSALOMELocalTrace 
#libMULTIPR_API_la_LDFLAGS+= ${KERNEL_LDFLAGS} -lSalomeContainer -lOpUtil -lSALOMELocalTrace 
endif

# Executables targets
bin_PROGRAMS= multipr

dist_multipr_SOURCES= multipr.cxx

multipr_CPPFLAGS= $(libMULTIPR_API_la_CPPFLAGS)
multipr_LDADD= $(libMULTIPR_API_la_LDFLAGS) \
	../MEDWrapper/V2_1/Core/libmed_V2_1.la \
	../MEDWrapper/V2_1/Wrapper/libMEDWrapper_V2_1.la \
	../MEDWrapper/V2_2/libMEDWrapper_V2_2.la \
	../MEDWrapper/Base/libMEDWrapperBase.la \
	libMULTIPR_API.la
