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

#define private public
#define protected public
#include "TESTMEDCLIENT_Gen_i.hxx"
#include "MESHClient.hxx"
#include "FIELDClient.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Field.hxx"

#include "MEDMEM_Family.hxx"
#include "FAMILYClient.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Array.hxx"
#include "MEDMEM_PointerOf.hxx"

#include "MemorySpy.hxx"

using namespace std;

extern "C"
{
  PortableServer::ObjectId *TESTMEDCLIENTEngine_factory(CORBA::ORB_ptr orb,
                                                        PortableServer::POA_ptr poa,
                                                        PortableServer::ObjectId * contId, 
                                                        const char *instanceName, 
                                                        const char *interfaceName)
  {
    TESTMEDCLIENT_Gen_i *ret=new TESTMEDCLIENT_Gen_i(orb,poa,contId,instanceName,interfaceName);
    return ret->getId();
  }
}

TESTMEDCLIENT_Gen_i::TESTMEDCLIENT_Gen_i(CORBA::ORB_ptr orb,
            PortableServer::POA_ptr poa,
            PortableServer::ObjectId * contId, 
            const char *instanceName, 
                                         const char *interfaceName):Engines_Component_i(orb,poa,contId,instanceName,interfaceName)
{
  _thisObj = this ;
  _id = _poa->activate_object(_thisObj);
}
  
TESTMEDCLIENT_Gen_i::~TESTMEDCLIENT_Gen_i()
{
}
  
void TESTMEDCLIENT_Gen_i::go(SALOME_MED::MED_ptr objMed)
{
  cerr << "Begin of test 1" << endl;
  SALOME_MED::MESH_ptr maillagePtr=objMed->getMeshByName("CUBE_EN_HEXA8_QUAD4");
  MESHClient mesh(maillagePtr);
  maillagePtr->setProtocol(SALOME::SOCKET_);
  mesh.fillCopy();

  long n=mesh.getNumberOfNodes();
  long dim=mesh.getMeshDimension();
  cout << "Mesh nodes nb :" << n << " dim : " << dim << endl;
  const double *tabRet=mesh.getCoordinates(MED_NO_INTERLACE);
  for(int k=0;k<n;k++)
    {
      for(int l=0;l<dim;l++)
        cout << *(tabRet++) << " ";
      cout << endl;
    }
  int nbOfElt=mesh.getNumberOfTypes(MED_FACE);
  cout << "____" <<  nbOfElt << endl;
  SUPPORT* sup1=new SUPPORT(&mesh,"MonSup",MED_FACE);
  nbOfElt=sup1->getNumberOfElements(MED_QUAD4);
  FIELD<double>* fd=mesh.getArea(sup1);
  delete sup1;
  int nbOfVal=fd->getNumberOfValues();
  int nbOfCpt=fd->getNumberOfComponents();
  cout << "nbOfVal " << nbOfVal << " nbOfCpt " << nbOfCpt << endl;
  const double *tabAera=fd->getValue(MED_NO_INTERLACE);
  for(int m=0;m<nbOfVal;m++)
    cout << "Aera " << m << " = " << tabAera[m] << endl;
  delete fd;

  const vector<FAMILY*> fams=mesh.getFamilies(MED_FACE);
  cout << "Nb Of FACES families :" << fams.size() << " " << mesh.getNumberOfFamilies(MED_FACE)  << endl;
  MEDSKYLINEARRAY *numb=fams[0]->getnumber();
  cout << "const MEDSKYLINEARRAY *numb" << endl;
  cout << "Length of : " << numb->getLength() << endl;
  const int *vec2=numb->getValue();
  for(int m=0;m<numb->getLength();m++)
    {
      cout << vec2[m] << " " << endl;
    }
  const vector<GROUP*>  groups=mesh.getGroups(MED_FACE);
  cout << "Nb Of FACES groups :" << groups.size() << endl;
  const int * tabConec=mesh.getConnectivity(MED_FULL_INTERLACE,MED_NODAL,MED_FACE,MED_QUAD4);
  for(int p=0;p<nbOfElt;p++){
    for(int p1=0;p1<4;p1++)
      {
        cout << tabConec[4*p+p1] << " ";
      }
    cout << endl;
  }
  cout << endl;


  SALOME_MED::string_array* strArray=objMed->getFieldNames();
  for(int r=0;r<strArray->length();r++)
    cout << (*strArray)[r] << endl;
  SALOME_MED::FIELD_var myField=objMed->getField((*strArray)[1],2,-1);
  if(myField==SALOME_MED::FIELD::_nil())
    cout << "big problem ... " << endl;
  SALOME_MED::FIELDDOUBLE_ptr myFieldD=SALOME_MED::FIELDDOUBLE::_narrow(myField);
  if(myFieldD==SALOME_MED::FIELDDOUBLE::_nil())
    cout << "not fielddouble " << (*strArray)[1] << endl;
  FIELDClient<double,SALOME_MED::FIELDDOUBLE_ptr> myFieldDouble(myFieldD);
  delete strArray;
  const SUPPORT *supField=myFieldDouble.getSupport();
  int nbOfValField=supField->getNumberOfElements(MED_NONE);

  const double * values = myFieldDouble.getValue(MED_FULL_INTERLACE);
  for(int r2=0;r2<myFieldDouble.getNumberOfComponents()*nbOfValField;r2++)
    cout << values[r2] << " ";
  cout << endl;
}

