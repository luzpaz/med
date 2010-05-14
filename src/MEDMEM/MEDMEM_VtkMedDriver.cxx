//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "MEDMEM_VtkMedDriver.hxx"

#include <sstream>

#include "MEDMEM_define.hxx"
#include "MEDMEM_Med.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

VTK_MED_DRIVER::VTK_MED_DRIVER(): GENDRIVER(VTK_DRIVER), 
                                  _ptrMed((MED * const)MED_NULL)
{
  //_vtkFile = new ofstream();
  // What about _id in Gendriver ?
  // _driverType ???
}


VTK_MED_DRIVER::VTK_MED_DRIVER(const string & fileName,  MED * const ptrMed):
  GENDRIVER(fileName, MED_EN::RDWR, VTK_DRIVER), _ptrMed(ptrMed)
{
  //_vtkFile = new ofstream();
}

VTK_MED_DRIVER::VTK_MED_DRIVER(const VTK_MED_DRIVER & driver):
  GENDRIVER(driver), 
  _ptrMed(driver._ptrMed)
{
  _ptrMed->addDriver(*this); // OU RECUPERER L'ID.
  //_vtkFile = new ofstream(); 
  // What about _id in Gendriver ?
  // _driverType ???
}

VTK_MED_DRIVER::~VTK_MED_DRIVER()
{
  const char* LOC = "VTK_MED_DRIVER::~VTK_MED_DRIVER()";
  BEGIN_OF_MED(LOC);

  //delete _vtkFile ;

  END_OF_MED(LOC);
}

GENDRIVER * VTK_MED_DRIVER::copy() const
{
  return new VTK_MED_DRIVER(*this) ;
}

//REM :  As t'on besoin du champ _status :  _vtkFile <-> _status  ?  Oui

void VTK_MED_DRIVER::openConst() const {

  const char * LOC ="VTK_MED_DRIVER::open() : ";
  BEGIN_OF_MED(LOC);

  if ( _fileName == "" )
    throw MED_EXCEPTION( LOCALIZED( STRING(LOC) 
                                    << "_fileName is |\"\"|, please set a correct fileName before calling open()"));

  // check if can open the file
  ofstream _vtkFile;
  _vtkFile.open(_fileName.c_str()); 
  if (!_vtkFile)
    throw MED_EXCEPTION( LOCALIZED( STRING(LOC) << "Could not open file "<< _fileName));
  
  END_OF_MED(LOC);
}

void VTK_MED_DRIVER::open() {
  openConst() ;
}

void VTK_MED_DRIVER::closeConst() const {

  const char* LOC = "VTK_MED_DRIVER::close() : ";
  BEGIN_OF_MED(LOC);
  END_OF_MED(LOC);
}

void VTK_MED_DRIVER::close() {
  closeConst() ;
}


