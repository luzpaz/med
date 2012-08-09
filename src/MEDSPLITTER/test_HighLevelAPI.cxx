// Copyright (C) 2007-2012  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
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
#include "MEDSPLITTER_API.hxx"
#include <string>

using namespace std;
int main()
{
  string testname="MEDSPLITTER - test #3 -";

  char filename[100] = "../../share/salome/resources/med/carre_en_quad4.med";
  char meshname[20]  = "carre_en_quad4";
  char output[100]="./tests/API_test";
  int is_distributed=0;
  int nprocs=4;
  int meshonly=0;
  int method=0;
  return medsplitter(filename, 
                     meshname,
                     output,  
                     is_distributed,
                     nprocs,
                     method,
                     meshonly);
}

