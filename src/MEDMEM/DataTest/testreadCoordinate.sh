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

#  File   : testreadCoordinate.sh
#  Module : MED
#
SALOME=${HOME}/V08
SALOME_LIB=${SALOME}/Build/lib
SALOME_PATH=${SALOME}/Build/MED/src/MEDMEM
LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${SALOME_LIB}
RESULT=PourMail

rm -rf ${RESULT}
echo " - Comparaison des Coordonnees : " >> ${RESULT}
echo -e " -------------------------------\n">> ${RESULT}

npb=0;
num=1
while [ true ]
do
	ligne=`sed -n -e"$num,$num p" Maillages.txt`
	num=`expr $num + 1 `
	if [ "$ligne" = "" ] 
	then 
		break 
	fi
	if [ `echo $ligne | cut -c1 ` = "#" ] 
	then 
		echo "COMMENTAIRE" 
		continue
	fi
	fich=`echo $ligne | cut -f1 -d":"`
	maill=`echo $ligne | cut -f2 -d":"`
	nb=`echo $ligne | cut -f3 -d":"`
	if [ "$nb" = "" ]
        then
		Result=Rmem/Rmem.${fich}.coor.dump
		Compare=Ref/${fich}.coor.dump
	else
		Result=Rmem/Rmem.${fich}.${nb}.coor.dump
		Compare=Ref/${fich}.${nb}.coor.dump
	fi
	${SALOME_PATH}/readCoordinate Data/${fich} "${maill}" > $Result

	echo "   Maillage " $maill "lu dans " $fich  >> ${RESULT} 
	diff $Result $Compare >> /dev/null
	rc=$?
        if [ "$rc" != "0" ]
        then
           nb=`diff $Result $Compare| grep -v "Type de repere des coordonnees" | wc -l`
           nb=`expr $nb + 0 `
           if [ "$nb" != "2" ]
           then
		echo " Difference entre les resultats des deux methodes de dumps : " >> ${RESULT}
		echo " Fichiers compares : $Result et $Compare " >> ${RESULT}
		diff $Result $Compare  >> ${RESULT}
		echo -e "\n">> ${RESULT}
           	npb=`expr $npb + 1 `
	   else
		echo "   Pas de différence sur les coordonnees 			== test OK == " >> ${RESULT}
	   fi
	else
	  echo "   Pas de différence sur les coordonnees 			== test OK == " >> ${RESULT}
	fi
	echo -e "\n" >> ${RESULT}
done
