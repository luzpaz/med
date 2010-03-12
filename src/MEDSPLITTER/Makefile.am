#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D
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
 SUBDIRS=. Test
endif

lib_LTLIBRARIES= libmedsplitter.la

salomeinclude_HEADERS= \
MEDSPLITTER_Topology.hxx \
MEDSPLITTER_MESHCollection.hxx \
MEDSPLITTER_MESHCollection.H \
MEDSPLITTER_MESHCollectionDriver.H \
MEDSPLITTER_MESHCollectionMedXMLDriver.H \
MEDSPLITTER_MESHCollectionMedAsciiDriver.H \
MEDSPLITTER_MESHCollectionDriver.hxx \
MEDSPLITTER_MESHCollectionMedXMLDriver.hxx \
MEDSPLITTER_MESHCollectionMedAsciiDriver.hxx \
MEDSPLITTER_ParallelTopology.hxx \
MEDSPLITTER_FaceModel.hxx \
MEDSPLITTER_Graph.hxx\
MEDSPLITTER_UserGraph.hxx\
MEDSPLITTER_API.hxx \
MEDSPLITTER_SequentialTopology.hxx \
MEDSPLITTER_utils.hxx \
MEDSPLITTER.hxx

if MED_ENABLE_METIS
    salomeinclude_HEADERS+= MEDSPLITTER_METISGraph.hxx
endif
if MED_ENABLE_SCOTCH
    salomeinclude_HEADERS+= MEDSPLITTER_SCOTCHGraph.hxx
endif

dist_libmedsplitter_la_SOURCES= \
MEDSPLITTER_MESHCollection.cxx \
MEDSPLITTER_MESHCollectionDriver.cxx \
MEDSPLITTER_MESHCollectionMedXMLDriver.cxx \
MEDSPLITTER_MESHCollectionMedAsciiDriver.cxx \
MEDSPLITTER_ParallelTopology.cxx \
MEDSPLITTER_Graph.cxx\
MEDSPLITTER_UserGraph.cxx\
MEDSPLITTER_API.cxx

if MED_ENABLE_METIS
    dist_libmedsplitter_la_SOURCES+= MEDSPLITTER_METISGraph.cxx
endif
if MED_ENABLE_SCOTCH
    dist_libmedsplitter_la_SOURCES+= MEDSPLITTER_SCOTCHGraph.cxx
endif

libmedsplitter_la_CPPFLAGS= $(MED2_INCLUDES) $(HDF5_INCLUDES) @CXXTMPDPTHFLAGS@ \
	$(BOOST_CPPFLAGS) $(LIBXML_INCLUDES) \
	-I$(srcdir)/../MEDMEM -I$(srcdir)/../MEDWrapper/V2_1/Core \
	-I$(srcdir)/../INTERP_KERNEL/Bases

libmedsplitter_la_LDFLAGS= 
#libmedsplitter_la_LDFLAGS= $(MED2_LIBS) $(HDF5_LIBS) $(STDLIB) $(LIBXML_LIBS) \
#	../MEDMEM/libmedmem.la ../MEDWrapper/V2_1/Core/libmed_V2_1.la

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

libmedsplitter_la_LDFLAGS+= $(MED2_LIBS) $(HDF5_LIBS) $(STDLIB) $(LIBXML_LIBS) \
	../MEDMEM/libmedmem.la ../MEDWrapper/V2_1/Core/libmed_V2_1.la ../INTERP_KERNEL/libinterpkernel.la

# Executables targets
bin_PROGRAMS= medsplitter

dist_medsplitter_SOURCES= medsplitter.cxx

medsplitter_CPPFLAGS= $(libmedsplitter_la_CPPFLAGS)
medsplitter_LDADD= $(libmedsplitter_la_LDFLAGS) -lm $(BOOST_LIBS) libmedsplitter.la
if MED_ENABLE_KERNEL
  medsplitter_LDADD+= -lSALOMEBasics
endif

OBSOLETE_FILES = \
	MEDSPLITTER_SequentialTopology.cxx \
	test_HighLevelAPI.cxx

EXTRA_DIST += $(OBSOLETE_FILES)
