#  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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

###
# ParaMEDMEM is enabled if MPI is detected ok
# In addition, ParaMEDMEM_Swig is not available in MICROMED mode
###

if MPI_IS_OK
  PARAMEDMEM_SUBDIRS = ParaMEDMEM
endif

###
# Other packages are not available in MICROMED mode
###

if !MED_ENABLE_MICROMED

  BASE_SUBDIRS = MEDWrapper MEDMEM MEDCoupling_Swig INTERPOLATION MEDMEM_SWIG MEDMEMBinTest

if MPI_IS_OK
    PARAMEDMEM_SUBDIRS += ParaMEDMEM_Swig
endif

if CPPUNIT_IS_OK
    BASE_SUBDIRS += MEDMEMCppTest
endif

if MED_ENABLE_SPLITTER
    SPLITTER_SUBDIRS = MEDSPLITTER MEDSPLITTER_Swig

if MED_ENABLE_METIS
      MULTIPR_SUBDIRS = MULTIPR
endif
if MED_ENABLE_SCOTCH
      MULTIPR_SUBDIRS = MULTIPR
endif

endif

if MED_ENABLE_KERNEL
    KERNEL_SUBDIRS = $(MULTIPR_SUBDIRS) MEDCouplingCorba MEDMEM_I MED MedCorba_Swig MED_SWIG MedClient

if MPI_IS_OK
  KERNEL_SUBDIRS += ParaMEDMEMComponent
endif

endif

if MED_ENABLE_GUI
    GUI_SUBDIRS = MEDGUI
endif

endif

SUBDIRS = $(COMMON_SUBDIRS) $(PARAMEDMEM_SUBDIRS) $(BASE_SUBDIRS) $(INTERPTEST_SUBDIRS) \
          $(SPLITTER_SUBDIRS) $(KERNEL_SUBDIRS) $(GUI_SUBDIRS)

DIST_SUBDIRS= MEDWrapper INTERP_KERNEL MEDMEM MEDCoupling MEDCoupling_Swig INTERPOLATION MEDMEM_SWIG    \
              MEDMEMBinTest MEDMEMCppTest INTERP_KERNELTest ParaMEDMEM ParaMEDMEM_Swig                  \
	      MEDSPLITTER MEDSPLITTER_Swig MULTIPR MEDCouplingCorba MEDMEM_I MED MED_SWIG MedCorba_Swig \
	      MedClient MEDGUI
