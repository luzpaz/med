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

include $(top_srcdir)/adm_local/unix/make_common_starter.am

###
# Common subdirectories
###

COMMON_SUBDIRS = INTERP_KERNEL MEDCoupling 

###
# Unitary tests for INTERP_KERNEL
### 

if CPPUNIT_IS_OK
 INTERPTEST_SUBDIRS = INTERP_KERNELTest
endif

if MED_ENABLE_MED3
  COMMON_SUBDIRS += MEDLoader
endif

###
# ParaMEDMEM is enabled if MPI is detected ok
# In addition, ParaMEDMEM_Swig is not available in MICROMED mode
###
PARAMEDMEM_SUBDIRS =

if MPI_IS_OK
  PARAMEDMEM_SUBDIRS += ParaMEDMEM
if MED_ENABLE_MED3
    PARAMEDMEM_SUBDIRS += ParaMEDLoader
if CPPUNIT_IS_OK
      PARAMEDMEM_SUBDIRS += ParaMEDMEMTest
endif
endif
endif

###
# Other packages are not available in MICROMED mode
###

if !MED_ENABLE_MICROMED

  BASE_SUBDIRS = MEDWrapper MEDMEM MEDCoupling_Swig MEDMEM_SWIG MEDMEMBinTest

if MPI_IS_OK
    PARAMEDMEM_SUBDIRS += ParaMEDMEM_Swig
endif

if CPPUNIT_IS_OK
    BASE_SUBDIRS += MEDMEMCppTest
endif

if MED_ENABLE_SPLITTER
    SPLITTER_SUBDIRS = MEDSPLITTER MEDSPLITTER_Swig MEDPartitioner
endif

if MED_ENABLE_RENUMBER
    RENUMBER_SUBDIRS = RENUMBER
endif

if MED_ENABLE_KERNEL
    KERNEL_SUBDIRS = MEDCouplingCorba MEDCouplingCorba_Swig MEDMEM_I MED MedCorba_Swig MED_SWIG MedClient

if MPI_IS_OK
  KERNEL_SUBDIRS += ParaMEDCouplingCorba ParaMEDMEMComponent
endif

if MED_ENABLE_MED3

  KERNEL_SUBDIRS += MEDCalculator
endif

endif

if MED_ENABLE_GUI
    GUI_SUBDIRS = MEDGUI MEDOP
endif

endif

SUBDIRS = $(COMMON_SUBDIRS) $(PARAMEDMEM_SUBDIRS) $(BASE_SUBDIRS) $(INTERPTEST_SUBDIRS) \
          $(SPLITTER_SUBDIRS) $(RENUMBER_SUBDIRS) $(KERNEL_SUBDIRS) $(GUI_SUBDIRS)

DIST_SUBDIRS= \
	MEDWrapper INTERP_KERNEL MEDMEM MEDCoupling MEDCoupling_Swig MEDMEM_SWIG    \
	MEDMEMBinTest MEDMEMCppTest INTERP_KERNELTest ParaMEDMEM ParaMEDMEM_Swig MEDLoader \
	MEDSPLITTER MEDSPLITTER_Swig MEDPartitioner RENUMBER MEDCouplingCorba MEDCouplingCorba_Swig \
	MEDMEM_I MED MED_SWIG MedCorba_Swig ParaMEDLoader MedClient ParaMEDMEMComponent \
	ParaMEDCouplingCorba MEDCalculator MEDGUI MEDOP ParaMEDMEMTest
