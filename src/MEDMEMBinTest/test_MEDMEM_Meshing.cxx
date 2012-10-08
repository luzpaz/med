// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

//#define protected public
//#define private public
//
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Field.hxx"
#ifdef WIN32
#include <io.h>
#define access _access
#define W_OK 02
#endif

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;
using namespace DRIVERFACTORY;

int main (int argc, char ** argv)
{
  if (argc >2)
  {
    cerr << "Usage : " << argv[0] 
         << " filenameRoot" << endl;
    cerr << "        where filenameRoot is a root filename, the program will produce" << endl;
    cerr << "        3 files filenameRoot21.med filenameRoot22.med and filenameRoot.vtk" << endl << endl;
    exit(-1);
  }

  string filenameRoot;
  if (argc==1)
  {
    // filename to save the generated MESH
    if ( getenv("TMP") && access(getenv("TMP"),W_OK)==0 )
      filenameRoot=getenv("TMP");
    else if (getenv("TMPDIR") && access(getenv("TMPDIR"),W_OK)==0 )
      filenameRoot=getenv("TMPDIR");
    else
      filenameRoot="/tmp";
    filenameRoot += "/test_MEDMEM_Meshing" ;
  }
  else
    filenameRoot=argv[1];

  string filenameMed21 = filenameRoot+"_V21.med";
  string filenameMed22 = filenameRoot+"_V22.med";
  string filenameVtk = filenameRoot+".vtk";

  MESHING *myMeshing=new MESHING;
  myMeshing->setName("meshing") ;

  // define coordinates

  int SpaceDimension = 3 ;
  int NumberOfNodes = 19 ;
  double Coordinates[57] =
    {
      0.0, 0.0, 0.0, 
      0.0, 0.0, 1.0, 
      2.0, 0.0, 1.0, 
      0.0, 2.0, 1.0, 
      -2.0, 0.0, 1.0, 
      0.0, -2.0, 1.0, 
      1.0, 1.0, 2.0, 
      -1.0, 1.0, 2.0, 
      -1.0, -1.0, 2.0, 
      1.0, -1.0, 2.0, 
      1.0, 1.0, 3.0, 
      -1.0, 1.0, 3.0, 
      -1.0, -1.0, 3.0, 
      1.0, -1.0, 3.0, 
      1.0, 1.0, 4.0, 
      -1.0, 1.0, 4.0, 
      -1.0, -1.0, 4.0, 
      1.0, -1.0, 4.0,
      0.0, 0.0, 5.0
    };

  myMeshing->setCoordinates(SpaceDimension,NumberOfNodes,Coordinates,"CARTESIAN",MED_FULL_INTERLACE);

  string Names[3] =
    {
      "X","Y","Z"
    } ;
  myMeshing->setCoordinatesNames(Names);

  string Units[3] =
    {
      "cm","cm","cm"
    } ;
  myMeshing->setCoordinatesUnits(Units) ;

  // define conectivities

  // cell part

  const int NumberOfTypes = 3 ;
  medGeometryElement Types[NumberOfTypes] = 
    {
      MED_TETRA4,MED_PYRA5,MED_HEXA8
    } ;
  const int NumberOfElements[NumberOfTypes] = 
    {
      12,2,2
    } ;

  myMeshing->setNumberOfTypes(NumberOfTypes,MED_CELL);
  myMeshing->setTypes(Types,MED_CELL);
  myMeshing->setNumberOfElements(NumberOfElements,MED_CELL);

  const int sizeTetra = 12*4 ;
  int ConnectivityTetra[sizeTetra]=
    {
      1,2,3,6,
      1,2,4,3,
      1,2,5,4,
      1,2,6,5,
      2,7,4,3,
      2,8,5,4,
      2,9,6,5,
      2,10,3,6,
      2,7,3,10,
      2,8,4,7,
      2,9,5,8,
      2,10,6,9
    };

  myMeshing->setConnectivity(MED_CELL,MED_TETRA4,ConnectivityTetra);

  int ConnectivityPyra[2*5]=
    {
      7,8,9,10,2,
      15,18,17,16,19
    };

  myMeshing->setConnectivity(MED_CELL,MED_PYRA5,ConnectivityPyra);

  int ConnectivityHexa[2*8]=
    {
      11,12,13,14,7,8,9,10,
      15,16,17,18,11,12,13,14
    };

  myMeshing->setConnectivity(MED_CELL,MED_HEXA8,ConnectivityHexa);

  // face part

  const int NumberOfFacesTypes = 2 ;
  medGeometryElement FacesTypes[NumberOfFacesTypes] =
    {
      MED_TRIA3,MED_QUAD4
    } ;
  const int NumberOfFacesElements[NumberOfFacesTypes] =
    {
      4,4
    } ;

  myMeshing->setNumberOfTypes(NumberOfFacesTypes,MED_FACE);
  myMeshing->setTypes(FacesTypes,MED_FACE);
  myMeshing->setNumberOfElements(NumberOfFacesElements,MED_FACE);

  const int sizeTria = 3*4 ;
  int ConnectivityTria[sizeTria]=
    {
      1,4,3,
      1,5,4,
      1,6,5,
      1,3,6
    };
  myMeshing->setConnectivity(MED_FACE,MED_TRIA3,ConnectivityTria);

  int ConnectivityQua[4*4]=
    {
      7,8,9,10,
      11,12,13,14,
      11,7,8,12,
      12,8,9,13
    };

  myMeshing->setConnectivity(MED_FACE,MED_QUAD4,ConnectivityQua);

  // edge part

  // not yet implemented : if set, results are unpredictable.

  // Some groups :

  // Node :

  GROUP *partialGroupOnNodes=new GROUP;
  {
    partialGroupOnNodes->setName("SomeNodes");
    partialGroupOnNodes->setMesh(myMeshing);
    partialGroupOnNodes->setEntity(MED_NODE);
    partialGroupOnNodes->setNumberOfGeometricType(1);
    medGeometryElement myTypes[1] = 
      {
        MED_NONE
      };
    partialGroupOnNodes->setGeometricType(myTypes);
    const int myNumberOfElements[1] = 
      {
        4
      } ;
    partialGroupOnNodes->setNumberOfElements(myNumberOfElements);
    const int index[1+1] = 
      {
        1,5
      } ;
    const int value[4]= 
      {
        1,4,5,7
      } ;
    partialGroupOnNodes->setNumber(index,value);
  }
  GROUP *partialGroupOnNodes2=new GROUP;
  {
    partialGroupOnNodes2->setName("OtherNodes");
    partialGroupOnNodes2->setMesh(myMeshing);
    partialGroupOnNodes2->setEntity(MED_NODE);
    partialGroupOnNodes2->setNumberOfGeometricType(1);
    medGeometryElement myTypes[1] = 
      {
        MED_NONE
      };
    partialGroupOnNodes2->setGeometricType(myTypes);
    const int myNumberOfElements[1] = 
      {
        3
      } ;
    partialGroupOnNodes2->setNumberOfElements(myNumberOfElements);
    const int index[1+1] = 
      {
        1,4
      } ;
    const int value[3]= 
      {
        2,3,6
      } ;
    partialGroupOnNodes2->setNumber(index,value);
  }
  myMeshing->addGroup(*partialGroupOnNodes2);


  // Cell :

  GROUP *partialGroupOnCells=new GROUP;
  {
    partialGroupOnCells->setName("SomeCells");
    partialGroupOnCells->setMesh(myMeshing);
    partialGroupOnCells->setEntity(MED_CELL);
    partialGroupOnCells->setNumberOfGeometricType(3);
    medGeometryElement myTypes[3] = 
      {
        MED_TETRA4,MED_PYRA5,MED_HEXA8
      };
    partialGroupOnCells->setGeometricType(myTypes);
    const int myNumberOfElements[3] = 
      {
        4,1,2
      } ;
    partialGroupOnCells->setNumberOfElements(myNumberOfElements);
    const int index[3+1] = 
      {
        1,5,6,8
      } ;
    const int value[4+1+2]=
      {
        2,7,8,12,
        13,
        15,16
      };
    partialGroupOnCells->setNumber(index,value);
  }
  myMeshing->addGroup(*partialGroupOnCells);


  GROUP *partialGroupOnCells2=new GROUP;
  {
    partialGroupOnCells2->setName("OtherCells");
    partialGroupOnCells2->setMesh(myMeshing);
    partialGroupOnCells2->setEntity(MED_CELL);
    partialGroupOnCells2->setNumberOfGeometricType(2);
    medGeometryElement myTypes[] = 
      {
        MED_TETRA4,MED_PYRA5
      };
    partialGroupOnCells2->setGeometricType(myTypes);
    const int myNumberOfElements[] = 
      {
        4,1
      } ;
    partialGroupOnCells2->setNumberOfElements(myNumberOfElements);
    const int index[2+1] = 
      {
        1,5,6
      } ;
    const int value[4+1]=
      {
        3,4,5,9,
        14
      };
    partialGroupOnCells2->setNumber(index,value);
  }
  myMeshing->addGroup(*partialGroupOnCells2);

  // Face :

  GROUP *partialGroupOnFaces=new GROUP;
  {
    partialGroupOnFaces->setName("SomeFaces");
    partialGroupOnFaces->setMesh(myMeshing);
    partialGroupOnFaces->setEntity(MED_FACE);
    partialGroupOnFaces->setNumberOfGeometricType(2);
    medGeometryElement myTypes[2] = 
      {
        MED_TRIA3,MED_QUAD4
      };
    partialGroupOnFaces->setGeometricType(myTypes);
    const int myNumberOfElements[2] = 
      {
        2,3
      } ;
    partialGroupOnFaces->setNumberOfElements(myNumberOfElements);
    const int index[2+1] = 
      {
        1,3,6
      } ;
    const int value[2+3]=
      {
        2,4,
        5,6,8
      } ;
    partialGroupOnFaces->setNumber(index,value);
  }
  myMeshing->addGroup(*partialGroupOnFaces);


  GROUP *partialGroupOnFaces2=new GROUP;
  {
    partialGroupOnFaces2->setName("OtherFaces");
    partialGroupOnFaces2->setMesh(myMeshing);
    partialGroupOnFaces2->setEntity(MED_FACE);
    medGeometryElement myTypes[1] = 
      {
        MED_TRIA3
      };
    int myNumberOfElements[1] = 
      {
        2
      } ;
    int value[2]=
      {
        1,3
      } ;
    //    partialGroupOnFaces2->setNumber(index,value);
    partialGroupOnFaces2->setpartial("description",1,2,myTypes,myNumberOfElements,value);
  }
  myMeshing->addGroup(*partialGroupOnFaces2);

  // all right, we save it in Med 2.1 2.2 and vtk !

  int idMed22 = myMeshing->addDriver(MED_DRIVER,filenameMed22,myMeshing->getName());
  myMeshing->write(idMed22) ;

  int idVtk = myMeshing->addDriver(VTK_DRIVER,filenameVtk,myMeshing->getName());
  myMeshing->write(idVtk) ;

  // we build now 8 fields : 4 fields double (integer) :
  //                         2 fields on nodes (cells) :
  //                         1 scalar (vector)

  const SUPPORT * supportOnNodes = myMeshing->getSupportOnAll(MED_NODE);
  int numberOfNodes = supportOnNodes->getNumberOfElements(MED_ALL_ELEMENTS);

  const SUPPORT * supportOnCells = myMeshing->getSupportOnAll(MED_CELL);
  myMeshing->removeReference();
  int numberOfCells = supportOnCells->getNumberOfElements(MED_ALL_ELEMENTS);

  FIELD<double> * fieldDoubleScalarOnNodes = new FIELD<double>(supportOnNodes,1);
  fieldDoubleScalarOnNodes->setName("fieldScalarDoubleNode");
  fieldDoubleScalarOnNodes->setIterationNumber(-1);
  fieldDoubleScalarOnNodes->setOrderNumber(-1);
  fieldDoubleScalarOnNodes->setTime(0.0);

  fieldDoubleScalarOnNodes->setComponentName(1,"Vx");
  fieldDoubleScalarOnNodes->setComponentDescription(1,"comp1");
  fieldDoubleScalarOnNodes->setMEDComponentUnit(1,"unit1");
  FIELD<double>* fieldDoubleScalarOnPartialNodes = new FIELD<double>(partialGroupOnNodes,1);
  fieldDoubleScalarOnPartialNodes->setName("fieldScalarDoublePartialNodes");
  fieldDoubleScalarOnPartialNodes->setIterationNumber(-1);
  fieldDoubleScalarOnPartialNodes->setOrderNumber(-1);
  fieldDoubleScalarOnPartialNodes->setTime(0.0);
  fieldDoubleScalarOnPartialNodes->setComponentName(1,"Vx");
  fieldDoubleScalarOnPartialNodes->setComponentDescription(1,"comp1");
  fieldDoubleScalarOnPartialNodes->setMEDComponentUnit(1,"unit1");


  FIELD<double> * fieldDoubleVectorOnNodes = new FIELD<double>(supportOnNodes,SpaceDimension);
  fieldDoubleVectorOnNodes->setName("fieldVectorDoubleNode");
  fieldDoubleVectorOnNodes->setIterationNumber(-1);
  fieldDoubleVectorOnNodes->setOrderNumber(-1);
  fieldDoubleVectorOnNodes->setTime(0.0);

  fieldDoubleVectorOnNodes->setComponentName(1,"Vx");
  fieldDoubleVectorOnNodes->setComponentDescription(1,"comp1");
  fieldDoubleVectorOnNodes->setMEDComponentUnit(1,"unit1");
  fieldDoubleVectorOnNodes->setComponentName(2,"Vy");
  fieldDoubleVectorOnNodes->setComponentDescription(2,"comp2");
  fieldDoubleVectorOnNodes->setMEDComponentUnit(2,"unit2");
  fieldDoubleVectorOnNodes->setComponentName(3,"Vz");
  fieldDoubleVectorOnNodes->setComponentDescription(3,"comp3");
  fieldDoubleVectorOnNodes->setMEDComponentUnit(3,"unit3");

  FIELD<double> * fieldDoubleScalarOnCells = new FIELD<double>(supportOnCells,1);
  fieldDoubleScalarOnCells->setName("fieldScalarDoubleCell");
  fieldDoubleScalarOnCells->setIterationNumber(-1);
  fieldDoubleScalarOnCells->setOrderNumber(-1);
  fieldDoubleScalarOnCells->setTime(0.0);

  fieldDoubleScalarOnCells->setComponentName(1,"Vx");
  fieldDoubleScalarOnCells->setComponentDescription(1,"comp1");
  fieldDoubleScalarOnCells->setMEDComponentUnit(1,"unit1");


  FIELD<double> * fieldDoubleScalarOnPartialCells = new FIELD<double>(partialGroupOnCells,1);
  fieldDoubleScalarOnPartialCells->setName("fieldScalarDoublePartialCell");
  fieldDoubleScalarOnPartialCells->setIterationNumber(-1);
  fieldDoubleScalarOnPartialCells->setOrderNumber(-1);
  fieldDoubleScalarOnPartialCells->setTime(0.0);

  fieldDoubleScalarOnPartialCells->setComponentName(1,"Vx");
  fieldDoubleScalarOnPartialCells->setComponentDescription(1,"comp1");
  fieldDoubleScalarOnPartialCells->setMEDComponentUnit(1,"unit1");

  FIELD<double> * fieldDoubleScalarOnPartialFaces = new FIELD<double>(partialGroupOnFaces,1);
  fieldDoubleScalarOnPartialFaces->setName("fieldScalarDoublePartialFace");
  fieldDoubleScalarOnPartialFaces->setIterationNumber(-1);
  fieldDoubleScalarOnPartialFaces->setOrderNumber(-1);
  fieldDoubleScalarOnPartialFaces->setTime(0.0);
  fieldDoubleScalarOnPartialFaces->setComponentName(1,"Vx");
  fieldDoubleScalarOnPartialFaces->setComponentDescription(1,"comp1");
  fieldDoubleScalarOnPartialFaces->setMEDComponentUnit(1,"unit1");

  FIELD<double> * fieldDoubleVectorOnPartialFaces = new FIELD<double>(partialGroupOnFaces2,2);
  fieldDoubleVectorOnPartialFaces->setName("fieldVectorDoublePartialFace");
  fieldDoubleVectorOnPartialFaces->setIterationNumber(-1);
  fieldDoubleVectorOnPartialFaces->setOrderNumber(-1);
  fieldDoubleVectorOnPartialFaces->setTime(0.0);
  fieldDoubleVectorOnPartialFaces->setComponentName(1,"Vx");
  fieldDoubleVectorOnPartialFaces->setComponentName(2,"Vy");

  fieldDoubleVectorOnPartialFaces->setComponentDescription(1,"comp1");
  fieldDoubleVectorOnPartialFaces->setComponentDescription(2,"comp2");

  fieldDoubleVectorOnPartialFaces->setMEDComponentUnit(1,"unit1");
  fieldDoubleVectorOnPartialFaces->setMEDComponentUnit(2,"unit2");


  FIELD<double> * fieldDoubleVectorOnCells = new FIELD<double>(supportOnCells,SpaceDimension);
  fieldDoubleVectorOnCells->setName("fieldVectorrDoubleCell");
  fieldDoubleVectorOnCells->setIterationNumber(-1);
  fieldDoubleVectorOnCells->setOrderNumber(-1);
  fieldDoubleVectorOnCells->setTime(0.0);

  fieldDoubleVectorOnCells->setComponentName(1,"Vx");
  fieldDoubleVectorOnCells->setComponentDescription(1,"comp1");
  fieldDoubleVectorOnCells->setMEDComponentUnit(1,"unit1");
  fieldDoubleVectorOnCells->setComponentName(2,"Vy");
  fieldDoubleVectorOnCells->setComponentDescription(2,"comp2");
  fieldDoubleVectorOnCells->setMEDComponentUnit(2,"unit2");
  fieldDoubleVectorOnCells->setComponentName(3,"Vz");
  fieldDoubleVectorOnCells->setComponentDescription(3,"comp3");
  fieldDoubleVectorOnCells->setMEDComponentUnit(3,"unit3");

  FIELD<int> * fieldIntScalarOnNodes = new FIELD<int>(supportOnNodes,1);
  fieldIntScalarOnNodes->setName("fieldScalarIntNode");
  fieldIntScalarOnNodes->setIterationNumber(-1);
  fieldIntScalarOnNodes->setOrderNumber(-1);
  fieldIntScalarOnNodes->setTime(0.0);

  fieldIntScalarOnNodes->setComponentName(1,"Vx");
  fieldIntScalarOnNodes->setComponentDescription(1,"comp1");
  fieldIntScalarOnNodes->setMEDComponentUnit(1,"unit1");

  FIELD<int> * fieldIntVectorOnNodes = new FIELD<int>(supportOnNodes,SpaceDimension);
  fieldIntVectorOnNodes->setName("fieldVectorIntNode");
  fieldIntVectorOnNodes->setIterationNumber(-1);
  fieldIntVectorOnNodes->setOrderNumber(-1);
  fieldIntVectorOnNodes->setTime(0.0);

  fieldIntVectorOnNodes->setComponentName(1,"Vx");
  fieldIntVectorOnNodes->setComponentDescription(1,"comp1");
  fieldIntVectorOnNodes->setMEDComponentUnit(1,"unit1");
  fieldIntVectorOnNodes->setComponentName(2,"Vy");
  fieldIntVectorOnNodes->setComponentDescription(2,"comp2");
  fieldIntVectorOnNodes->setMEDComponentUnit(2,"unit2");
  fieldIntVectorOnNodes->setComponentName(3,"Vz");
  fieldIntVectorOnNodes->setComponentDescription(3,"comp3");
  fieldIntVectorOnNodes->setMEDComponentUnit(3,"unit3");

  FIELD<int> * fieldIntScalarOnCells = new FIELD<int>(supportOnCells,1);
  fieldIntScalarOnCells->setName("fieldScalarIntCell");
  fieldIntScalarOnCells->setIterationNumber(-1);
  fieldIntScalarOnCells->setOrderNumber(-1);
  fieldIntScalarOnCells->setTime(0.0);

  fieldIntScalarOnCells->setComponentName(1,"Vx");
  fieldIntScalarOnCells->setComponentDescription(1,"comp1");
  fieldIntScalarOnCells->setMEDComponentUnit(1,"unit1");

  FIELD<int> * fieldIntVectorOnCells = new FIELD<int>(supportOnCells,SpaceDimension);
  fieldIntVectorOnCells->setName("fieldVectorrIntCell");
  fieldIntVectorOnCells->setIterationNumber(-1);
  fieldIntVectorOnCells->setOrderNumber(-1);
  fieldIntVectorOnCells->setTime(0.0);

  fieldIntVectorOnCells->setComponentName(1,"Vx");
  fieldIntVectorOnCells->setComponentDescription(1,"comp1");
  fieldIntVectorOnCells->setMEDComponentUnit(1,"unit1");
  fieldIntVectorOnCells->setComponentName(2,"Vy");
  fieldIntVectorOnCells->setComponentDescription(2,"comp2");
  fieldIntVectorOnCells->setMEDComponentUnit(2,"unit2");
  fieldIntVectorOnCells->setComponentName(3,"Vz");
  fieldIntVectorOnCells->setComponentDescription(3,"comp3");
  fieldIntVectorOnCells->setMEDComponentUnit(3,"unit3");

  for (int i = 0; i<numberOfNodes; i++)
  {
    double valueDbl1, valueDbl2, valueDbl3;
    int valueInt1, valueInt2, valueInt3;
    valueInt1 = i+1;
    valueInt2 = i+2;
    valueInt3 = i+3;
    valueDbl1 = valueInt1*0.1;
    valueDbl2 = valueInt2*0.1;
    valueDbl3 = valueInt3*0.1;
    fieldDoubleScalarOnNodes->setValueIJ(i+1,1,valueDbl1);

    fieldIntScalarOnNodes->setValueIJ(i+1,1,valueInt1);

    fieldDoubleVectorOnNodes->setValueIJ(i+1,1,valueDbl1);
    fieldDoubleVectorOnNodes->setValueIJ(i+1,2,valueDbl2);
    fieldDoubleVectorOnNodes->setValueIJ(i+1,3,valueDbl3);

    fieldIntVectorOnNodes->setValueIJ(i+1,1,valueInt1);
    fieldIntVectorOnNodes->setValueIJ(i+1,2,valueInt2);
    fieldIntVectorOnNodes->setValueIJ(i+1,3,valueInt3);
  }

  for (int i = 0; i<numberOfCells; i++)
  {
    double valueDbl1, valueDbl2, valueDbl3;
    int valueInt1, valueInt2, valueInt3;
    valueInt1 = i+1;
    valueInt2 = i+2;
    valueInt3 = i+3;
    valueDbl1 = valueInt1*0.1;
    valueDbl2 = valueInt2*0.1;
    valueDbl3 = valueInt3*0.1;
    fieldDoubleScalarOnCells->setValueIJ(i+1,1,valueDbl1);

    fieldIntScalarOnCells->setValueIJ(i+1,1,valueInt1);

    fieldDoubleVectorOnCells->setValueIJ(i+1,1,valueDbl1);
    fieldDoubleVectorOnCells->setValueIJ(i+1,2,valueDbl2);
    fieldDoubleVectorOnCells->setValueIJ(i+1,3,valueDbl3);

    fieldIntVectorOnCells->setValueIJ(i+1,1,valueInt1);
    fieldIntVectorOnCells->setValueIJ(i+1,2,valueInt2);
    fieldIntVectorOnCells->setValueIJ(i+1,3,valueInt3);
  }

  for (int i=0; i<partialGroupOnNodes->getNumberOfElements(MED_ALL_ELEMENTS); i++)
  {
    const int* number=partialGroupOnNodes->getNumber(MED_ALL_ELEMENTS);
    fieldDoubleScalarOnPartialNodes->setValueIJ(number[i],1,i+1);
  }

  for (int i=0; i<partialGroupOnCells->getNumberOfElements(MED_ALL_ELEMENTS); i++)
  {
    const int* number=partialGroupOnCells->getNumber(MED_ALL_ELEMENTS);
    fieldDoubleScalarOnPartialCells->setValueIJ(number[i],1,i+1);
  }

  for (int i=0; i<partialGroupOnFaces->getNumberOfElements(MED_ALL_ELEMENTS); i++)
  {
    const int* number=partialGroupOnFaces->getNumber(MED_ALL_ELEMENTS);
    fieldDoubleScalarOnPartialFaces->setValueIJ(number[i],1,i+1);
  }

  for (int i=0; i<partialGroupOnFaces2->getNumberOfElements(MED_ALL_ELEMENTS); i++)
  {
    const int* number=partialGroupOnFaces2->getNumber(MED_ALL_ELEMENTS);
    fieldDoubleVectorOnPartialFaces->setValueIJ(number[i],1,i+1);
    fieldDoubleVectorOnPartialFaces->setValueIJ(number[i],2,-i-1);
  }

  idMed22 = fieldDoubleScalarOnNodes->addDriver(MED_DRIVER,filenameMed22,fieldDoubleScalarOnNodes->getName());
  fieldDoubleScalarOnNodes->write(idMed22) ;

  idMed22 = fieldIntScalarOnNodes->addDriver(MED_DRIVER,filenameMed22,fieldIntScalarOnNodes->getName());
  fieldIntScalarOnNodes->write(idMed22) ;

  idMed22 = fieldDoubleVectorOnNodes->addDriver(MED_DRIVER,filenameMed22,fieldDoubleVectorOnNodes->getName());
  fieldDoubleVectorOnNodes->write(idMed22) ;

  idMed22 = fieldIntVectorOnNodes->addDriver(MED_DRIVER,filenameMed22,fieldIntVectorOnNodes->getName());
  fieldIntVectorOnNodes->write(idMed22) ;

  idMed22 = fieldDoubleScalarOnCells->addDriver(MED_DRIVER,filenameMed22,fieldDoubleScalarOnCells->getName());
  fieldDoubleScalarOnCells->write(idMed22) ;

  idMed22 = fieldIntScalarOnCells->addDriver(MED_DRIVER,filenameMed22,fieldIntScalarOnCells->getName());
  fieldIntScalarOnCells->write(idMed22) ;

  idMed22 = fieldDoubleVectorOnCells->addDriver(MED_DRIVER,filenameMed22,fieldDoubleVectorOnCells->getName());
  fieldDoubleVectorOnCells->write(idMed22) ;

  idMed22 = fieldIntVectorOnCells->addDriver(MED_DRIVER,filenameMed22,fieldIntVectorOnCells->getName());
  fieldIntVectorOnCells->write(idMed22) ;

  idMed22 = fieldDoubleScalarOnPartialNodes->addDriver(MED_DRIVER,filenameMed22,fieldDoubleScalarOnPartialNodes->getName());
  fieldDoubleScalarOnPartialNodes->write(idMed22) ;

  idMed22 = fieldDoubleScalarOnPartialCells->addDriver(MED_DRIVER,filenameMed22,fieldDoubleScalarOnPartialCells->getName());
  fieldDoubleScalarOnPartialCells->write(idMed22) ;

  idMed22 = fieldDoubleScalarOnPartialFaces->addDriver(MED_DRIVER,filenameMed22,fieldDoubleScalarOnPartialFaces->getName());
  fieldDoubleScalarOnPartialFaces->write(idMed22) ;

  idMed22 = fieldDoubleVectorOnPartialFaces->addDriver(MED_DRIVER,filenameMed22,fieldDoubleVectorOnPartialFaces->getName());
  fieldDoubleVectorOnPartialFaces->write(idMed22) ;


  idVtk = fieldDoubleScalarOnNodes->addDriver(VTK_DRIVER,filenameVtk,fieldDoubleScalarOnNodes->getName());
  fieldDoubleScalarOnNodes->writeAppend(idVtk) ;

  idVtk = fieldIntScalarOnNodes->addDriver(VTK_DRIVER,filenameVtk,fieldIntScalarOnNodes->getName());
  fieldIntScalarOnNodes->writeAppend(idVtk) ;

  idVtk = fieldDoubleVectorOnNodes->addDriver(VTK_DRIVER,filenameVtk,fieldDoubleVectorOnNodes->getName());
  fieldDoubleVectorOnNodes->writeAppend(idVtk) ;

  idVtk = fieldIntVectorOnNodes->addDriver(VTK_DRIVER,filenameVtk,fieldIntVectorOnNodes->getName());
  fieldIntVectorOnNodes->writeAppend(idVtk) ;

  idVtk = fieldDoubleScalarOnCells->addDriver(VTK_DRIVER,filenameVtk,fieldDoubleScalarOnCells->getName());
  fieldDoubleScalarOnCells->writeAppend(idVtk) ;

  idVtk = fieldIntScalarOnCells->addDriver(VTK_DRIVER,filenameVtk,fieldIntScalarOnCells->getName());
  fieldIntScalarOnCells->writeAppend(idVtk) ;

  idVtk = fieldDoubleVectorOnCells->addDriver(VTK_DRIVER,filenameVtk,fieldDoubleVectorOnCells->getName());
  fieldDoubleVectorOnCells->writeAppend(idVtk) ;

  idVtk = fieldIntVectorOnCells->addDriver(VTK_DRIVER,filenameVtk,fieldIntVectorOnCells->getName());
  fieldIntVectorOnCells->writeAppend(idVtk) ;

  if (argc==1) 
  {
    cout << "Remove generated files" << endl;
    remove(filenameMed21.c_str());
    remove(filenameMed22.c_str());
    remove(filenameVtk.c_str());
  }

  fieldDoubleScalarOnNodes->removeReference();
  fieldIntScalarOnNodes->removeReference();
  fieldDoubleVectorOnNodes->removeReference();
  fieldIntVectorOnNodes->removeReference();
  fieldDoubleScalarOnCells->removeReference();
  fieldIntScalarOnCells->removeReference();
  fieldDoubleVectorOnCells->removeReference();
  fieldIntVectorOnCells->removeReference();
  fieldDoubleScalarOnPartialCells->removeReference();
  fieldDoubleScalarOnPartialNodes->removeReference();
  fieldDoubleScalarOnPartialFaces->removeReference();
  fieldDoubleVectorOnPartialFaces->removeReference();

  partialGroupOnNodes->removeReference();
  partialGroupOnNodes2->removeReference();
  partialGroupOnCells->removeReference();
  partialGroupOnCells2->removeReference();
  partialGroupOnFaces->removeReference();
  partialGroupOnFaces2->removeReference();
}
