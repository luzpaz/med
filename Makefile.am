#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
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

# -* Makefile *- 
# Author : Patrick GOLDBRONN (CEA)
# Date : 28/06/2001
# $Header$
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

if MED_ENABLE_KERNEL
if MED_ENABLE_GUI
  ACLOCAL_AMFLAGS = -I adm_local/unix/config_files			\
                    -I ${KERNEL_ROOT_DIR}/salome_adm/unix/config_files	\
                    -I ${GUI_ROOT_DIR}/adm_local/unix/config_files
else !MED_ENABLE_GUI
  ACLOCAL_AMFLAGS = -I adm_local/unix/config_files			\
                    -I ${KERNEL_ROOT_DIR}/salome_adm/unix/config_files
endif
else !MED_ENABLE_KERNEL
if MED_ENABLE_GUI
  ACLOCAL_AMFLAGS = -I adm_local/unix/config_files			\
                    -I adm_local_without_kernel/unix/config_files	\
                    -I ${GUI_ROOT_DIR}/adm_local/unix/config_files
else !MED_ENABLE_GUI
  ACLOCAL_AMFLAGS = -I adm_local/unix/config_files			\
                    -I adm_local_without_kernel/unix/config_files
endif
endif

if MED_ENABLE_KERNEL
  IDLDIR = idl
endif

SUBDIRS = adm_local

if !MED_ENABLE_KERNEL
SUBDIRS += adm_local_without_kernel
endif

SUBDIRS += $(IDLDIR) src doc resources bin

DIST_SUBDIRS = adm_local adm_local_without_kernel idl src doc resources bin

DISTCLEANFILES = a.out aclocal.m4 configure local-install.sh adm_local/unix/config_files/config.guess adm_local/unix/config_files/config.sub adm_local/unix/config_files/depcomp adm_local/unix/config_files/install-sh adm_local/unix/config_files/libtool.m4 adm_local/unix/config_files/ltmain.sh adm_local/unix/config_files/ltoptions.m4 adm_local/unix/config_files/ltsugar.m4 adm_local/unix/config_files/ltversion.m4 adm_local/unix/config_files/lt~obsolete.m4 adm_local/unix/config_files/missing adm_local/unix/config_files/py-compile

salomeinclude_DATA = MED_version.h

EXTRA_DIST += build_configure clean_configure LICENCE INSTALL README

dist-hook:
	rm -rf `find $(distdir) -name CVS`

dev_docs:
	(cd doc && $(MAKE) $(AM_MAKEFLAGS) dev_docs)
