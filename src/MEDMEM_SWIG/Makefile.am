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

#  MED MEDMEM_SWIG : binding of C++ implementation and Python
#
include $(top_srcdir)/adm_local/unix/make_common_starter.am

salomepython_PYTHON = libMEDMEM_Swig.py
salomepyexec_LTLIBRARIES = _libMEDMEM_Swig.la

salomeinclude_HEADERS =  \
	libMEDMEM_Swig.i \
	my_typemap.i     \
	MEDMEM_SWIG_Templates.hxx 

if MED_SWIG_1_3_21
SWIG_DEF = libMEDMEM_Swig.i dummy.i

dummy_wrap.cxx: libMEDMEM_Swig_wrap.cxx
	sed -e 's/SwigValueWrapper< vector<FAMILY \* > >/vector<FAMILY \* >/g' libMEDMEM_Swig_wrap.cxx > toto
	mv toto libMEDMEM_Swig_wrap.cxx
	sed -e 's/SwigValueWrapper< vector<SUPPORT \* > const >/vector<SUPPORT \* >/g' libMEDMEM_Swig_wrap.cxx > toto
	mv toto libMEDMEM_Swig_wrap.cxx
	touch dummy_wrap.cxx
else !MED_SWIG_1_3_21
SWIG_DEF = libMEDMEM_Swig.i
endif

SWIG_FLAGS = @SWIG_FLAGS@ -I$(srcdir)

dist__libMEDMEM_Swig_la_SOURCES   = libMEDMEM_Swig.i
nodist__libMEDMEM_Swig_la_SOURCES = libMEDMEM_Swig_wrap.cxx
libMEDMEM_Swig.py: libMEDMEM_Swig_wrap.cxx

libMEDMEM_Swig_wrap.cxx: $(SWIG_DEF)
	$(SWIG) $(SWIG_FLAGS) -o $@  $<

_libMEDMEM_Swig_la_CPPFLAGS = \
	$(PYTHON_INCLUDES) $(MED3_INCLUDES) $(HDF5_INCLUDES) $(XDR_CPPFLAGS) @CXXTMPDPTHFLAGS@ \
	-I$(srcdir)/../MEDMEM \
	-I$(srcdir)/../INTERP_KERNEL \
	-I$(srcdir)/../INTERP_KERNEL/Bases \
	-I$(srcdir)/../INTERP_KERNEL/GaussPoints

_libMEDMEM_Swig_la_LDFLAGS  = -module $(MED3_LIBS_C_ONLY) $(HDF5_LIBS) $(PYTHON_LIBS) $(XDR_LIBS) \
	../MEDMEM/libmedmem.la ../INTERP_KERNEL/libinterpkernel.la

CLEANFILES = libMEDMEM_Swig_wrap.cxx libMEDMEM_Swig.py

dist_salomescript_DATA   = \
		   medmem.py \
		   med_test1.py \
		   med_test2.py \
		   med_test3.py \
		   med_test_grid.py \
		   med_test_skin.py \
		   medMeshing_test.py \
		   med_field_anal.py \
		   med_opfield_test.py \
		   med_opsupp_test.py \
		   test_gibi.py \
		   test_porflow.py \
		   testDriverAscii.py \
		   testGaussLocalization.py \
		   test_profil_MedFieldDriver.py \
		   testWriteAndFam.py \
		   testMedMemGeneral.py \
		   testMedMemCxxTests.py \
		   dumpMEDMEM.py \
		   ensightMedEnsight_test.py \
		   medEnsightMed_test.py \
		   medNumPy_test.py

dist_salomescript_SCRIPTS = \
		   med2sauv \
		   sauv2med \
		   avs2med

dist_salomepython_DATA = \
		   medutilities.py

#TESTS = medMeshing_test.py medNumPy_test.py 
#TESTS_ENVIRONMENT = $(PYTHON)

install-data-hook:
	@for f in $(dist_salomescript_SCRIPTS) ; do \
          chmod -f a+x $(DESTDIR)$(salomescriptdir)/$$f ; \
	done
