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

#  File   : tous.sh
#  Module : MED
#
SALOME=${HOME}/V08
SALOME_LIB=${SALOME}/Build/lib
SALOME_PATH=${SALOME}/Build/MED/src/MEDMEM
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${SALOME_LIB}
RESULT=cptrendu
rm -rf ${RESULT}

echo " Tests Unitaires " >> $RESULT
echo " --------------- " >> $RESULT
echo " " >> $RESULT

for prog in testUGeoNameMeshEntities testUCellModel testUUnit testUCoordinate
#for prog in testUCoordinate
do

	echo "Passage du test : " ${prog}
	echo "" >> result_test
	echo "Passage du test : " ${prog} >> result_test
	echo "Passage du test : " ${prog}
	${SALOME_PATH}/${prog} >> result_test
	rc=$?
	if [ "$rc" = "0" ]
	then
		echo "Test de " $prog "                    == Test OK" >> $RESULT
		echo " " >> $RESULT
	else
		echo "Test de " $prog "                    NOK !!!!!!" >> $RESULT
	fi
done 
cat $RESULT
