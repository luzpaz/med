  #  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
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
#  MED MEDMEM : MED files in memory
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

# this directory must be recompiled before Test folder

if CPPUNIT_IS_OK
 SUBDIRS=. 
endif

lib_LTLIBRARIES= libmedsplitter.la

salomeinclude_HEADERS= \
MEDPARTITIONER_Topology.hxx \
MEDPARTITIONER_MESHCollection.hxx \
MEDPARTITIONER_MESHCollectionDriver.H \
MEDPARTITIONER_MESHCollectionMedXMLDriver.H \
MEDPARTITIONER_MESHCollectionMedAsciiDriver.H \
MEDPARTITIONER_MESHCollectionDriver.hxx \
MEDPARTITIONER_MESHCollectionMedXMLDriver.hxx \
MEDPARTITIONER_MESHCollectionMedAsciiDriver.hxx \
MEDPARTITIONER_ParallelTopology.hxx \
MEDPARTITIONER_FaceModel.hxx \
MEDPARTITIONER_Graph.hxx\
MEDPARTITIONER_UserGraph.hxx\
MEDPARTITIONER_SequentialTopology.hxx \
MEDPARTITIONER_utils.hxx \
MEDPARTITIONER.hxx \
MEDPARTITIONER_ParaDomainSelector.hxx \
MEDPARTITIONER_ConnectZone.hxx \
MEDPARTITIONER_SkyLineArray.hxx

if MED_ENABLE_METIS
    salomeinclude_HEADERS+= MEDPARTITIONER_METISGraph.hxx
endif
if MED_ENABLE_SCOTCH
    salomeinclude_HEADERS+= MEDPARTITIONER_SCOTCHGraph.hxx
endif

dist_libmedsplitter_la_SOURCES= \
MEDPARTITIONER_MESHCollection.cxx \
MEDPARTITIONER_MESHCollectionDriver.cxx \
MEDPARTITIONER_MESHCollectionMedXMLDriver.cxx \
MEDPARTITIONER_MESHCollectionMedAsciiDriver.cxx \
MEDPARTITIONER_ParallelTopology.cxx \
MEDPARTITIONER_Graph.cxx\
MEDPARTITIONER_UserGraph.cxx\
MEDPARTITIONER_ParaDomainSelector.cxx \
MEDPARTITIONER_JointExchangeData.cxx \
MEDPARTITIONER_SkyLineArray.cxx \
MEDPARTITIONER_ConnectZone.cxx

if MED_ENABLE_METIS
    dist_libmedsplitter_la_SOURCES+= MEDPARTITIONER_METISGraph.cxx
endif
if MED_ENABLE_SCOTCH
    dist_libmedsplitter_la_SOURCES+= MEDPARTITIONER_SCOTCHGraph.cxx
endif

libmedsplitter_la_CPPFLAGS= $(MPI_INCLUDES) $(MED2_INCLUDES) $(HDF5_INCLUDES) @CXXTMPDPTHFLAGS@ \
	$(BOOST_CPPFLAGS) $(LIBXML_INCLUDES) \
	-I$(srcdir)/../MEDMEM -I$(srcdir)/../MEDWrapper/V2_1/Core \
	-I$(srcdir)/../INTERP_KERNEL/Bases -I$(srcdir)/../MEDCoupling \
  -I$(srcdir)/../MEDLoader -I$(srcdir)/../INTERP_KERNEL

libmedsplitter_la_LDFLAGS= 
#libmedsplitter_la_LDFLAGS= $(MED2_LIBS) $(HDF5_LIBS) $(STDLIB) $(LIBXML_LIBS) \
#	../MEDMEM/libmedmem.la ../MEDWrapper/V2_1/Core/libmed_V2_1.la

if MED_ENABLE_PARMETIS
  libmedsplitter_la_CPPFLAGS+= $(PARMETIS_CPPFLAGS)
  libmedsplitter_la_LDFLAGS+= $(PARMETIS_LIBS)
endif
if MED_ENABLE_METIS
  libmedsplitter_la_CPPFLAGS+= $(METIS_CPPFLAGS)
  libmedsplitter_la_LDFLAGS+= $(METIS_LIBS)
endif
if MED_ENABLE_SCOTCH
  libmedsplitter_la_CPPFLAGS+= $(SCOTCH_CPPFLAGS)
  libmedsplitter_la_LDFLAGS+= $(SCOTCH_LIBS)
endif
if MED_ENABLE_KERNEL
  libmedsplitter_la_CPPFLAGS+= ${KERNEL_CXXFLAGS}
  libmedsplitter_la_LDFLAGS+= ${KERNEL_LDFLAGS} -lSALOMELocalTrace 
endif

libmedsplitter_la_LDFLAGS+= $(MED2_LIBS) $(HDF5_LIBS) $(STDLIB) $(LIBXML_LIBS) $(MPI_LIBS) \
	../MEDMEM/libmedmem.la  ../INTERP_KERNEL/libinterpkernel.la ../MEDCoupling/libmedcoupling.la ../MEDLoader/libmedloader.la 

# Executables targets
bin_PROGRAMS= medsplitter

dist_medsplitter_SOURCES= medsplitter.cxx

medsplitter_CPPFLAGS= $(libmedsplitter_la_CPPFLAGS)
medsplitter_LDADD= $(libmedsplitter_la_LDFLAGS) -lm $(BOOST_LIBS) libmedsplitter.la
if MED_ENABLE_KERNEL
  medsplitter_LDADD+= -lSALOMEBasics
endif

if MPI_IS_OK
  bin_PROGRAMS+=medsplitter_para
  dist_medsplitter_para_SOURCES= medsplitter_para.cxx
  medsplitter_para_CPPFLAGS= $(medsplitter_CPPFLAGS)
  medsplitter_para_LDADD= $(medsplitter_LDADD)
endif

OBSOLETE_FILES = \
	MEDPARTITIONER_SequentialTopology.cxx \
	test_HighLevelAPI.cxx

EXTRA_DIST += $(OBSOLETE_FILES)
