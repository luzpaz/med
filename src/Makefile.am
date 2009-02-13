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

SUBDIRS = MEDWrapper INTERP_KERNEL MEDMEM MEDCoupling INTERPOLATION MEDMEM_SWIG MEDMEMBinTest $(MPIDIR)

if CPPUNIT_IS_OK
 SUBDIRS+= MEDMEMCppTest INTERP_KERNELTest
endif

if MPI_IS_OK
  SUBDIRS+= ParaMEDMEM
endif

if MED_ENABLE_SPLITTER
  SUBDIRS+= MEDSPLITTER MEDSPLITTER_Swig


# MULTIPR
if MED_ENABLE_METIS
  MULTIPR_DIR = MULTIPR
endif
if MED_ENABLE_SCOTCH
  MULTIPR_DIR = MULTIPR
endif

endif

if MED_ENABLE_KERNEL
  SUBDIRS+= $(MULTIPR_DIR) MEDMEM_I MED MedCorba_Swig MED_SWIG # MedClient
endif

if MED_ENABLE_GUI
  SUBDIRS+= MEDGUI
endif

DIST_SUBDIRS= MEDWrapper INTERPOLATION MEDCoupling MEDMEM INTERP_KERNEL MEDMEM_SWIG MEDMEMBinTest MEDMEMCppTest MEDSPLITTER MULTIPR ParaMEDMEM ParaMEDMEM_Swig MEDMEM_I MED MedCorba_Swig MED_SWIG MedClient MEDGUI