void TESTMEDCLIENT_Gen_i::go2(SALOME_MED::MED_ptr objMed)
{
  cerr << "Begin of test 2" << endl;
  SALOME_MED::string_array_var strA=objMed->getMeshNames();
  cout << strA[0] << endl;
  SALOME_MED::MESH_ptr maillagePtr;
  MemorySpy spy;
  cout << "Mem0 : " << spy.getCurrentMemoryUsage() << endl;
  maillagePtr=objMed->getMeshByName("cube_tetra4");
  MESHClient* mesh=new MESHClient(maillagePtr);
  cout << "Mem1 : " << spy.getCurrentMemoryUsage() << endl;
  mesh->fillCopy();
  cout << "Mem2 : " << spy.getCurrentMemoryUsage() << endl;
  delete mesh;
  cout << "Mem3 : " << spy.getCurrentMemoryUsage() << endl;
  SALOME_MED::string_array_var strB=objMed->getFieldNames();
  cout << "________________" << endl;
  cout << "il y a " << strB->length() << " champs" << endl;
  for(int i=0;i<strB->length();i++)
    cout << strB[i] << endl;
  cout << "Field beeing get " << strB[0] << endl;
  SALOME_MED::FIELD_var myField=objMed->getField(strB[0],-1,-1);
  SALOME_MED::FIELDDOUBLE_ptr myFieldD=SALOME_MED::FIELDDOUBLE::_narrow(myField);
  if(myFieldD==SALOME_MED::FIELDDOUBLE::_nil())
    cout << "not fielddouble " << strB[0] << endl;
  else
    cout << "Field " << strB[0] << " is double" << endl;
  //myFieldD->setProtocol(SALOME::MPI_);
  FIELDClient<double,SALOME_MED::FIELDDOUBLE_ptr> *myFieldDouble=new FIELDClient<double,SALOME_MED::FIELDDOUBLE_ptr>(myFieldD);
  cout << "Mem3bis : " << spy.getCurrentMemoryUsage() << endl;
  const SUPPORT *supField=myFieldDouble->getSupport();
  int nbOfValField=supField->getNumberOfElements(MED_TETRA4);
  
  cout << "Mem4 : " << spy.getCurrentMemoryUsage() << endl;
  const double * values = myFieldDouble->getValue(MED_FULL_INTERLACE);
//   values= myFieldDouble->getValue(MED_FULL_INTERLACE);
//   const  MEDARRAY<double>* valAr=myFieldDouble->getvalue();
//   double* ptOf=valAr->_valuesNo._pointer;
//   double* ptOf2=valAr->_valuesFull._pointer;
  cout << "Mem5 : " << spy.getCurrentMemoryUsage() << " -- " << myFieldDouble->getNumberOfComponents() << " " << nbOfValField  << endl;
//   cout << valAr->_ldValues << " " << valAr->_lengthValues << endl;
  for(int r2=0;r2<myFieldDouble->getNumberOfComponents()*nbOfValField;r2++)
    {
//       for(int r2j=0;r2j<nbOfValField;r2j++)
      {
//      cout << ptOf2[r2j*3+r2i] << " " << ptOf[r2i*nbOfValField+r2j] << " | ";
              double val = values[r2];
              if ( (val < 1.0) || (val > 100.0))
                cout << val << " ";
      }
    }
  delete myFieldDouble;
  cout << "Mem6 : " << spy.getCurrentMemoryUsage() << endl;
}
