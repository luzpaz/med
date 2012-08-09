// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

#include "MEDMEM_define.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEMTest_Utils.hxx"

using namespace std;
using namespace MEDMEM;

int main (int argc, char** argv)
{
  string filename = getResourceFile("pointe.med");
  string meshname = "maa1";

  MESH * aMesh = new MESH;
  aMesh->setName(meshname);
  MED_MESH_RDONLY_DRIVER aMeshDriver (filename, aMesh);
  aMeshDriver.setMeshName(meshname);
  aMeshDriver.open();
  aMeshDriver.read();
  aMeshDriver.close();

  const FAMILY * aFamily1 = aMesh->getFamily(MED_EN::MED_NODE, 1);

  // check default constructor and operator=
  FAMILY *aFamily4=new FAMILY;

  // (BUG) Wrong implementation or usage of PointerOf<string>.
  //       Do not use memcpy() with array of std::string!
  //       Impossible to use FAMILY::operator=!
  *aFamily4 = *aFamily1;
  aFamily4->removeReference();

  /*{
    int nb = 3;
    string * str = new string[nb];

    char tmp_str [32];
    for (int i = 0; i < nb; i++) {
      sprintf(tmp_str, "My String N° %d", i+1);
      str[i] = tmp;
    }

    // bad
    string* _pointer;
    _pointer = new string[3];
    // This is wrong, because class string is not simple type
    // and can have pointers to some data, deallocated in it's destructor,
    // which (data) will not be copied by this operation.
    memcpy(_pointer, str, 3*sizeof(string));
    delete [] _pointer;

    // good
    //PointerOf<int> p1 (1);
    //PointerOf<int> p2 (20);
    //p2 = newAttributeValue;
    //p1.set(3, p2);

    // bad
    //PointerOf<string> p1 (1);
    //PointerOf<string> p2 (20);
    //p2 = str;
    //p1.set(3, p2);

    delete [] str;
  }
  */

  aMesh->removeReference();
}
