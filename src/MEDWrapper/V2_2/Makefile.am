# Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
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

#  File   : 
#  Author : 
#  Module : 
#  $Header$
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

lib_LTLIBRARIES= libMEDWrapper_V2_2.la

salomeinclude_HEADERS = \
	MED_V2_2_Wrapper.hxx

dist_libMEDWrapper_V2_2_la_SOURCES = \
	MED_V2_2_Wrapper.cxx

libMEDWrapper_V2_2_la_CPPFLAGS= $(BOOST_CPPFLAGS) $(MED2_INCLUDES) -I$(srcdir)/../Base
if MED_ENABLE_KERNEL
  libMEDWrapper_V2_2_la_CPPFLAGS+= -I$(KERNEL_ROOT_DIR)/include/salome
else
  libMEDWrapper_V2_2_la_CPPFLAGS+= -I$(top_builddir)/adm_local_without_kernel/unix
endif
libMEDWrapper_V2_2_la_LDFLAGS= $(MED2_LIBS) $(HDF5_LIBS)
libMEDWrapper_V2_2_la_LIBADD= ../Base/libMEDWrapperBase.la
