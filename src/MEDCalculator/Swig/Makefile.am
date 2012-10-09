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
# Author : Anthony Geay (CEA/DEN)

#  MED MEDMEM_SWIG : binding of C++ implementation and Python
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

bin_PROGRAMS = spython

lib_LTLIBRARIES = libmedcalculatorspython.la _MEDCalculator.la 

salomeinclude_HEADERS =  \
	MEDCalculator.i MEDCalculatorTypemaps.i SPythonInterpreter.hxx SPythonParser.hxx MedCalculatorSPythonDefines.hxx

SWIG_DEF = MEDCalculator.i MEDCalculatorTypemaps.i

SWIG_FLAGS = @SWIG_FLAGS@ -I$(srcdir) -I$(srcdir)/.. -I$(srcdir)/../../MEDCoupling \
	-I$(srcdir)/../../MEDCoupling_Swig -I$(srcdir)/../../MEDLoader -I$(srcdir)/../../MEDLoader/Swig \
	-I$(srcdir)/../../INTERP_KERNEL -I$(srcdir)/../../INTERP_KERNEL/Bases

dist__MEDCalculator_la_SOURCES   = $(SWIG_DEF)
nodist__MEDCalculator_la_SOURCES = MEDCalculator_wrap.cxx
MEDCalculator.py: MEDCalculator_wrap.cxx

MEDCalculator_wrap.cxx: $(SWIG_DEF)
	$(SWIG) $(SWIG_FLAGS) -o $@  $<

_MEDCalculator_la_CPPFLAGS = $(PYTHON_INCLUDES) \
	$(MED3_INCLUDES) $(HDF5_INCLUDES) @CXXTMPDPTHFLAGS@ \
	-I$(srcdir)/../../INTERP_KERNEL \
	-I$(srcdir)/.. -I$(srcdir)/../../MEDCoupling_Swig -I$(srcdir)/../../INTERP_KERNEL/Bases \
	-I$(srcdir)/../../MEDCoupling -I$(srcdir)/../../MEDLoader -I$(srcdir)/../../MEDLoader/Swig

_MEDCalculator_la_LDFLAGS  = -module $(MED3_LIBS_C_ONLY) $(HDF5_LIBS) $(PYTHON_LIBS) \
	../libmedcalculator.la

dist_libmedcalculatorspython_la_SOURCES = SPythonInterpreter.cxx SPythonParser.cxx

libmedcalculatorspython_la_CPPFLAGS = $(PYTHON_INCLUDES) -I$(srcdir)/../../INTERP_KERNEL/Bases \
				      -I$(srcdir)/../../INTERP_KERNEL	 

libmedcalculatorspython_la_LDFLAGS = $(PYTHON_LIBS) ../../INTERP_KERNEL/libinterpkernel.la

spython_CPPFLAGS = $(PYTHON_INCLUDES)

spython_LDFLAGS = $(PYTHON_LIBS)
spython_LDADD   = libmedcalculatorspython.la

dist_spython_SOURCES = spython.cxx

CLEANFILES = MEDCalculator_wrap.cxx MEDCalculator.py

dist_salomescript_DATA= MEDCalculator.py MEDCalculatorBasicsTest.py test2.spy

UNIT_TEST_PROG = MEDCalculatorBasicsTest.py