void VTK_MED_DRIVER::write() const
{
  const char* LOC = "VTK_MED_DRIVER::write() : ";
  BEGIN_OF_MED(LOC);

  // VTK supports only one dataset per a file (in Simple Legacy Formats)
  // so we write the first mesh only

  int NumberOfMeshes = _ptrMed->getNumberOfMeshes() ;
  deque<string> MeshNames = _ptrMed->getMeshNames() ;

  int NumberOfFields = _ptrMed->getNumberOfFields() ;
  deque<string> FieldNames = _ptrMed->getFieldNames() ;

  //  for ( currentMesh=MeshName.begin();currentMesh != MeshName.end(); currentMesh++) {
  for (int i=0; i<NumberOfMeshes; i++)
    {
      MESH * myMesh = _ptrMed->getMesh(MeshNames[i]) ;
      writeMesh(myMesh) ;
      // get all field which values are on this mesh => revoir api de Med !!!
      // first : field on node
      // fields is on all node !
      for (int j=0; j<NumberOfFields; j++) {
        deque<DT_IT_> timeStep = _ptrMed->getFieldIteration(FieldNames[j]) ;
        deque<DT_IT_>::const_iterator currentTimeStep ;
        for ( currentTimeStep=timeStep.begin(); currentTimeStep!=timeStep.end(); currentTimeStep++) {
          int dt = (*currentTimeStep).dt ;
          int it = (*currentTimeStep).it ;
          FIELD_ * myField = _ptrMed->getField(FieldNames[j],dt,it) ;
          if( MeshNames[i] == myField->getSupport()->getMesh()->getName() ) { 
            // rigth in all case : better compare pointeur ?
            if (MED_NODE == myField->getSupport()->getEntity())
              {
                if (myField->getSupport()->isOnAllElements()) {
                  ostringstream name ; 
                  name << myField->getName() << "_" << dt << "_" << it ;
                  writeField(myField,name.str()) ;
                } else
                  MESSAGE_MED(PREFIX_MED << "Could not write field "<<myField->getName()<<" which is not on all nodes !");
              }
          }
        }
      }

      // second : field on cell
      for (int j=0; j<NumberOfFields; j++) {
        deque<DT_IT_> timeStep = _ptrMed->getFieldIteration(FieldNames[j]) ;
        deque<DT_IT_>::const_iterator currentTimeStep ;
        for ( currentTimeStep=timeStep.begin(); currentTimeStep!=timeStep.end(); currentTimeStep++) {
          int dt = (*currentTimeStep).dt;
          int it = (*currentTimeStep).it;
          FIELD_ * myField = _ptrMed->getField(FieldNames[j],dt,it) ;
          if( MeshNames[i] == myField->getSupport()->getMesh()->getName() ) { 
            // rigth in all case : better compare pointeur ?
            if (MED_CELL == myField->getSupport()->getEntity())
              {
                if (myField->getSupport()->isOnAllElements()) {
                  ostringstream name ; 
                  name << myField->getName() << "_" << dt << "_" << it ;
                  writeField(myField,name.str()) ;
                }
                else
                  MESSAGE_MED(PREFIX_MED << "Could not write field "<<myField->getName()<<" which is not on all cells !");
              }
          }
        }
      }
      // VTK supports only one dataset per a file (in Simple Legacy Formats)
      // so we write the first mesh only
      break;
    } // loop on meshes

  END_OF_MED(LOC);
}

void VTK_MED_DRIVER::writeMesh(MESH * myMesh) const
{
  const char * LOC = "VTK_MED_DRIVER::writeMesh() : ";
  BEGIN_OF_MED(LOC);

  VTK_MESH_DRIVER meshDriver( _fileName, myMesh );
  meshDriver.write();

  END_OF_MED(LOC);
}

void VTK_MED_DRIVER::writeField(FIELD_ * myField,string name) const
{
  const char* LOC = "VTK_MED_DRIVER::writeField() : ";
  BEGIN_OF_MED(LOC);

  med_type_champ type = myField->getValueType() ;
  GENDRIVER* driver = 0;
  switch (type)
    {
    case MED_INT32 :

      if ( myField->getInterlacingType() == MED_FULL_INTERLACE )
        driver = new VTK_FIELD_DRIVER<int>(_fileName,
                                           static_cast< FIELD<int,FullInterlace>* >(myField));

      else if ( myField->getInterlacingType() == MED_NO_INTERLACE_BY_TYPE )
        driver = new VTK_FIELD_DRIVER<int>(_fileName,
                                           static_cast< FIELD<int,NoInterlaceByType>* >(myField));

      else
        driver = new VTK_FIELD_DRIVER<int>(_fileName,
                                           static_cast< FIELD<int,NoInterlace>* >(myField));
      break;

    case MED_REEL64 : 

      if ( myField->getInterlacingType() == MED_FULL_INTERLACE )
        driver = new VTK_FIELD_DRIVER<double>(_fileName,
                                              static_cast< FIELD<double,FullInterlace>* >(myField));

      else if ( myField->getInterlacingType() == MED_NO_INTERLACE_BY_TYPE )
        driver = new VTK_FIELD_DRIVER<double>(_fileName,
                                              static_cast< FIELD<double,NoInterlaceByType>*>(myField));

      else
        driver = new VTK_FIELD_DRIVER<double>(_fileName,
                                              static_cast< FIELD<double,NoInterlace>* >(myField));
      break;

    default :
      {
        MESSAGE_MED(PREFIX_MED << "Could not write field "<<name<<" the type is not int or double !");
      }
    }

  if ( driver )
    {
      driver->writeAppend();
      delete driver;
    }
  END_OF_MED(LOC);
}

// void VTK_MED_DRIVER::writeSupport(SUPPORT * mySupport) const {
//   const char* LOC = "VTK_MED_DRIVER::writeSupport(SUPPORT *) : ";
//   BEGIN_OF_MED(LOC);
//   MESSAGE_MED(PREFIX_MED << "Not yet implemented, acting on the object " << *mySupport);
//   END_OF_MED(LOC);
// }
