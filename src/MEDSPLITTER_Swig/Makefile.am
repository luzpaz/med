# Copyright (C) 2007-2012  CEA/DEN, EDF R&D
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

#  MED MEDMEM_SWIG : binding of C++ implementation and Python
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

salomepython_PYTHON = libMEDSPLITTER_Swig.py
salomepyexec_LTLIBRARIES = _libMEDSPLITTER_Swig.la

salomeinclude_HEADERS =  \
	libMEDSPLITTER_Swig.i

SWIG_DEF = libMEDSPLITTER_Swig.i

SWIG_FLAGS = @SWIG_FLAGS@ -I$(srcdir) -I$(srcdir)/../MEDMEM_SWIG

dist__libMEDSPLITTER_Swig_la_SOURCES   = libMEDSPLITTER_Swig.i
nodist__libMEDSPLITTER_Swig_la_SOURCES = libMEDSPLITTER_Swig_wrap.cxx
libMEDSPLITTER_Swig.py: libMEDSPLITTER_Swig_wrap.cxx

libMEDSPLITTER_Swig_wrap.cxx: $(SWIG_DEF)
	$(SWIG) $(SWIG_FLAGS) -o $@  $<

_libMEDSPLITTER_Swig_la_CPPFLAGS = $(CORBA_CXXFLAGS) $(CORBA_INCLUDES) $(PYTHON_INCLUDES) \
	$(MED3_INCLUDES) $(HDF5_INCLUDES) @CXXTMPDPTHFLAGS@ $(BOOST_CPPFLAGS) \
	-I$(srcdir)/../MEDMEM -I$(srcdir)/../MEDWrapper/V2_1/Core -I$(srcdir)/../INTERP_KERNEL \
	-I$(srcdir)/../MEDSPLITTER -I$(srcdir)/../INTERP_KERNEL/Bases

_libMEDSPLITTER_Swig_la_LDFLAGS  = -module $(MED3_LIBS_C_ONLY) $(HDF5_LIBS) $(PYTHON_LIBS) $(BOOST_LIBS) \
	../MEDMEM/libmedmem.la ../INTERP_KERNEL/libinterpkernel.la \
	../MEDSPLITTER/libmedsplitter.la

if MED_ENABLE_KERNEL
  _libMEDSPLITTER_Swig_la_CPPFLAGS += ${KERNEL_CXXFLAGS}
  _libMEDSPLITTER_Swig_la_LDFLAGS  += ${KERNEL_LDFLAGS} -lSALOMELocalTrace
endif

CLEANFILES = libMEDSPLITTER_Swig_wrap.cxx libMEDSPLITTER_Swig.py

dist_salomescript_DATA   = MEDSPLITTER.py test_MEDSPLITTER.py
