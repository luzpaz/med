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
#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_ModulusArray.hxx"
#include "MEDMEM_MedMeshDriver.hxx"
#include <vector>

#define MESHNAME "poly3D"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

#define DIM_OF_FIELD 3


class SupportTester 
{
private:
  const int *_tabOfNodes;
  vector<int> _eltsActiveYet;
  vector<int> _lgthOfEltsActiveYet;
public:
  SupportTester(const int *tabOfNodes, int nbOfElts, int nbOfNodesPerElt);
  SupportTester(const int *tabOfNodes, int nbOfElts, const int *nbOfNodesPerElt);
  bool isIncludedAndNotAlreadyConsumed(const int *tabOfNodesOfTheElementToTest);
  bool areAllEltsConsumed();
private:
  static bool areEquivalent(const int *nodes1, const int *nodes2, int nbOfNodes);
};

SupportTester::SupportTester(const int *tabOfNodes, int nbOfElts, int nbOfNodesPerElt):_tabOfNodes(tabOfNodes)
{
  for(int i=0;i<nbOfElts;i++)
    {
      _eltsActiveYet.push_back(i*nbOfNodesPerElt);
      _lgthOfEltsActiveYet.push_back(nbOfNodesPerElt);
    }
}

SupportTester::SupportTester(const int *tabOfNodes, int nbOfElts, const int *nbOfNodesPerElt):_tabOfNodes(tabOfNodes)
{
  int offset=0;
  for(int i=0;i<nbOfElts;i++)
    {
      _eltsActiveYet.push_back(offset);
      _lgthOfEltsActiveYet.push_back(nbOfNodesPerElt[i]);
    }
}

bool SupportTester::isIncludedAndNotAlreadyConsumed(const int *tabOfNodesOfTheElementToTest)
{
  vector<int>::iterator iter2=_lgthOfEltsActiveYet.begin();
  for(vector<int>::iterator iter=_eltsActiveYet.begin();iter!=_eltsActiveYet.end();iter++)
    if(areEquivalent(_tabOfNodes+(*iter),tabOfNodesOfTheElementToTest,*iter2))
      {
        _eltsActiveYet.erase(iter);
        _lgthOfEltsActiveYet.erase(iter2);
        return true;
      }
    else
      {
        iter2++;
      }
  return false;
}

bool SupportTester::areAllEltsConsumed()
{
  return _eltsActiveYet.size()==0;
}

bool SupportTester::areEquivalent(const int *nodes1, const int *nodes2, int nbOfNodes)
{
  MEDMODULUSARRAY arr1(nbOfNodes,nodes1);
  MEDMODULUSARRAY arr2(nbOfNodes,nodes2);
  return arr1.compare(arr2)!=0;
}

