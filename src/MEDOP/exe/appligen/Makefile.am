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

# author: Guillaume Boulant (EDF/R&D, 2012)

include $(top_srcdir)/adm_local/unix/make_common_starter.am

appligenbindir = $(bindir)/@MODULE_NAME@/appligen

appligenbin_DATA= \
	envappli.sh \
	config_appli.xml       \
	appli-splashscreen.jpg \
	SalomeApp.xml

appligenbin_SCRIPTS=\
	appligen.sh

EXTRA_DIST += $(appligenbin_DATA) $(appligenbin_SCRIPTS) genenv.sh README.txt

config_appli.xml: config_appli_model.xml
	cat $< | sed \
	-e s~__GEOM_ROOT_DIR__~$(GEOM_ROOT_DIR)~g \
	-e s~__SMESH_ROOT_DIR__~$(SMESH_ROOT_DIR)~g \
	-e s~__BLSURFPLUGIN_ROOT_DIR__~$(BLSURFPLUGIN_ROOT_DIR)~g \
	-e s~__GHS3DPLUGIN_ROOT_DIR__~$(GHS3DPLUGIN_ROOT_DIR)~g \
	-e s~__NETGENPLUGIN_ROOT_DIR__~$(NETGENPLUGIN_ROOT_DIR)~g \
	-e s~__VISU_ROOT_DIR__~$(VISU_ROOT_DIR)~g \
	-e s~__YACS_ROOT_DIR__~$(YACS_ROOT_DIR)~g \
	-e s~__PARAVIS_ROOT_DIR__~$(PARAVIS_ROOT_DIR)~g \
	> tmp; mv tmp $@

envappli.sh:
	$(srcdir)/genenv.sh envappli.sh


mostlyclean-local:
	rm -f @builddir@/config_appli.xml @builddir@/envappli.sh
