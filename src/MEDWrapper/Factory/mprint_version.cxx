// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

extern "C"{
#include <med.h>
}

#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
  med_idt aFid = MEDouvrir(argv[1],MED_LECTURE);
  if(aFid < 0)
    exit(1);

  med_int aMajor, aMinor, aRelease;
  med_err aRet = MEDversionLire(aFid,&aMajor,&aMinor,&aRelease);
  MEDfermer(aFid);
  if(aRet < 0)
    exit(2);

  printf("%d.%d.%d\n",aMajor,aMinor,aRelease);
}