int main (int argc, char ** argv)
{
  if (argc<2) // after 2, ignored !
    {
      cerr << "Usage : " << argv[0] << " poly3D.med typically in ../../share/salome/resources/med" << endl << endl;
      exit(-1);
    }
  int nbOfPtsForTest=0;
  int i;
  string filename = argv[1];
  MESH * myMesh = new MESH;
  myMesh->setName(MESHNAME);
  MED_MESH_RDONLY_DRIVER myMeshReadDriver(filename,myMesh);
  myMeshReadDriver.setMeshName(MESHNAME);
  myMeshReadDriver.open();
  myMeshReadDriver.read();
  myMeshReadDriver.close();
  //Test 1 : test if connectivity of poly3D mesh is OK
  if(myMesh->getMeshDimension()==3 && myMesh->getSpaceDimension()==3)
    nbOfPtsForTest++;
  if(myMesh->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_TETRA4)==1 && myMesh->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_POLYHEDRA)==2)
    nbOfPtsForTest++;
  const int REFnodalConnForTetra[4]=
    {
      17, 9, 18, 19
    };
  const int *connForTetraToTest=myMesh->getConnectivity(MED_NODAL,MED_CELL,MED_TETRA4);
  const int *connIndForTetraToTest=myMesh->getConnectivityIndex(MED_NODAL,MED_CELL);
  for(i=connIndForTetraToTest[0]-1;i<connIndForTetraToTest[1]-1;i++)
    if(connForTetraToTest[i]==REFnodalConnForTetra[i])
      nbOfPtsForTest++;
  //6
  const int *globIndex=connIndForTetraToTest + 1; // skip 1 tetra
  const int *nodalConnOfFaces=myMesh->getConnectivity(MED_NODAL,MED_CELL,MED_POLYHEDRA);
  if(globIndex[1]-globIndex[0]==46 && globIndex[2]-globIndex[1]==45)// resp 46 nodes and 45 nodes are in polyh 1 and 2.
    nbOfPtsForTest++;
  //7
  const int REFnodalConnOfFaces[91]=
    {
      1, 2, 3, 4, 5, 6, -1,// Polyhedron 1
      1, 7, 8, 2,       -1,
      2, 8, 9, 3,       -1,
      4, 3, 9, 10,      -1,
      5, 4, 10, 11,     -1,
      6, 5, 11, 12,     -1,
      1, 6, 12, 7,      -1,
      7, 12, 8,         -1,
      10, 9, 8, 12, 11,     

      13, 14, 15, 3, 2, -1,// Polyhedron 2
      13, 2, 8, 16,     -1,
      14, 13, 16, 17,   -1,
      15, 14, 17,       -1,
      15, 17, 18,       -1,
      15, 18, 9,        -1,
      3, 15, 9,         -1,
      2, 3, 9, 8,       -1,
      8, 9, 17, 16,     -1,
      9, 18, 17 
    };
  for(i=0;i<74;i++)
    if(REFnodalConnOfFaces[i]==nodalConnOfFaces[i])
      nbOfPtsForTest++;
  if(nbOfPtsForTest!=7+74)
    {
      cout << "TEST1 K0 ! : Invalid Globaldata in memory..." << endl;
      return 1;
    }
  // TEST 2 : FAMILY 
  nbOfPtsForTest=0;
  vector<FAMILY*> families=myMesh->getFamilies(MED_FACE);
  if(families.size()==3)
    nbOfPtsForTest++;
  vector<FAMILY *>::iterator iter=families.begin();
  FAMILY *fam1=*(iter++);
  FAMILY *fam2=*(iter++);
  FAMILY *fam3=*(iter);
  const int *nbs;
  // family 1
  if(fam1->getNumberOfTypes()==1 && fam1->getTypes()[0]==MED_POLYGON && fam1->getNumberOfElements(MED_ALL_ELEMENTS)==3)
    nbOfPtsForTest++;
  nbs=fam1->getNumber(MED_ALL_ELEMENTS);
  const int REFTabForPolyg[16]=
    {
      1, 2, 3, 4, 5, 6, 10, 9, 8, 12, 11, 13, 14, 15, 3, 2
    };
  const int REFTabForPolygLgth[3]=
    {
      6,5,5
    };
  SupportTester test1(REFTabForPolyg,3,REFTabForPolygLgth);
  for(i=0;i<3;i++)
    {
      int lgth;
      const int *conn=((CONNECTIVITY *)myMesh->getConnectivityptr())->getConnectivityOfAnElement(MED_NODAL,MED_FACE,nbs[i],lgth);
      if(test1.isIncludedAndNotAlreadyConsumed(conn))
        nbOfPtsForTest++;
    }
  if(test1.areAllEltsConsumed())
    nbOfPtsForTest++;
  // family 2
  if(fam2->getNumberOfElements(MED_ALL_ELEMENTS)==8)
    nbOfPtsForTest++;
  nbs=fam2->getNumber(MED_ALL_ELEMENTS);
  const int REFTabForQuad[32]=
    {
      1, 7, 8, 2, 2, 8, 9, 3, 4, 3, 9, 10, 5, 4, 10, 11, 6, 5, 11, 12, 1, 6, 12, 7, 14, 13, 16, 17, 8, 9, 17, 16
    };
  SupportTester test2(REFTabForQuad,8,4);
  for(i=0;i<8;i++)
    {
      int lgth;
      const int *conn=((CONNECTIVITY *)myMesh->getConnectivityptr())->getConnectivityOfAnElement(MED_NODAL,MED_FACE,nbs[i],lgth);
      if(test2.isIncludedAndNotAlreadyConsumed(conn))
        nbOfPtsForTest++;
    }
  if(test2.areAllEltsConsumed())
    nbOfPtsForTest++;
  // family 3
  if(fam3->getNumberOfElements(MED_ALL_ELEMENTS)==6)
    nbOfPtsForTest++;
  nbs=fam3->getNumber(MED_ALL_ELEMENTS);
  const int REFTabForTria[18]=
    {
      7, 12, 8, 15, 14, 17, 15, 17, 18, 15, 18, 9, 3, 15, 9, 18, 17, 9
    };
  SupportTester test3(REFTabForTria,6,3);
  for(i=0;i<6;i++)
    {
      int lgth;
      const int *conn=((CONNECTIVITY *)myMesh->getConnectivityptr())->getConnectivityOfAnElement(MED_NODAL,MED_FACE,nbs[i],lgth);
      if(test3.isIncludedAndNotAlreadyConsumed(conn))
        nbOfPtsForTest++;
    }
  if(test3.areAllEltsConsumed())
    nbOfPtsForTest++;
  if(nbOfPtsForTest!=21)
    {
      cout << "TEST2 K0 ! : Invalid data in memory for families !!!" << endl;
      return 1;
    }
  // TEST 3 : volumes, areas, barycenter
  nbOfPtsForTest=0;
  const SUPPORT *supOnCell=myMesh->getSupportOnAll(MED_CELL);
  FIELD<double>* vol1=myMesh->getVolume(supOnCell, false);
  int lgth=vol1->getValueLength();
  const double *vals=vol1->getValue();
  if(lgth==3)
    nbOfPtsForTest++;
  const double REFVolOfPolyHedron[3]=
    {
      2.333333333333333,-11.66666666666666,-13.83224131414673
    };
  for(i=0;i<3;i++)
    if(fabs(REFVolOfPolyHedron[i]-vals[i])<1e-12)
      nbOfPtsForTest++;
  vol1->removeReference();
  vol1=myMesh->getVolume(supOnCell, true);
  lgth=vol1->getValueLength();
  vals=vol1->getValue();
  if(lgth==3)
    nbOfPtsForTest++;
  for(i=0;i<3;i++)
    if(fabs(fabs(REFVolOfPolyHedron[i])-vals[i])<1e-12)
      nbOfPtsForTest++;
  vol1->removeReference();
  FIELD<double>* bary=myMesh->getBarycenter(supOnCell);
  lgth=bary->getValueLength();
  vals=bary->getValue();
  if(lgth==9)
    nbOfPtsForTest++;
  const double REFBaryOfPolyHedron[9]= 
    {
      5.5, 1, -1, 2, 1.5, 1.0833333333333333, 5.1, 1.6, 0.9
    };
  for(i=0;i<9;i++)
    if(fabs(REFBaryOfPolyHedron[i]-vals[i])<1e-12)
      nbOfPtsForTest++;
  bary->removeReference();
  //area
  vol1=myMesh->getArea(fam1);
  lgth=vol1->getValueLength();
  vals=vol1->getValue();
  if(lgth==3)
    nbOfPtsForTest++;
  const double REFAreaForPolyg[3]=
    {
      6,5,6.5
    };
  for(i=0;i<3;i++)
    if(fabs(REFAreaForPolyg[i]-vals[i])<1e-12)
      nbOfPtsForTest++;
  vol1->removeReference();

  vol1=myMesh->getArea(fam2);
  lgth=vol1->getValueLength();
  vals=vol1->getValue();
  if(lgth==8)
    nbOfPtsForTest++;
  const double REFAreaForQuad[8]=
    {
      2.1213203435596424, 2.8284271247461903, 4.4721359549995796, 4.4721359549995796, 
      2.8284271247461903, 2.1213203435596428, 3.6798724963767362, 4
    };
  for(i=0;i<8;i++)
    if(fabs(REFAreaForQuad[i]-vals[i])<1e-12)
      nbOfPtsForTest++;
  vol1->removeReference();

  vol1=myMesh->getArea(fam3);
  lgth=vol1->getValueLength();
  vals=vol1->getValue();
  if(lgth==6)
    nbOfPtsForTest++;
  const double REFAreaForTri[6]=
    {
      2.9580398915498081, 1.4142135623730951, 2.2360679774997898, 
      3.3541019662496847, 3.3541019662496847, 2.2360679774997898
    };
  for(i=0;i<6;i++)
    if(fabs(REFAreaForTri[i]-vals[i])<1e-12)
      nbOfPtsForTest++;
  vol1->removeReference();
  if(nbOfPtsForTest!=38)
    {
      cout << "TEST3 K0 ! : Error in calculation of basic properties !!!" << endl;
      return 1;
    }
  // TEST 4 -- CHECK FOR Reverse descending using getBoundaryElements.
  nbOfPtsForTest=0;
  SUPPORT *bound=myMesh->getBoundaryElements(MED_NODE);
  if(bound->getNumberOfElements(MED_ALL_ELEMENTS)==19)
    nbOfPtsForTest++;
  if(bound->isOnAllElements())
    nbOfPtsForTest++;
  if(nbOfPtsForTest!=2)
    {
      cout << "TEST4 K0 ! : Error in getBoundaryElements probably due to Reverse descending !!!" << endl;
      return 1;
    }
  bound->removeReference();
  ///////////
  cout << "ALL TESTS OK !!!" << endl;
  myMesh->removeReference();
  return 0;
}
