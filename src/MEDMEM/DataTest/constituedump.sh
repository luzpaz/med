#!/bin/sh
# Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
#
# Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
# CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#  File   : constituedump.sh
#  Module : MED
#
cd Data
for i in `ls *.med`
do
	echo "constitution de $i : 1er maillage"
	mdump $i > ${i}.coor.dump << EOF
1
1
1
EOF
cp ${i}.coor.dump ${i}.ent.dump
done

for i in `echo zzzz121b.med`
do
	echo "constitution de $i : 2nd maillage"
	mdump $i > ${i}.2.coor.dump << EOF
1
1
2
EOF
cp ${i}.2.coor.dump ${i}.2.ent.dump
done

mv *.dump ../Ref
cd ../Ref

for j in `ls *.ent.dump`
do
echo "Traitement de $j  : "
vi ${j} << EOF
:/INFORMATIONS GENERALES
:-2
:1,.d
:/Nombre de familles
:.,\$d
:wq
EOF
done

for j in `ls *.coor.dump`
do
echo "Traitement de $j  : "
vi ${j} << EOF
:/NOEUDS DU MAILLAGE
:-2
:1,.d
:/Coordonnees des noeuds
: +2
:.,\$d
:wq
EOF
done


