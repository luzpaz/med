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
# -* Makefile *- 
# Author : Patrick GOLDBRONN (CEA)
# Date : 28/06/2001
# $Header$
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

SUBDIRS = adm_local 

if MED_ENABLE_KERNEL
      SUBDIRS+= idl
endif

SUBDIRS+= src

if MED_ENABLE_GUI
      SUBDIRS+= doc
endif

SUBDIRS+= resources bin
DIST_SUBDIRS= adm_local adm_local_without_kernel idl src doc resources bin

DISTCLEANFILES= a.out aclocal.m4 configure 

salomeinclude_DATA = MED_version.h

EXTRA_DIST+= build_configure clean_configure LICENCE INSTALL README

dist-hook:
	rm -rf `find . -name CVS`

if MED_ENABLE_SSTREAM
  install-data-local: $(pkgincludedir)/SALOMEconfig.h
else !MED_ENABLE_SSTREAM
  install-data-local: $(pkgincludedir)/SALOMEconfig.h $(pkgincludedir)/sstream
endif

$(pkgincludedir)/SALOMEconfig.h: ./salome_adm/unix/SALOMEconfig.h
	cp -f $^ $(pkgincludedir)

$(pkgincludedir)/sstream: ./include/salome/sstream
	cp -f $^ $(pkgincludedir)
