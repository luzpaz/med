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

#include "MEDMEM_MedMeshDriver21.hxx"

#include "MEDMEM_DriversDef.hxx"

#include "MEDMEM_Family.hxx"
#include "MEDMEM_Group.hxx"
#include "MEDMEM_Coordinate.hxx"
#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_CellModel.hxx"
#include "MEDMEM_Grid.hxx"
#include "MEDMEM_MedVersion.hxx"


using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

namespace med_2_1 {
  extern "C" {
    extern med_idt _MEDdatagroupOuvrir(med_idt pid, char *nom);
    extern med_err _MEDdatagroupFermer(med_idt id);
    extern med_err _MEDparametresGeometrie(med_entite_maillage type_ent, 
                                           med_geometrie_element type_geo,
                                           int *dim, int *nnoe,int *ndes);
  }
}

// Every memory allocation made in the MedDriver members function are desallocated in the Mesh destructor 

MED_MESH_DRIVER21::MED_MESH_DRIVER21():_medIdt(MED_INVALID)
{
}

MED_MESH_DRIVER21::MED_MESH_DRIVER21(const string & fileName,  
                                     MESH * ptrMesh, 
                                     MED_EN::med_mode_acces accessMode):
  _medIdt(MED_INVALID),MED_MESH_DRIVER(fileName,ptrMesh,accessMode)
{
}

MED_MESH_DRIVER21::MED_MESH_DRIVER21(const MED_MESH_DRIVER21 & driver):
  MED_MESH_DRIVER(driver),_medIdt(driver._medIdt)
{
}

MED_MESH_DRIVER21::~MED_MESH_DRIVER21()
{
}

void MED_MESH_DRIVER21::open()
{
  const char * LOC = "MED_MESH_DRIVER21::open()" ;
  BEGIN_OF_MED(LOC);
  int accessMode = getMedAccessMode( _accessMode, MED_EN::V21 );
  MESSAGE_MED(LOC<<" : _fileName.c_str : "<< _fileName.c_str()<<",mode : "<< accessMode);
  _medIdt = med_2_1::MEDouvrir( (const_cast <char *> (_fileName.c_str())),(med_2_1::med_mode_acces) accessMode);
  MESSAGE_MED(LOC<<" _medIdt : "<< _medIdt );
  if (_medIdt > 0) 
    _status = MED_OPENED; 
  else {
    _medIdt = MED_INVALID;
    _status = MED_CLOSED;
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Could not open file "<<_fileName<<" in mode "<<_accessMode));
  }
  
  END_OF_MED(LOC);
}
  
void MED_MESH_DRIVER21::close()
{
  const char * LOC = "MED_MESH_DRIVER21::close() " ;
  BEGIN_OF_MED(LOC);
  int err = 0;
  if ( _status == MED_OPENED) {
    err=med_2_1::MEDfermer(_medIdt);
    // san -- MED5873 : Calling H5close() here leads to failure of SALOMEDS::StudyManager_i::_SaveAs()
    // method during study saving process. MEDfermer() seems sufficient for closing a file.
    //H5close(); // If we call H5close() all the files are closed.
    if (err != 0)
      throw MEDEXCEPTION( LOCALIZED(STRING(LOC)
                                    <<" Error when closing file !"
                                    )
                          );
    MESSAGE_MED(LOC <<": _medIdt= " << _medIdt );
    MESSAGE_MED(LOC<<": MEDfermer : err    = " << err );
    _status = MED_CLOSED;
    _medIdt = MED_INVALID;
  }
  END_OF_MED(LOC);
}

//A FAIRE UTILISER LES MAPS...
const med_2_1::med_geometrie_element  MED_MESH_DRIVER21::all_cell_type[MED_NBR_GEOMETRIE_MAILLE]=
  { med_2_1::MED_POINT1,med_2_1::MED_SEG2,med_2_1::MED_SEG3,med_2_1::MED_TRIA3,med_2_1::MED_QUAD4,med_2_1::MED_TRIA6,med_2_1::MED_QUAD8,
    med_2_1::MED_TETRA4,med_2_1::MED_PYRA5,med_2_1::MED_PENTA6,med_2_1::MED_HEXA8,med_2_1::MED_TETRA10,med_2_1::MED_PYRA13,
    med_2_1::MED_PENTA15, med_2_1::MED_HEXA20};

const char * const MED_MESH_DRIVER21::all_cell_type_tab [MED_NBR_GEOMETRIE_MAILLE]=
  { "MED_POINT1","MED_SEG2","MED_SEG3","MED_TRIA3","MED_QUAD4","MED_TRIA6","MED_QUAD8",
    "MED_TETRA4","MED_PYRA5","MED_PENTA6","MED_HEXA8","MED_TETRA10","MED_PYRA13",
    "MED_PENTA15","MED_HEXA20"};

//---------------------------------- RDONLY PART -------------------------------------------------------------

MED_MESH_RDONLY_DRIVER21::MED_MESH_RDONLY_DRIVER21():_computeFaces(true)
{
  this->GENDRIVER::_accessMode = MED_EN::RDONLY;
}
  
MED_MESH_RDONLY_DRIVER21::MED_MESH_RDONLY_DRIVER21(const string & fileName,
                                                   MESH * ptrMesh):
  IMED_MESH_RDONLY_DRIVER(fileName,ptrMesh),
  MED_MESH_DRIVER21(fileName,ptrMesh,RDONLY),
  MED_MESH_DRIVER(fileName,ptrMesh,RDONLY),
  _computeFaces(true)
{ 
  MESSAGE_MED("MED_MESH_RDONLY_DRIVER21::MED_MESH_RDONLY_DRIVER21(const string & fileName, MESH * ptrMesh) has been created");
}
  
MED_MESH_RDONLY_DRIVER21::MED_MESH_RDONLY_DRIVER21(const MED_MESH_RDONLY_DRIVER21 & driver): 
  IMED_MESH_RDONLY_DRIVER(driver),
  MED_MESH_DRIVER21(driver),
  MED_MESH_DRIVER(driver),
  _computeFaces(driver._computeFaces)
{
}

MED_MESH_RDONLY_DRIVER21::~MED_MESH_RDONLY_DRIVER21()
{
  //MESSAGE_MED("MED_MESH_RDONLY_DRIVER21::~MED_MESH_RDONLY_DRIVER21() has been destroyed");
}
  
GENDRIVER * MED_MESH_RDONLY_DRIVER21::copy(void) const
{
  return new MED_MESH_RDONLY_DRIVER21(*this);
}

void MED_MESH_RDONLY_DRIVER21::read(void)
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER21::read() : " ;
  BEGIN_OF_MED(LOC);
  if (_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The _idt of file " << _fileName << " is : " << _medIdt <<  " (the file is not opened)." )) ;

  _ptrMesh->_name =  _meshName;

  SCRUTE_MED(_ptrMesh->getIsAGrid());

  if (_ptrMesh->getIsAGrid())
    {
      getGRID( );
        {
          if (getFAMILY()!=MED_VALID)
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERREUR in getFAMILY when the mesh is a grid")) ;

          buildAllGroups(_ptrMesh->_groupNode,_ptrMesh->_familyNode) ;
        }

  END_OF_MED(LOC);
      return;
    }
  // (EAP at EDF): Commented for MEDMEMTest_VtkMedDriver.cxx:126 -
  // aMedMeshRdDriver21->read();
//   else // check that the mesh is really unstructured (PAL14113)
//   {
//     char                  meshName[MED_TAILLE_NOM+1]="";
//     char                  meshDescription[MED_TAILLE_DESC+1]="";
//     med_2_3::med_int      meshDim;
//     med_2_3::med_maillage meshType;
//     int numberOfMeshes = med_2_3::MEDnMaa(_medIdt);
//     for (int i=1;i<=numberOfMeshes;i++)
//     {
//       MEDmaaInfo(_medIdt, i ,meshName, &meshDim, &meshType, meshDescription);
//       if (_meshName == string(meshName)) {
//         if ( meshType == med_2_3::MED_STRUCTURE ) {
//           throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<
//                                        "class GRID must be used for a structured mesh"));
//         }
//         else {
//           break;
//         }
//       }
//     }
//   }

  if (getCOORDINATE()!=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERREUR in getCOORDINATE"  )) ;
 
  if (getCONNECTIVITY()!=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERREUR in getCOONECTIVITY")) ;
  
  if (getFAMILY()!=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERREUR in getFAMILY"      )) ;

  if (_computeFaces)
    updateFamily();

  // we build all groups
  // on node
  buildAllGroups(_ptrMesh->_groupNode,_ptrMesh->_familyNode) ;
  // on cell
  buildAllGroups(_ptrMesh->_groupCell,_ptrMesh->_familyCell) ;

  if (_ptrMesh->_meshDimension == 3)
    // on face
    buildAllGroups(_ptrMesh->_groupFace,_ptrMesh->_familyFace) ;
//   else if (_ptrMesh->_meshDimension == 2) -- PAL13414
  if (_ptrMesh->_meshDimension > 1)
    // on edge
    buildAllGroups(_ptrMesh->_groupEdge,_ptrMesh->_familyEdge) ;

//   MESSAGE_MED(LOC<<"Checking of CellModel !!!!!!!");

//   int nbOfTypes =  _ptrMesh->_connectivity->_numberOfTypes;
//    for(int i=0;i<nbOfTypes;i++)
//      {
//        MESSAGE_MED(LOC << _ptrMesh->_connectivity->_type[i]) ;
//      }

  END_OF_MED(LOC);
}

//=======================================================================
//function : getGRID
//purpose  : 
//=======================================================================

void MED_MESH_RDONLY_DRIVER21::getGRID()
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER21::getGRID() : " ;
  BEGIN_OF_MED(LOC);
  
  if (_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "med file is not opened"));
  
  GRID * ptrGrid = (GRID *) _ptrMesh;
    
  int err, i;

  // Read the dimension of the space for the mesh <_meshName>
  int SpaceDimension = med_2_1::MEDdimLire(_medIdt,const_cast <char *> (_meshName.c_str())) ;
  if ( SpaceDimension  <= MED_VALID ) 
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"The space dimension |" << SpaceDimension <<
                                 "| seems to be incorrect " << "for the mesh : |" <<
                                 _meshName << "|")) ;
  _ptrMesh->_spaceDimension = SpaceDimension;

  _ptrMesh->_meshDimension = SpaceDimension;

  // Read Array length
  int * ArrayLen[] = { & ptrGrid->_iArrayLength,
                       & ptrGrid->_jArrayLength,
                       & ptrGrid->_kArrayLength  };
  int idim;
  for (idim = 0; idim < _ptrMesh->_spaceDimension; ++idim)
    {
      int lenght = med_2_1::MEDnGrid(_medIdt,
                                    const_cast <char *> (_ptrMesh->_name.c_str()),
                                    (med_2_1::med_grid)
                                    idim
                                    );
      if ( lenght <= MED_VALID )
        throw MEDEXCEPTION(STRING(LOC) <<"The number of nodes |" << lenght <<
                           "| seems to be incorrect "
                           << "for the mesh : |" << _meshName << "|" ) ;
    
      ArrayLen [idim][0] = lenght;
    }

  med_2_1::med_repere rep ;
  string tmp_nom_coord (MED_TAILLE_PNOM21*(_ptrMesh->_spaceDimension)+1,' ');
  string tmp_unit_coord(MED_TAILLE_PNOM21*(_ptrMesh->_spaceDimension)+1,' ');
  char * tmp_nom = (const_cast <char *> ( tmp_nom_coord.c_str())  ) ;
  char * tmp_unit= (const_cast <char *> ( tmp_unit_coord.c_str()) ) ;
  
  // Read node coordinates for MED_BODY_FITTED grid

  SCRUTE_MED(ptrGrid->getGridType());

  if (ptrGrid->getGridType() == MED_EN::MED_BODY_FITTED)
    {
      // Read nb of nodes
      int NumberOfNodes = med_2_1::MEDnGrid(_medIdt,
                                           const_cast <char *> (_meshName.c_str()),
                                           med_2_1::MED_GRID_NOEUD);
      if ( NumberOfNodes <= MED_VALID )
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"The number of nodes |" << NumberOfNodes <<
                                     "| seems to be incorrect "
                                     << "for the mesh : |" << _meshName << "|" )) ;
      _ptrMesh->_numberOfNodes = NumberOfNodes ;

      // this array is useless because families numbers are read in getFAMILY
      //CCRT  as it is useless replace int by med_int
      med_2_1::med_int * MEDArrayNodeFamily = new med_2_1::med_int[ NumberOfNodes ];
      // create coordinates
      _ptrMesh->_coordinate = new COORDINATE(SpaceDimension,NumberOfNodes,
                                             MED_EN::MED_FULL_INTERLACE);

      // Read coordinates and families
//       double * coo = const_cast <double *>
//      (_ptrMesh->_coordinate->getCoordinates(MED_EN::MED_FULL_INTERLACE));
    
//       err = med_2_1::MEDbodyFittedLire (_medIdt,
//                                     const_cast <char *> (_ptrMesh->_name.c_str()),
//                                     _ptrMesh->_spaceDimension,
//                                     coo,
//                                     med_2_1::MED_FULL_INTERLACE,
//                                     & rep,
//                                     tmp_nom,
//                                     tmp_unit,
//                                     MEDArrayNodeFamily,
//                                     NumberOfNodes);

      err = med_2_1::MEDbodyFittedLire (_medIdt,
                                       const_cast <char *> (_ptrMesh->_name.c_str()),
                                       _ptrMesh->_spaceDimension,
                                       const_cast <double *> ( _ptrMesh->_coordinate->_coordinate.get(MED_EN::MED_FULL_INTERLACE) ),
                                       med_2_1::MED_FULL_INTERLACE,
                                       & rep,
                                       tmp_nom,
                                       tmp_unit,
                                       MEDArrayNodeFamily,
                                       NumberOfNodes);

      MESSAGE_MED(LOC << " NumberOfNodes = " << NumberOfNodes << " SpaceDimension = " << SpaceDimension);

      ptrGrid->_is_coordinates_filled = true;

//       for (int icoor = 0 ; icoor<NumberOfNodes ; icoor++)
//      for(int jcoor = 0 ; jcoor<SpaceDimension ; jcoor++)
//        MESSAGE_MED(LOC << " icoor = " << icoor << " jcoor = " << jcoor << " COOR = " << _ptrMesh->getCoordinates(MED_FULL_INTERLACE)[icoor*SpaceDimension+jcoor]);

      delete[] MEDArrayNodeFamily;
      if (err != MED_VALID)
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"error in MEDbodyFittedLire()"));

      //      _ptrMesh->_MEDArrayNodeFamily = fam ;

    }
  else
    {
      // Read Arrays and Node families in Cartesian or Polar Grid

      int nbNodes = 1;
      double * Array[] = { (double*) 0, (double*) 0, (double*) 0 };
      for (idim = 0; idim < _ptrMesh->_spaceDimension; ++idim)
        {
          int nbNodesDim = * ArrayLen [idim];
          nbNodes *= nbNodesDim;
          Array [idim] = new double [ nbNodesDim ];
          err = med_2_1::MEDgridLire (_medIdt,
                                     const_cast <char *> (_ptrMesh->_name.c_str()),
                                     _ptrMesh->_spaceDimension,
                                     Array [idim],
                                     idim,
                                     med_2_1::MED_FULL_INTERLACE,
                                     & rep,
                                     tmp_nom,
                                     tmp_unit);
          if (err != MED_VALID)
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Error in MEDgridLire for dimention" << idim ));

        }
      ptrGrid->_iArray = Array[0];
      ptrGrid->_jArray = Array[1];
      ptrGrid->_kArray = Array[2];
    
      _ptrMesh->_numberOfNodes = nbNodes ;
    
      // create coordinates
      _ptrMesh->_coordinate = new COORDINATE(SpaceDimension,nbNodes,
                                             MED_EN::MED_FULL_INTERLACE);
    } // end read  Cartesian or Polar Grid

  // set coordinate names

  for (i=0; i<_ptrMesh->_spaceDimension; ++i ) {
    string myStringName(tmp_nom_coord,i*MED_TAILLE_PNOM21,MED_TAILLE_PNOM21) ;
    string myStringUnit(tmp_unit_coord,i*MED_TAILLE_PNOM21,MED_TAILLE_PNOM21) ;
    // suppress space at the end
    int j ;
    for(j=MED_TAILLE_PNOM21-1;j>=0;j--)
      if (myStringName[j] != ' ') break ;
    _ptrMesh->_coordinate->_coordinateName[i]=string(myStringName,0,j+1);
    for(j=MED_TAILLE_PNOM21-1;j>=0;j--)
      if (myStringUnit[j] != ' ') break ;
    _ptrMesh->_coordinate->_coordinateUnit[i]=string(myStringUnit,0,j+1);
  }

  string coordinateSystem = "UNDEFINED";

  if( rep == med_2_1::MED_CART) coordinateSystem = "CARTESIAN";
  else if ( rep == med_2_1::MED_CYL) coordinateSystem = "CYLINDRICAL";
  else if ( rep == med_2_1::MED_SPHER) coordinateSystem = "SPHERICAL";

  _ptrMesh->_coordinate->setCoordinatesSystem(coordinateSystem);


  END_OF_MED(LOC);
}

//=======================================================================
//function : getCOORDINATE
// A FAIRE : RENVOYER DU VOID
//=======================================================================
int  MED_MESH_RDONLY_DRIVER21::getCOORDINATE()
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER21::getCOORDINATE() : " ;
  BEGIN_OF_MED(LOC);

  if (_status==MED_OPENED)
    {
      int err ;
      
      // Read the dimension of the space for the mesh <_meshName>
      // to be able to create a COORDINATE object
      int SpaceDimension = med_2_1::MEDdimLire(_medIdt,const_cast <char *> (_meshName.c_str())) ;
      if ( SpaceDimension  <= MED_VALID ) 
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"The space dimension |" << SpaceDimension << "| seems to be incorrect "
                                     << "for the mesh : |" << _meshName << "|")) ;
      _ptrMesh->_spaceDimension = SpaceDimension ;

      

      // Read the number of nodes used in the mesh <_meshName>
      // to be able to create a COORDINATE object
      int NumberOfNodes=MEDnEntMaa(_medIdt,
                                   const_cast <char *> (_meshName.c_str()),
                                   med_2_1::MED_COOR,
                                   med_2_1::MED_NOEUD,
                                   (med_2_1::med_geometrie_element) MED_NONE,
                                   (med_2_1::med_connectivite)      MED_NONE);
      if ( NumberOfNodes <= MED_VALID )
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"The number of nodes |" << NumberOfNodes << "| seems to be incorrect "
                                     << "for the mesh : |" << _meshName << "|" )) ;
      _ptrMesh->_numberOfNodes = NumberOfNodes ;



      // create a COORDINATE object
      _ptrMesh->_coordinate = new COORDINATE(SpaceDimension, NumberOfNodes, MED_EN::MED_FULL_INTERLACE);
      
      med_2_1::med_repere rep ; // ATTENTION ---> DOIT ETRE INTEGRE DS MESH EF: FAIT NON?
      string tmp_nom_coord (MED_TAILLE_PNOM21*(_ptrMesh->_spaceDimension)+1,'\0');
      string tmp_unit_coord(MED_TAILLE_PNOM21*(_ptrMesh->_spaceDimension)+1,'\0');
      char * tmp_nom = (const_cast <char *> ( tmp_nom_coord.c_str())  ) ;
      char * tmp_unit= (const_cast <char *> ( tmp_unit_coord.c_str()) ) ;

      err=MEDcoordLire(_medIdt,
                       const_cast <char *> (_ptrMesh->_name.c_str()),
                       _ptrMesh->_spaceDimension,
                       //const_cast <double *> ( _ptrMesh->_coordinate->_coordinate->get(MED_EN::MED_FULL_INTERLACE) ),
                       const_cast <double *> ( _ptrMesh->_coordinate->_coordinate.get(MED_EN::MED_FULL_INTERLACE) ),
                       med_2_1::MED_FULL_INTERLACE,
                       MED_ALL,                      // we read all the coordinates
                       NULL,                         // we don't use a profile
                       0,                            // so the profile's size is 0
                       &rep,tmp_nom,tmp_unit);
      if (err != MED_VALID)
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't read coordinates of the |" << NumberOfNodes << "| nodes "
                                     << "for the mesh : |" << _meshName 
                                     << "| of space dimension |" << SpaceDimension 
                                     << "| with units names |"   << tmp_nom
                                     << "| and units |"          << tmp_unit
                                     << " |")) ;
      

      for (int i=0;i<_ptrMesh->_spaceDimension;i++) {
        string myStringName(tmp_nom_coord,i*MED_TAILLE_PNOM21,MED_TAILLE_PNOM21) ;
        string myStringUnit(tmp_unit_coord,i*MED_TAILLE_PNOM21,MED_TAILLE_PNOM21) ;
        // suppress space at the end
        int j ;
        for(j=MED_TAILLE_PNOM21-1;j>=0;j--)
          if (myStringName[j] != ' ') break ;
        _ptrMesh->_coordinate->_coordinateName[i]=string(myStringName,0,j+1);
        for(j=MED_TAILLE_PNOM21-1;j>=0;j--)
          if (myStringUnit[j] != ' ') break ;
        _ptrMesh->_coordinate->_coordinateUnit[i]=string(myStringUnit,0,j+1);
      }

      // Pourquoi le stocker sous forme de chaÃ®ne ?
      switch (rep)
        {
        case med_2_1::MED_CART : 
          {
            _ptrMesh->_coordinate->_coordinateSystem = "CARTESIAN";
            break ;
          }
        case med_2_1::MED_CYL :
          {
            _ptrMesh->_coordinate->_coordinateSystem = "CYLINDRICAL";
            break ;
          }
        case med_2_1::MED_SPHER :
          {
            _ptrMesh->_coordinate->_coordinateSystem = "SPHERICAL";
            break ;
          }
        default :
          {
            _ptrMesh->_coordinate->_coordinateSystem = "UNDEFINED"; // ?Erreur ?
            break ;
          }
        }

      // Read the unused optional node Names
      char * tmp_node_name = new char[NumberOfNodes*MED_TAILLE_PNOM21+1];
      tmp_node_name[NumberOfNodes]='\0' ;
      err=MEDnomLire(_medIdt,const_cast <char*> (_ptrMesh->_name.c_str()),
                     tmp_node_name,NumberOfNodes*MED_TAILLE_PNOM21,med_2_1::MED_NOEUD,
                     (med_2_1::med_geometrie_element) MED_NONE);
      if (err == MED_VALID) 
        MESSAGE_MED(LOC<<"MED_MESH_RDONLY_DRIVER::getNoeuds() : WARNING : Nodes have names but we do not read them !");
      delete[] tmp_node_name ;


      // ??? Read the unused optional node Numbers ???
      med_2_1::med_int * tmp_node_number = new med_2_1::med_int[NumberOfNodes] ;
      err=MEDnumLire(_medIdt,const_cast <char*> (_ptrMesh->_name.c_str()),
                     tmp_node_number,NumberOfNodes,med_2_1::MED_NOEUD,(med_2_1::med_geometrie_element)0);
      if (err == MED_VALID) {
        // INFOS_MED(LOC<<"WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING");
        // INFOS_MED(LOC<<"MED_MESH_RDONLY_DRIVER::getNoeuds() : WARNING : Nodes have numbers but we do not take care of them !");
        // INFOS_MED(LOC<<"WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING - WARNING");
        MESSAGE_MED(LOC<<"MED_MESH_RDONLY_DRIVER::getNoeuds() : Nodes have numbers, we DO TAKE care of them !");
        _ptrMesh->_coordinate->_nodeNumber.set(NumberOfNodes) ; 
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        for(med_2_1::med_int i2=0;i2<NumberOfNodes;i2++)
          _ptrMesh->_coordinate->_nodeNumber[i2]=(int)(tmp_node_number[i2]);
#else
        memcpy((int*)_ptrMesh->_coordinate->_nodeNumber,tmp_node_number,sizeof(int)*NumberOfNodes) ;
#endif
        
        //////////////////////////////////////////////////////////////////////////////////////
        ///  Modification pour prise en compte de la numérotation optionnelle des noeuds   ///
        //////////////////////////////////////////////////////////////////////////////////////
        ///
        /// Calcule _optionnalToCanonicNodesNumbers de telle sorte que _optionnalToCanonicNodesNumbers[OptionnalNumber]==CanonicNumber
        
//      _ptrMesh->_arePresentOptionnalNodesNumbers=1;
//      for (int canonicNumber=1;canonicNumber<=NumberOfNodes;canonicNumber++) _ptrMesh->_optionnalToCanonicNodesNumbers[tmp_node_number[canonicNumber-1]]=canonicNumber;
// ICI RETOUR A LA NORMALE::: AUCUNE PRISE EN COMPTE D'UN NUMEROTATION OPTIONNEL
        _ptrMesh->_arePresentOptionnalNodesNumbers=0;
      } 
      else _ptrMesh->_arePresentOptionnalNodesNumbers=0;

        //////////////////////////////////////////////////////////////////////////////////////

      delete[] tmp_node_number ;
      
  END_OF_MED(LOC);
      return MED_VALID;
    }
  return MED_ERROR;
}


int MED_MESH_RDONLY_DRIVER21::getCONNECTIVITY() 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER21::getCONNECTIVITY : " ;
  BEGIN_OF_MED(LOC);

  if (_status==MED_OPENED)
    {

      int err = 0 ;
      // read MED_CELL connectivity
      CONNECTIVITY * Connectivity     = new CONNECTIVITY(MED_CELL) ;
      Connectivity->_numberOfNodes    = _ptrMesh->_numberOfNodes ;   // EF : Pourquoi cet attribut est-il dans MESH et non dans COORDINATE ?

      // Try to read nodal connectivity of the cells <Connectivity->_nodal>
      // then try to read descending connectivity    <Connectivity->_descending>
      // if neither nodal nor descending connectivity exists
      // throw an exception.
      err = getNodalConnectivity(Connectivity) ;
      if (err!=MED_VALID)
        {
          Connectivity->_typeConnectivity = MED_DESCENDING ;
          err = getDescendingConnectivity(Connectivity) ;
        }
      else 
        getDescendingConnectivity(Connectivity) ; // we read it if there is one
      
      if (err!=MED_VALID)
        {
          delete Connectivity ;
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "We could not read " <<
                                       "any Connectivity")) ;
        }

      _ptrMesh->_meshDimension = Connectivity->_entityDimension ; 

      // At this point Connectivity->_typeConnectivity is either NODAL or DESCENDING
      // If both connectivities are found Connectivity->_typeConnectivity is NODAL
      // If space dimension is 3 
      // try to read the nodal connectivity of the faces <ConnectivityFace->_nodal> then
      // try to read the descending connectivity <ConnectivityFace->_descending>
      // if there is no descending connectivity and the CELLS are
      // defined in descending mode then throw an exception

      // PROVISOIRE : if we have some face or edge in MED_MAILLE, we don't read more. There could not be have face or edge !!!!

      if(Connectivity->_constituent==NULL)
        {
          SCRUTE_MED(_ptrMesh->_meshDimension);
          if (_ptrMesh->_meshDimension == 3)
            {
              MESSAGE_MED(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE DES FACES..." );
              CONNECTIVITY * ConnectivityFace = new CONNECTIVITY(MED_EN::MED_FACE) ;
              ConnectivityFace->_typeConnectivity = Connectivity->_typeConnectivity ;
              // NODAL or DESCENDING
              SCRUTE_MED(ConnectivityFace->_typeConnectivity);
              if (Connectivity->_typeConnectivity == MED_DESCENDING)
                {
                  MESSAGE_MED(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE DESCENDANTE DES FACES" );
                  err = getDescendingConnectivity(ConnectivityFace) ;
                  if (err!=MED_VALID)
                    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<
                                                 "No FACE in descending connectivity")) ;
                  getNodalConnectivity(ConnectivityFace) ; // if any !
                }
              else
                {
                  MESSAGE_MED(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE NODALE DES FACES" );
                  err = getNodalConnectivity(ConnectivityFace) ;
                  if (err!=MED_VALID)
                    { // or error ????? we are in NODAL mode.
                      err = getDescendingConnectivity(ConnectivityFace) ;
                    }
                  else
                    getDescendingConnectivity(ConnectivityFace); // if any !
                }

              if (err!=MED_VALID)
                {
                  delete ConnectivityFace ;
                  MESSAGE_MED(LOC<<"No FACE defined.") ;
                }
              else
                {
                  MESSAGE_MED(LOC<<" SAUVEGARDE DE LA CONNECTIVITE DES " <<
                          "FACES DANS L'OBJET CONNECTIVITY" );
                  Connectivity->_constituent=ConnectivityFace ; 
                }
            }
      
          // read MED_EDGE connectivity
          if (_ptrMesh->_meshDimension > 1)
            { // we are in 3 or 2D 
              MESSAGE_MED(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE DES ARRETES...." );
              CONNECTIVITY * ConnectivityEdge = new CONNECTIVITY(MED_EDGE) ;
              ConnectivityEdge->_typeConnectivity = Connectivity->_typeConnectivity ;
              if (Connectivity->_typeConnectivity == MED_DESCENDING)
                {
                  MESSAGE_MED(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE DESCENDANTE " <<
                          "DES ARRETES" );
                  err = getDescendingConnectivity(ConnectivityEdge) ;
                  if (err!=MED_VALID)
                    throw MEDEXCEPTION ( LOCALIZED(STRING(LOC) <<
                                                   "No EDGE in descending connectivity")) ;
                  getNodalConnectivity(ConnectivityEdge) ; // if any !
                }
              else
                {
                  MESSAGE_MED(LOC<<" ESSAI DE LECTURE DE LA CONNECTIVITE NODALE DES ARRETES" );
                  err = getNodalConnectivity(ConnectivityEdge) ;
                  if (err!=MED_VALID)
                    { // or error ????? we are in NODAL mode.
                      err = getDescendingConnectivity(ConnectivityEdge) ;
                    }
                  else
                    getDescendingConnectivity(ConnectivityEdge) ; // if any !
                }

              if (err!=MED_VALID)
                {
                  delete ConnectivityEdge ;
                  MESSAGE_MED(LOC<<"No EDGE defined.") ;
                }
              else
                {
                  if (_ptrMesh->_meshDimension == 3)
                    if (Connectivity->_constituent != NULL)
                      Connectivity->_constituent->_constituent=ConnectivityEdge ;
                    else
                      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<< "EDGE defined but there are no FACE !")) ;
                  else
                    { // IN 2D
                      MESSAGE_MED(LOC<<" SAUVEGARDE DE LA CONNECTIVITE DES " <<
                              "ARETES DANS L'OBJET CONNECTIVITY" );
                      Connectivity->_constituent=ConnectivityEdge ;
                    }
                }
            }
        }
      _ptrMesh->_connectivity  = Connectivity ;                                      

      // all right !

      // we have read all connectivity in file, now we must build descending connectivity if necessary !

      // If connectivity descending is defined, we have nothing to do, all constituent are defined !
      // If connectivity is only nodal, we must rebuild descending if we have some contituent !

      //A FAIRE !!!!
//        if (Connectivity->_descending == NULL)
//      if (Connectivity->_constituent != NULL){
//        // update Connectivity->_constituent
//        CONNECTIVITY * myConstituentOld = Connectivity->_constituent ;
//        Connectivity->_constituent = (CONNECTIVITY *)NULL ;
//        Connectivity->calculateDescendingConnectivity() ;
          
//      }
      
  END_OF_MED(LOC);
      return MED_VALID;
    }
  return MED_ERROR;
}

int MED_MESH_RDONLY_DRIVER21::getNodalConnectivity(CONNECTIVITY * Connectivity) 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER21::getNodalConnectivity : " ;
  BEGIN_OF_MED(LOC);

  if (_status==MED_OPENED)
    {
      int spaceDimension = _ptrMesh->_spaceDimension;

      // Get the type of entity to work on (previously set in the Connectivity Object)
      med_2_1::med_entite_maillage Entity = (med_2_1::med_entite_maillage) Connectivity->getEntity();

      // Get the number of cells of each type & store it in <tmp_cells_count>.
      int * tmp_cells_count = new int[MED_NBR_GEOMETRIE_MAILLE] ;
      int i;
      for (i=1;i<MED_NBR_GEOMETRIE_MAILLE;i++)
        { // EF :ON SCANNE DES GEOMETRIES INUTILES, UTILISER LES MAPS

          tmp_cells_count[i]=MEDnEntMaa(_medIdt,(const_cast <char *> (_ptrMesh->_name.c_str())),
                                        med_2_1::MED_CONN,(med_2_1::med_entite_maillage) Entity,
                                        all_cell_type[i],med_2_1::MED_NOD); 

          // Get the greatest dimension of the cells : Connectivity->_entityDimension
          // We suppose there is no cells used as faces in MED 2.2.x , this is forbidden !!!
          // In version prior to 2.2.x, it is possible
          if (tmp_cells_count[i]>0)
            { 
              Connectivity->_entityDimension=all_cell_type[i]/100;  
              Connectivity->_numberOfTypes++;
            }
        }
      

      // If there is no nodal connectivity, we quit !
      if ( Connectivity->_numberOfTypes == 0 )
        {
          delete[] tmp_cells_count ;
          return MED_ERROR ;
        }

      // if MED version < 2.2.x, we read only entity with dimention = Connectivity->_entityDimension. Lesser dimension are face or edge !

      char version_med[10] ;
      if ( MEDfichEntete(_medIdt,med_2_1::MED_VERSION,version_med) != 0 )
        {
          // error : we suppose we have not a good med file !
          delete[] tmp_cells_count ;
          return MED_ERROR ;
        }

      // we get MED version number
      // If MED version is < 2.2 then the cells which dimension
      // is lesser than the main dimension ( Connectivity->_entityDimension )
      // are either faces or edges

      //        string medVersion(version_med);
      //        int firstNumber = 
      int * tmpEdgeCount = new int[MED_NBR_GEOMETRIE_MAILLE] ;
      tmpEdgeCount[0]     = 0 ;
      int numberOfEdgesTypes = 0;
      int * tmpFaceCount = new int[MED_NBR_GEOMETRIE_MAILLE] ;
      tmpFaceCount[0]     = 0 ;
      int numberOfFacesTypes = 0;
  
//       if ((version_med != "2.2")&(Entity==med_2_1::MED_MAILLE))
//      {
//        Connectivity->_numberOfTypes=0;
        
//        for ( i=1;i<MED_NBR_GEOMETRIE_MAILLE;i++)
//          {
//            tmpFaceCount[i]=0;
//            tmpEdgeCount[i]=0;
//            if (tmp_cells_count[i]!=0)
//              {
//                int dimension = all_cell_type[i]/100 ;
//                if (Connectivity->_entityDimension==dimension) 
//                  Connectivity->_numberOfTypes++ ;
            
//                if (dimension == 2)
//                  if (Connectivity->_entityDimension==3)
//                    {
//                      tmpFaceCount[i]=tmp_cells_count[i] ;
//                      tmp_cells_count[i]=0 ;
//                      numberOfFacesTypes++;
//                    }
//                if (dimension == 1)
//                  if (Connectivity->_entityDimension>dimension)
//                    {
//                      tmpEdgeCount[i]=tmp_cells_count[i] ;
//                      tmp_cells_count[i]=0;
//                      numberOfEdgesTypes++ ;
//                    }
//              }
//          }
//      }

      if (Entity==med_2_1::MED_MAILLE)
        {
          Connectivity->_numberOfTypes=0;
        
          for ( i=1;i<MED_NBR_GEOMETRIE_MAILLE;i++)
            {
              tmpFaceCount[i]=0;
              tmpEdgeCount[i]=0;
              if (tmp_cells_count[i]!=0)
                {
                  int dimension = all_cell_type[i]/100 ;
                  if (Connectivity->_entityDimension==dimension) 
                    Connectivity->_numberOfTypes++ ;
            
                  if (dimension == 2)
                    if (Connectivity->_entityDimension==3)
                      {
                        tmpFaceCount[i]=tmp_cells_count[i] ;
                        //tmp_cells_count[i]=0 ;
                        //Connectivity->_numberOfTypes++ ;
                        numberOfFacesTypes++;
                      }
                  if (dimension == 1)
                    if (Connectivity->_entityDimension>dimension)
                      {
                        tmpEdgeCount[i]=tmp_cells_count[i] ;
                        //tmp_cells_count[i]=0;
                        //Connectivity->_numberOfTypes++ ;
                        numberOfEdgesTypes++ ;
                      }
                }
            }
        }

      // bloc to read CELL :
      {
        // Prepare an array of indexes on the different cell types to create a MEDSKYLINEARRAY
        // We use <tmp_cells_count> to calculate <Connectivity->_count> then we release it
        Connectivity->_geometricTypes = new MED_EN::medGeometryElement [Connectivity->_numberOfTypes]   ;  // Double emploi pour des raisons pratiques 
        Connectivity->_type           = new CELLMODEL                  [Connectivity->_numberOfTypes]   ;  //
        Connectivity->_count          = new int                        [Connectivity->_numberOfTypes+1] ;
        Connectivity->_count[0]       = 1;
        
        int size = 0 ; 
        int typeNumber=1 ;
        int i;
        for ( i=1;i<MED_NBR_GEOMETRIE_MAILLE;i++)
          { // no point1 cell type (?)
            int dimension = all_cell_type[i]/100 ;
            if ((tmp_cells_count[i]>0) && (Connectivity->_entityDimension == dimension))
              {
                Connectivity->_count[typeNumber]=Connectivity->_count[typeNumber-1]+tmp_cells_count[i];

                CELLMODEL t( (MED_EN::medGeometryElement) MED_MESH_DRIVER21::all_cell_type[i]) ;

                Connectivity->_type[typeNumber-1] = t ;
            
                Connectivity->_geometricTypes[typeNumber-1]=( MED_EN::medGeometryElement) MED_MESH_DRIVER21::all_cell_type[i] ;
            
                // probleme avec les mailles de dimension < a dimension du maillage :
                // Il faut oter le zero a la lecture est le remettre a l'ecriture : ce n'est pas fait !!!!! On interdit ce cas pour l'instant !!!

              
                size+=tmp_cells_count[i]*((MED_MESH_DRIVER21::all_cell_type[i])%100) ;
            
                MESSAGE_MED(LOC
                        << Connectivity->_count[typeNumber]-1 << " cells of type " 
                        << all_cell_type_tab[i] ); 

                typeNumber++;
              }
          }
        
        // Creation of the MEDSKYLINEARRAY
        //Connectivity->_nodal = new MEDSKYLINEARRAY(Connectivity->_count[Connectivity->_numberOfTypes]-1,size) ; 
        //int * NodalIndex = Connectivity->_nodal->getIndex() ;
        int * NodalValue = new int[size] ;
        int * NodalIndex = new int[Connectivity->_count[Connectivity->_numberOfTypes]] ;
        NodalIndex[0]=1 ;
        
        // Fill the MEDSKYLINEARRAY by reading the MED file.
        int j=0;
        for ( i=0;i<Connectivity->_numberOfTypes;i++)
          {
            int multi = 0 ;
            med_2_1::med_geometrie_element med_type = (med_2_1::med_geometrie_element) Connectivity->_type[i].getType() ;
            //if ( Connectivity->_type[i].getDimension() < Connectivity->_entityDimension) 
            if (Connectivity->_entity == MED_CELL)
              if ( Connectivity->_type[i].getDimension() < spaceDimension) 
                multi=1;
          
            //    int NumberOfCell = Connectivity->_count[i+1]-Connectivity->_count[i] ;
            int NumberOfNodeByCell = Connectivity->_type[i].getNumberOfNodes() ;
          
            // initialise index
            for ( j=Connectivity->_count[i]; j<Connectivity->_count[i+1];j++)
              NodalIndex[j]=NodalIndex[j-1]+NumberOfNodeByCell ; 

            int tmp_numberOfCells = Connectivity->_count[i+1]-Connectivity->_count[i] ;
            med_2_1::med_int * tmp_ConnectivityArray = new med_2_1::med_int[(NumberOfNodeByCell+multi)*tmp_numberOfCells];
          
            //int err=MEDconnLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
            //                    Connectivity->_entityDimension,tmp_ConnectivityArray,
            //med_2_1::MED_FULL_INTERLACE,NULL,0,Entity,med_type,med_2_1::MED_NOD);

            int err=MEDconnLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
                                spaceDimension,tmp_ConnectivityArray,
                                med_2_1::MED_FULL_INTERLACE,NULL,0,Entity,med_type,
                                med_2_1::MED_NOD);

            if ( err != MED_VALID)
              {
                delete[] tmp_ConnectivityArray;
                delete[] tmp_cells_count;
                delete[] tmpFaceCount;
                delete[] tmpEdgeCount;
                MESSAGE_MED(LOC<<": MEDconnLire returns "<<err) ;
                return MED_ERROR ;
              }

            int * ConnectivityArray = NodalValue + NodalIndex[Connectivity->_count[i]-1]-1 ;

            // version originale sans prise en compte des numéros optionnels
            //
            for ( j=0; j<tmp_numberOfCells; j++) for (int k=0; k<NumberOfNodeByCell; k++) 
              ConnectivityArray[j*NumberOfNodeByCell+k]=(int)(tmp_ConnectivityArray[j*(NumberOfNodeByCell+multi)+k]) ;

            //////////////////////////////////////////////////////////////////////////////
            // Modification pour prise en compte de la numérotation optionnelle des noeuds ///
            //////////////////////////////////////////////////////////////////////////////
            //
            // Rénumérote le tableau temporaire tmp_ConnectivityArray en utilisant _optionnalToCanonicNodesNumbers
            // Le traitement est identique à la version originelle s'il n'y a pas de numérotation optionnelle
            
            //  if (_ptrMesh->_arePresentOptionnalNodesNumbers==1) 
            //          {
            //          for ( j=0; j<tmp_numberOfCells; j++) for (int k=0; k<NumberOfNodeByCell; k++) 
            //                  ConnectivityArray[j*NumberOfNodeByCell+k]=_ptrMesh->_optionnalToCanonicNodesNumbers[tmp_ConnectivityArray[j*(NumberOfNodeByCell+multi)+k]] ;
            //          }
            //  else
            //          {
            //          for ( j=0; j<tmp_numberOfCells; j++) for (int k=0; k<NumberOfNodeByCell; k++) 
            //                  ConnectivityArray[j*NumberOfNodeByCell+k]=tmp_ConnectivityArray[j*(NumberOfNodeByCell+multi)+k] ;
            //          }
            ////////////////////////////////////////////////////////////////////////////
        
            delete[] tmp_ConnectivityArray;  
          }

        Connectivity->_nodal = new MEDSKYLINEARRAY(Connectivity->_count[Connectivity->_numberOfTypes]-1,
                                                   size,
                                                   NodalIndex,
                                                   NodalValue) ; 

        delete[] NodalIndex;
        delete[] NodalValue;
      } // end of bloc to read CELL

      delete[] tmp_cells_count; 



      // Get Face if any
      // ===============
     
      if (numberOfFacesTypes!=0) {

        // Create a CONNECTIVITY constituent to put in the top level CONNECTIVITY recursive class
        CONNECTIVITY * constituent = new CONNECTIVITY(numberOfFacesTypes,MED_EN::MED_FACE) ;
        constituent->_entityDimension = 2 ;
        constituent->_count[0]=1 ;

        // In order to create the MEDSKYLINEARRAY of the constituent object we need :
        // 1:
        // To initialize the _count array of the constituent object (containning cumulated face count by geometric type)
        // _count[0]=1 and _count[_numberOfTypes] give the size of NodalIndex
        // 2:
        // To calculate the total number of face nodes whatever the geometric type is.
        // The result is the size of the array containning all the nodes : NodalValue
        // 3 :
        // To calculate the starting indexes of the different face types in NodalValue, 
        // this is the NodalIndex array.
        
        int size       = 0 ; 
        int typeNumber = 1 ;
        int i;
        for ( i=1; i < MED_NBR_GEOMETRIE_MAILLE; i++)  { // no point1 cell type (?)
          if (tmpFaceCount[i]>0) {
            
            constituent->_count[typeNumber] = constituent->_count[typeNumber-1] + tmpFaceCount[i];
            CELLMODEL t( (MED_EN::medGeometryElement) MED_MESH_DRIVER21::all_cell_type[i]) ;
            constituent->_type[typeNumber-1]=t ;
            
            constituent->_geometricTypes[typeNumber-1]=( MED_EN::medGeometryElement) MED_MESH_DRIVER21::all_cell_type[i] ;
            
            size+=tmpFaceCount[i]*((MED_MESH_DRIVER21::all_cell_type[i])%100) ;
            typeNumber++;
          }
        }
        
        // Creation of the MEDSKYLINEARRAY
        //constituent->_nodal = new MEDSKYLINEARRAY(constituent->_count[constituent->_numberOfTypes]-1,size) ; 
        //int * NodalIndex = constituent->_nodal->getIndex() ;
        int * NodalValue = new int[size] ;
        int * NodalIndex = new int[constituent->_count[constituent->_numberOfTypes]] ;
        NodalIndex[0]=1 ;
        
        // Fill the MEDSKYLINEARRAY by reading the MED file.
        for ( i=0; i<constituent->_numberOfTypes; i++) {
          med_2_1::med_geometrie_element med_type = (med_2_1::med_geometrie_element) constituent->_type[i].getType() ;

          int NumberOfNodeByFace = constituent->_type[i].getNumberOfNodes() ;
          
          // initialise NodalIndex
          for (int j=constituent->_count[i]; j<constituent->_count[i+1];j++)
            NodalIndex[j]=NodalIndex[j-1]+NumberOfNodeByFace ; 
          
          int tmp_numberOfFaces = constituent->_count[i+1]-constituent->_count[i] ;
          // Il faut ajouter 1 pour le zero a la lecture !!!
          // ATTENTION UNIQUEMENT POUR MED < 2.2.x
          med_2_1::med_int * tmp_constituentArray = NULL;
          if (version_med != "2.2") 
            tmp_constituentArray = new med_2_1::med_int[(NumberOfNodeByFace+1)*tmp_numberOfFaces] ;
          else {
            tmp_constituentArray = new med_2_1::med_int[NumberOfNodeByFace*tmp_numberOfFaces] ;
            MESSAGE_MED(LOC<<": WE ARE USING MED2.2 so there is no +1 for calculating the size of  tmp_constituentArray !") ;
          }

          int err=MEDconnLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
                              Connectivity->_entityDimension,tmp_constituentArray,
                              med_2_1::MED_FULL_INTERLACE,NULL,0,med_2_1::MED_MAILLE,med_type,med_2_1::MED_NOD);

          if ( err != MED_VALID) {
            MESSAGE_MED(LOC<<": MEDconnLire returns "<<err) ;
            delete constituent ;
            delete[] tmp_constituentArray;
            delete[] tmpFaceCount;
            delete[] tmpEdgeCount;
            return MED_ERROR ;
          }

          int * constituentArray = NodalValue + NodalIndex[constituent->_count[i]-1]-1 ;
          
          // version originale sans prise en compte des numéros optionnels
          //    
          for (int j=0; j<tmp_numberOfFaces; j++)
            for (int k=0; k<NumberOfNodeByFace; k++)
              constituentArray[j*NumberOfNodeByFace+k]=(int)(tmp_constituentArray[j*(NumberOfNodeByFace+1)+k]) ;

        //////////////////////////////////////////////////////////////////////////////////////
        ///  Modification pour prise en compte de la numérotation optionnelle des noeuds   ///
        //////////////////////////////////////////////////////////////////////////////////////
        ///
        /// Rénumérote le tableau temporaire tmp_constituentArray en utilisant _optionnalToCanonicNodesNumbers
        /// Le traitement est identique à la version originelle s'il n'y a pas de numérotation optionnelle
        
//      if (_ptrMesh->_arePresentOptionnalNodesNumbers) 
//              {
//              for (int j=0; j<tmp_numberOfFaces; j++) for (int k=0; k<NumberOfNodeByFace; k++)
//                      constituentArray[j*NumberOfNodeByFace+k]=_ptrMesh->_optionnalToCanonicNodesNumbers[tmp_constituentArray[j*(NumberOfNodeByFace+1)+k]] ;
//              }
//      else
//              {
//              for (int j=0; j<tmp_numberOfFaces; j++) for (int k=0; k<NumberOfNodeByFace; k++)
//                      constituentArray[j*NumberOfNodeByFace+k]=tmp_constituentArray[j*(NumberOfNodeByFace+1)+k] ;
//              }
        
        //////////////////////////////////////////////////////////////////////////////////////
          
          delete[] tmp_constituentArray;
        }
        
        constituent->_nodal = new MEDSKYLINEARRAY(constituent->_count[constituent->_numberOfTypes]-1,
                                                  size,
                                                  NodalIndex,
                                                  NodalValue) ;
        delete[] NodalIndex ;
        delete[] NodalValue ;

        Connectivity->_constituent = constituent ;
      }

      delete[] tmpFaceCount;

      // get Edge if any
      // ===============
      if (numberOfEdgesTypes!=0) {
        CONNECTIVITY * constituent = new CONNECTIVITY(numberOfEdgesTypes,MED_EDGE) ;
        constituent->_entityDimension = 1 ;
        constituent->_count[0]=1 ;

        int size = 0 ; 
        int typeNumber=1 ;
        // if you declare a variable <i> in two <for> initialization statement, 
        // compiler gcc2.95.3 says nothing but there are two <i> variables in the same block 
        //and the value you get in the common block seems to be the value of the first variable !
        int i; 

        for ( i=1; i<MED_NBR_GEOMETRIE_MAILLE; i++)  { // no point1 cell type (?)
          if (tmpEdgeCount[i]>0) {
            
            constituent->_count[typeNumber]=constituent->_count[typeNumber-1]+tmpEdgeCount[i];
            CELLMODEL t( (MED_EN::medGeometryElement) MED_MESH_DRIVER21::all_cell_type[i]) ;
            constituent->_type[typeNumber-1]=t ;
            
            constituent->_geometricTypes[typeNumber-1]=( MED_EN::medGeometryElement) MED_MESH_DRIVER21::all_cell_type[i] ;
            
            size+=tmpEdgeCount[i]*((MED_MESH_DRIVER21::all_cell_type[i])%100) ;
            typeNumber++;
          }
        }
        
        // Creation of the MEDSKYLINEARRAY
        //constituent->_nodal = new MEDSKYLINEARRAY(constituent->_count[constituent->_numberOfTypes]-1,size) ; 
        //int * NodalIndex = constituent->_nodal->getIndex() ;
        int * NodalValue = new int[size] ;
        int * NodalIndex = new int[constituent->_count[constituent->_numberOfTypes]] ;
        NodalIndex[0]=1 ;
        
        // Fill the MEDSKYLINEARRAY by reading the MED file.
        for ( i=0; i<constituent->_numberOfTypes; i++) {
          med_2_1::med_geometrie_element med_type = (med_2_1::med_geometrie_element) constituent->_type[i].getType() ;

          int NumberOfNodeByEdge = constituent->_type[i].getNumberOfNodes() ;
          
          // initialise index
          for (int j=constituent->_count[i]; j<constituent->_count[i+1];j++)
            NodalIndex[j]=NodalIndex[j-1]+NumberOfNodeByEdge ; 
          
          int tmp_numberOfEdges = constituent->_count[i+1]-constituent->_count[i] ;
          // Il faut ajouter 1 pour le zero a la lecture !!!

          // ATTENTION UNIQUEMENT POUR MED < 2.2.x
          med_2_1::med_int * tmp_constituentArray = NULL;
          if (version_med != "2.2") 
            tmp_constituentArray = new med_2_1::med_int[(NumberOfNodeByEdge+1)*tmp_numberOfEdges] ;
          else {
            tmp_constituentArray = new med_2_1::med_int[NumberOfNodeByEdge*tmp_numberOfEdges] ;
            MESSAGE_MED(LOC<<": WE ARE USING MED2.2 so there is no +1 for calculating the size of  tmp_constituentArray !") ;
          }
          
          int err=MEDconnLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
                              spaceDimension,tmp_constituentArray,
                              med_2_1::MED_FULL_INTERLACE,NULL,0,med_2_1::MED_MAILLE,
                              med_type,med_2_1::MED_NOD);
          if ( err != MED_VALID) {
            MESSAGE_MED(LOC<<": MEDconnLire returns "<<err) ;
            delete constituent ;
            delete[] tmp_constituentArray;
            delete[] tmpEdgeCount;
            return MED_ERROR ;
          }

          int * constituentArray = NodalValue + NodalIndex[constituent->_count[i]-1]-1 ;
          
          // version originale sans prise en compte des numéros optionnels   
          //
          for (int j=0; j<tmp_numberOfEdges; j++)
            for (int k=0; k<NumberOfNodeByEdge; k++)
              constituentArray[j*NumberOfNodeByEdge+k]=(int)(tmp_constituentArray[j*(NumberOfNodeByEdge+1)+k]) ;

        //////////////////////////////////////////////////////////////////////////////////////
        ///  Modification pour prise en compte de la numérotation optionnelle des noeuds   ///
        //////////////////////////////////////////////////////////////////////////////////////
        ///
        /// Rénumérote le tableau temporaire tmp_constituentArray en utilisant _optionnalToCanonicNodesNumbers
        /// Le traitement est identique à la version originelle s'il n'y a pas de numérotation optionnelle
        
//      if (_ptrMesh->_arePresentOptionnalNodesNumbers) 
//              {
//              for (int j=0; j<tmp_numberOfEdges; j++) for (int k=0; k<NumberOfNodeByEdge; k++)
//                      constituentArray[j*NumberOfNodeByEdge+k]=_ptrMesh->_optionnalToCanonicNodesNumbers[tmp_constituentArray[j*(NumberOfNodeByEdge+1)+k]] ;
//              }
//      else
//              {
//              for (int j=0; j<tmp_numberOfEdges; j++) for (int k=0; k<NumberOfNodeByEdge; k++)
//                      constituentArray[j*NumberOfNodeByEdge+k]=tmp_constituentArray[j*(NumberOfNodeByEdge+1)+k] ;
//              }
        
        //////////////////////////////////////////////////////////////////////////////////////

          delete[] tmp_constituentArray;
        }

        constituent->_nodal = new MEDSKYLINEARRAY(constituent->_count[constituent->_numberOfTypes]-1,
                                                  size,
                                                  NodalIndex,
                                                  NodalValue) ;

        delete[] NodalIndex ;
        delete[] NodalValue ;

        if (Connectivity->_entityDimension == 3) {
          if (Connectivity->_constituent==NULL)
            throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Edges are defined but there are no Faces !"));
          Connectivity->_constituent->_constituent = constituent ;
        } else 
          Connectivity->_constituent = constituent ;
      }

      delete[] tmpEdgeCount; 
      
      return MED_VALID;
    }

  return MED_ERROR;
}

int  MED_MESH_RDONLY_DRIVER21::getFAMILY() 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER21::getFAMILY() : " ;
  BEGIN_OF_MED(LOC);

  if (_status==MED_OPENED) {
    int err = 0 ;

    int * MEDArrayNodeFamily = NULL ;
    int ** MEDArrayCellFamily = NULL ;
    int ** MEDArrayFaceFamily = NULL ;
    int ** MEDArrayEdgeFamily = NULL ;

    if ( !_ptrMesh->getIsAGrid() )
      {
        // read number :
        // NODE :
        MEDArrayNodeFamily = new int[_ptrMesh->getNumberOfNodes()] ;
        err = getNodesFamiliesNumber(MEDArrayNodeFamily) ; // error only if (_status!=MED_OPENED), other case exeception !
        // CELL

        MESSAGE_MED(LOC << "error returned from getNodesFamiliesNumber " << err);

        MEDArrayCellFamily = new int*[_ptrMesh->getNumberOfTypes(MED_CELL)] ; // ET SI IL N'Y A PAS DE CELLS ?
        const medGeometryElement * myTypes = _ptrMesh->getTypes(MED_CELL);
        for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_CELL);i++)
          MEDArrayCellFamily[i] = new int[_ptrMesh->getNumberOfElements(MED_CELL,myTypes[i])] ;

        err = getCellsFamiliesNumber(MEDArrayCellFamily,_ptrMesh->_connectivity,MED_CELL) ;

        MESSAGE_MED(LOC << "error returned from getCellsFamiliesNumber for Cells " << err);

        if (_ptrMesh->_connectivity->_constituent != NULL) {
          if (_ptrMesh->_connectivity->_constituent->_entity == MED_EN::MED_FACE) {
            // FACE
            MEDArrayFaceFamily = new int*[_ptrMesh->getNumberOfTypes(MED_FACE)] ;
            myTypes = _ptrMesh->getTypes(MED_FACE);
            for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_FACE);i++)
              MEDArrayFaceFamily[i] = new int[_ptrMesh->getNumberOfElements(MED_FACE,myTypes[i])] ;

            err = getCellsFamiliesNumber(MEDArrayFaceFamily,_ptrMesh->_connectivity->_constituent,MED_FACE) ;

            MESSAGE_MED(LOC << "error returned from getCellsFamiliesNumber for Faces " << err);

          } else {
            // EDGE in 2D
            MEDArrayEdgeFamily = new int*[_ptrMesh->getNumberOfTypes(MED_EDGE)] ;
            myTypes = _ptrMesh->getTypes(MED_EDGE);
            for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_EDGE);i++)
              MEDArrayEdgeFamily[i] = new int[_ptrMesh->getNumberOfElements(MED_EDGE,myTypes[i])] ;
            err = getCellsFamiliesNumber(MEDArrayEdgeFamily,_ptrMesh->_connectivity->_constituent,MED_EDGE) ;

            MESSAGE_MED(LOC << "error returned from getCellsFamiliesNumber for Edges in 2D " << err);

          }
          // EDGE in 3D
          if (_ptrMesh->_connectivity->_constituent->_constituent != NULL) {
            MEDArrayEdgeFamily = new int*[_ptrMesh->getNumberOfTypes(MED_EDGE)] ;
            myTypes = _ptrMesh->getTypes(MED_EDGE);
            for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_EDGE);i++)
              MEDArrayEdgeFamily[i] = new int[_ptrMesh->getNumberOfElements(MED_EDGE,myTypes[i])] ;
            err = getCellsFamiliesNumber(MEDArrayEdgeFamily,_ptrMesh->_connectivity->_constituent->_constituent,MED_EDGE) ; // we are in 3D !

            MESSAGE_MED(LOC << "error returned from getCellsFamiliesNumber for Edges in 3D " << err);

          }
        }
      }
    else
      {
        // node 
        int NumberOfNodes =  _ptrMesh->getNumberOfNodes() ;
        MEDArrayNodeFamily = new int[ NumberOfNodes ];
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        med_2_1::med_int* MEDArrayNodeFamily2 = new med_2_1::med_int[ NumberOfNodes ];
        err = med_2_1::MEDfamGridLire (_medIdt,
                                      const_cast <char *> (_ptrMesh->_name.c_str()),
                                      MEDArrayNodeFamily2,
                                      NumberOfNodes,
                                      med_2_1::MED_NOEUD);
        for(int i=0;i<NumberOfNodes;i++)
          MEDArrayNodeFamily[i]=(int) MEDArrayNodeFamily2[i];
        delete [] MEDArrayNodeFamily2;
#else
        err = med_2_1::MEDfamGridLire (_medIdt,
                                      const_cast <char *> (_ptrMesh->_name.c_str()),
                                      MEDArrayNodeFamily,
                                      NumberOfNodes,
                                      med_2_1::MED_NOEUD);
#endif
        // what about cell face and edge ?
      }

    // Creation of the families
    int NumberOfFamilies = MEDnFam(_medIdt,const_cast <char *> (_meshName.c_str()),0,med_2_1::MED_FAMILLE) ;
    if ( NumberOfFamilies < 1 && !_ptrMesh->getIsAGrid() ) // at least family 0 must exist 
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"There is no FAMILY, FAMILY 0 must exists" ));

    SCRUTE_MED(NumberOfFamilies);

    vector<FAMILY*> &NodeFamilyVector = _ptrMesh->_familyNode ;
    vector<FAMILY*> &CellFamilyVector = _ptrMesh->_familyCell ;
    vector<FAMILY*> &FaceFamilyVector = _ptrMesh->_familyFace ;
    vector<FAMILY*> &EdgeFamilyVector = _ptrMesh->_familyEdge ;

    int numberOfNodesFamilies = 0 ;
    int numberOfCellsFamilies = 0 ;
    int numberOfFacesFamilies = 0 ;
    int numberOfEdgesFamilies = 0 ;

    for (int i=0;i<NumberOfFamilies;i++) {
      
      med_2_1::med_int NumberOfAttributes = med_2_1::MEDnFam(_medIdt,const_cast <char *> (_meshName.c_str()),i+1,med_2_1::MED_ATTR) ;
      if (NumberOfAttributes < 0) 
        throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER21::getFAMILY() : NumberOfAttributes" );
    
      med_2_1::med_int NumberOfGroups = med_2_1::MEDnFam(_medIdt,const_cast <char *> (_meshName.c_str()),i+1,med_2_1::MED_GROUPE) ;
      if (NumberOfGroups < 0)
        throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER21::getFAMILY() : NumberOfGroups" );
      
      med_2_1::med_int FamilyIdentifier ;
      string FamilyName(MED_TAILLE_NOM,'\0');
      int *  AttributesIdentifier = new int[NumberOfAttributes] ;
      int *  AttributesValues     = new int[NumberOfAttributes] ;
      string AttributesDescription(MED_TAILLE_DESC*NumberOfAttributes,' ') ;
      string GroupsNames(MED_TAILLE_LNOM*NumberOfGroups+1,'\0') ;
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      med_2_1::med_int tmp_NumberOfAttributes=NumberOfAttributes;
      med_2_1::med_int *  AttributesIdentifier2 = new med_2_1::med_int[NumberOfAttributes] ;
      med_2_1::med_int *  AttributesValues2     = new med_2_1::med_int[NumberOfAttributes] ;
      err = med_2_1::MEDfamInfo(_medIdt,const_cast <char *> (_meshName.c_str()),
                               i+1,const_cast <char *> (FamilyName.c_str()),
                               &FamilyIdentifier,AttributesIdentifier2,AttributesValues2,
                               const_cast <char *> (AttributesDescription.c_str()),
                               &NumberOfAttributes,
                               const_cast <char *> (GroupsNames.c_str()),&NumberOfGroups
                       );
      for(med_2_1::med_int i2=0;i2<tmp_NumberOfAttributes;i2++)
        {
          AttributesIdentifier[i2]=(int)(AttributesIdentifier2[i2]);
          AttributesValues[i2]=(int)(AttributesValues2[i2]);
        }
      delete [] AttributesIdentifier2;
      delete [] AttributesValues2;
#else
      err = med_2_1::MEDfamInfo(_medIdt,const_cast <char *> (_meshName.c_str()),
                               i+1,const_cast <char *> (FamilyName.c_str()),
                               &FamilyIdentifier,AttributesIdentifier,AttributesValues,
                               const_cast <char *> (AttributesDescription.c_str()),
                               &NumberOfAttributes,
                               const_cast <char *> (GroupsNames.c_str()),&NumberOfGroups
                       );
#endif

      SCRUTE_MED(GroupsNames);
      SCRUTE_MED(FamilyName);
      SCRUTE_MED(err);
      SCRUTE_MED(i);

      if (err != MED_VALID)
        throw MEDEXCEPTION("MED_MESH_RDONLY_DRIVER21::getFAMILY() : ERROR when get FAMILY informations" );
      if (FamilyIdentifier != 0 ) {
        FAMILY * Family = new FAMILY(_ptrMesh,(int)FamilyIdentifier,FamilyName,
                                     (int)NumberOfAttributes,AttributesIdentifier,
                                     AttributesValues,AttributesDescription,
                                     (int)NumberOfGroups,GroupsNames,
                                     MEDArrayNodeFamily,
                                     MEDArrayCellFamily,
                                     MEDArrayFaceFamily,
                                     MEDArrayEdgeFamily
                                     ) ;
        // All good ?
        // if nothing found, delete Family


        //MESSAGE_MED(LOC << " Well is that OK now ?? " << (*Family));



        if (Family->getNumberOfTypes() == 0) {
          MESSAGE_MED(LOC<<"Nothing found for family "<<FamilyName<< " : skip");
          delete Family;
        } else
          switch (Family->getEntity()) {
          case MED_EN::MED_NODE :
            NodeFamilyVector.push_back(Family) ;
            numberOfNodesFamilies++ ;
            break ;
          case MED_EN::MED_CELL :
            CellFamilyVector.push_back(Family) ;
            numberOfCellsFamilies++ ;
            break ;
          case MED_EN::MED_FACE :
            FaceFamilyVector.push_back(Family) ;
            numberOfFacesFamilies++ ;
            break ;
          case MED_EN::MED_EDGE :
            EdgeFamilyVector.push_back(Family) ;
            numberOfEdgesFamilies++ ;
            break ;
          }

        //      MESSAGE_MED(LOC << (*Family));




      }

      delete [] AttributesIdentifier ;
      delete [] AttributesValues ;
    }

    if (MEDArrayNodeFamily != NULL)
      delete[] MEDArrayNodeFamily ;
    if (MEDArrayCellFamily != NULL) {
      for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_CELL);i++)
        delete[] MEDArrayCellFamily[i] ;
      delete[] MEDArrayCellFamily ;
    }
    if (MEDArrayFaceFamily != NULL) {
      for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_FACE);i++)
        delete[] MEDArrayFaceFamily[i] ;
      delete[] MEDArrayFaceFamily ;
    }
    if (MEDArrayEdgeFamily != NULL) {
      for (int i=0;i<_ptrMesh->getNumberOfTypes(MED_EDGE);i++)
        delete[] MEDArrayEdgeFamily[i] ;
      delete[] MEDArrayEdgeFamily ;
    }

  END_OF_MED(LOC);
    return MED_VALID ;
  }
  return MED_ERROR;
}

int  MED_MESH_RDONLY_DRIVER21::getNodesFamiliesNumber(int * MEDArrayNodeFamily) 
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER21::getNodesFamiliesNumber() : " ;
  BEGIN_OF_MED(LOC);
  if (_status==MED_OPENED) {
    int err = 0 ;
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
    int lgth=_ptrMesh->getNumberOfNodes();
    med_2_1::med_int *temp=new med_2_1::med_int[lgth];
    err = MEDfamLire(_medIdt,(const_cast <char *> (_ptrMesh->_name.c_str())), 
                     temp,
                     _ptrMesh->getNumberOfNodes(),
                     med_2_1::MED_NOEUD,(med_2_1::med_geometrie_element) MED_NONE);
      for(int i2=0;i2<lgth;i2++)
        MEDArrayNodeFamily[i2]=(int)(temp[i2]);
      delete [] temp;
#else
    err = MEDfamLire(_medIdt,(const_cast <char *> (_ptrMesh->_name.c_str())), 
                     MEDArrayNodeFamily,
                     _ptrMesh->getNumberOfNodes(),
                     med_2_1::MED_NOEUD,(med_2_1::med_geometrie_element) MED_NONE);
#endif
    if ( err != MED_VALID) {
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "There is no family for the |"<< _ptrMesh->getNumberOfNodes() 
                                   << "| nodes in mesh |" 
                                   << _ptrMesh->_name.c_str() << "|" ));
    }
  END_OF_MED(LOC);
    return MED_VALID;
  }
  return MED_ERROR;
}

int  MED_MESH_RDONLY_DRIVER21::getCellsFamiliesNumber(int **MEDArrayFamily,
                                                      CONNECTIVITY *Connectivity,
                                                      MED_EN::medEntityMesh entity)
{
  const char * LOC = "MED_MESH_RDONLY_DRIVER21::getCellsFamiliesNumber " ;
  BEGIN_OF_MED(LOC);

  if (_status==MED_OPENED) {
    int i, err = 0 ;
    for (i=0;i<Connectivity->_numberOfTypes;i++)        {
      int NumberOfCell = Connectivity->_count[i+1]-Connectivity->_count[i] ;
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      const medGeometryElement * myTypes = _ptrMesh->getTypes(entity);
      int lgthI=_ptrMesh->getNumberOfElements(entity,myTypes[i]);
      med_2_1::med_int *temp=new med_2_1::med_int[lgthI];
      err=MEDfamLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
                     temp,NumberOfCell,
                     (med_2_1::med_entite_maillage) Connectivity->_entity,
                     (med_2_1::med_geometrie_element) Connectivity->_geometricTypes[i]);
      for(int i2=0;i2<lgthI;i2++)
        MEDArrayFamily[i][i2]=(int)(temp[i2]);
      delete [] temp;
#else
      err=MEDfamLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
                     MEDArrayFamily[i],NumberOfCell,
                     (med_2_1::med_entite_maillage) Connectivity->_entity,
                     (med_2_1::med_geometrie_element) Connectivity->_geometricTypes[i]);
#endif

      // provisoire : si les faces ou les aretes sont des mailles !!!
      if (err != MED_VALID) {
        MESSAGE_MED(LOC<<"search face/edge family on cell !!!");
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        int lgthI=_ptrMesh->getNumberOfElements(entity,myTypes[i]);
        med_2_1::med_int *temp=new med_2_1::med_int[lgthI];
        err=MEDfamLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
                       temp,NumberOfCell,
                       med_2_1::MED_MAILLE,
                       (med_2_1::med_geometrie_element) Connectivity->_geometricTypes[i]);
        for(int i2=0;i2<lgthI;i2++)
          MEDArrayFamily[i][i2]=(int)(temp[i2]);
        delete [] temp;
#else
        err=MEDfamLire(_medIdt,const_cast <char *> (_ptrMesh->_name.c_str()),
                       MEDArrayFamily[i],NumberOfCell,
                       med_2_1::MED_MAILLE,
                       (med_2_1::med_geometrie_element) Connectivity->_geometricTypes[i]);
#endif
      }

      if (err != MED_VALID) 
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<" Family not found for entity "<<Connectivity->_entity<<" and geometric type "<<Connectivity->_geometricTypes[i]));
      
    }
    return MED_VALID;
  }
  return MED_ERROR;  
}

/*--------------------- WRONLY PART -------------------------------*/

MED_MESH_WRONLY_DRIVER21::MED_MESH_WRONLY_DRIVER21()
{
  this->GENDRIVER::_accessMode = MED_EN::WRONLY;
}
  
MED_MESH_WRONLY_DRIVER21::MED_MESH_WRONLY_DRIVER21(const string & fileName,
                                                   MESH * ptrMesh):
  MED_MESH_DRIVER21(fileName,ptrMesh,WRONLY),
  IMED_MESH_WRONLY_DRIVER(fileName,ptrMesh),
  MED_MESH_DRIVER(fileName,ptrMesh,WRONLY)
{
  MESSAGE_MED("MED_MESH_WRONLY_DRIVER21::MED_MESH_WRONLY_DRIVER(const string & fileName, MESH * ptrMesh) has been created");
}

MED_MESH_WRONLY_DRIVER21::MED_MESH_WRONLY_DRIVER21(const MED_MESH_WRONLY_DRIVER21 & driver): 
  IMED_MESH_WRONLY_DRIVER(driver),MED_MESH_DRIVER21(driver),MED_MESH_DRIVER(driver)
{
}

MED_MESH_WRONLY_DRIVER21::~MED_MESH_WRONLY_DRIVER21()
{
  //MESSAGE_MED("MED_MESH_WRONLY_DRIVER21::MED_MESH_WRONLY_DRIVER21(const string & fileName, MESH * ptrMesh) has been destroyed");
}

GENDRIVER * MED_MESH_WRONLY_DRIVER21::copy(void) const
{
  return new MED_MESH_WRONLY_DRIVER21(*this);
}

void MED_MESH_WRONLY_DRIVER21::write(void) const
{ 
  const char * LOC = "void MED_MESH_WRONLY_DRIVER21::write(void) const : ";
  BEGIN_OF_MED(LOC);

  // we must first create mesh !!
  MESSAGE_MED(LOC << "MeshName : |" << _meshName << "| FileName : |"<<_fileName<<"| MedIdt : | "<< _medIdt << "|");

  if (_status!=MED_OPENED)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "File "<<_fileName<<" is not open. Open it before write !"));

  if (_ptrMesh->_spaceDimension == MED_INVALID )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Mesh was not defined before calling write()"));

  if (_meshName.size() > MED_TAILLE_NOM )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Mesh name |"<<_meshName<<
                                 "| in object driver MESH is longer than " << MED_TAILLE_NOM));

  if (_ptrMesh->getIsAGrid())
  {
    if ( writeGRID() != MED_VALID )
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeGRID()"  )) ;
  }
  else
  {
    if (writeCoordinates()!=MED_VALID)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeCoordinates()"  )) ;

    if (writeConnectivities(MED_EN::MED_CELL)!=MED_VALID)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeConnectivities(MED_CELL)"  )) ;
    if (writeConnectivities(MED_EN::MED_FACE)!=MED_VALID)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeConnectivities(MED_FACE)"  )) ;
    if (writeConnectivities(MED_EN::MED_EDGE)!=MED_VALID)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeConnectivities(MED_EDGE)"  )) ;
  }

  if (writeFamilyNumbers() !=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeFamilyNumbers()"  )) ;
  

  // well we must first write zero family :
  if (_status==MED_OPENED) {
    int err ;
    // test if the family already exists (HDF trick waiting a MED evolution to be replaced)
    string dataGroupFam = "/ENS_MAA/"+_meshName+"/FAS/FAMILLE_0/";  
    MESSAGE_MED("|"<<dataGroupFam<<"|");
    err = med_2_1::_MEDdatagroupOuvrir(_medIdt,const_cast <char *> (dataGroupFam.c_str()) );
    if ( err < MED_VALID ) {
      SCRUTE_MED(err);
      
      err = med_2_1::MEDfamCr( _medIdt,
                              const_cast <char *> ( _meshName.c_str() ),
                              (char*)"FAMILLE_0", 0,
                              (med_2_1::med_int*)NULL, (med_2_1::med_int*)NULL, (char*)NULL, 0,
                              (char*)NULL, 0);
      
      if ( err != MED_VALID) 
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't create family |FAMILLE_0| with identifier |0| groups names || and  attributes descriptions ||")) ;
    }
    else
      med_2_1::_MEDdatagroupFermer(_medIdt);
     
  }

  MESSAGE_MED(LOC<<"writeFamilies(_ptrMesh->_familyNode)");
  if (writeFamilies(_ptrMesh->_familyNode) !=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeFamilies(_ptrMesh->_familyNode)"  )) ;

  MESSAGE_MED(LOC<<"writeFamilies(_ptrMesh->_familyCell)");
  if (writeFamilies(_ptrMesh->_familyCell) !=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeFamilies(_ptrMesh->_familyCell)"  )) ;

  MESSAGE_MED(LOC<<"writeFamilies(_ptrMesh->_familyFace)");
  if (writeFamilies(_ptrMesh->_familyFace) !=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeFamilies(_ptrMesh->_familyFace)"  )) ;

  MESSAGE_MED(LOC<<"writeFamilies(_ptrMesh->_familyEdge)");
  if (writeFamilies(_ptrMesh->_familyEdge) !=MED_VALID)
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "ERROR in writeFamilies(_ptrMesh->_familyEdge)"  )) ;

  END_OF_MED(LOC);
} 

//=======================================================================
//function : writeGRID
//purpose  : 
//=======================================================================

int MED_MESH_WRONLY_DRIVER21::writeGRID() const
{
  const char * LOC = "MED_MESH_WRONLY_DRIVER21::writeGRID() : " ;
  BEGIN_OF_MED(LOC);
  
  if (_status!=MED_OPENED)
  {
    MESSAGE_MED (LOC<<" Not open !!!");
    return MED_ERROR;
  }
  GRID * ptrGrid = (GRID*) _ptrMesh;
  
  med_2_1::med_err err = MED_ERROR;
  med_2_1::med_repere rep;
  string tmp_name(_ptrMesh->_spaceDimension*MED_TAILLE_PNOM21,' ');
  string tmp_unit(_ptrMesh->_spaceDimension*MED_TAILLE_PNOM21,' ');

  // Test if the mesh <_meshName> already exists
  // If it doesn't exists create it
  // If it already exists verify if its dimension is the same as <_ptrMesh->_spaceDimension>
  // rem : <_meshName> is the driver meshName not <ptrMesh->_meshName>
  int dim = med_2_1::MEDdimLire(_medIdt, const_cast <char *> (_meshName.c_str()) );
  if (dim < MED_VALID)
  {
    err = med_2_1::MEDgridCr(_medIdt,
                    const_cast <char *> (_meshName.c_str()),
                    _ptrMesh->_spaceDimension,
                    (med_2_1::med_grid_type) ptrGrid->getGridType());
    if (err != MED_VALID)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Unable to create Grid"));
    else 
      MESSAGE_MED(LOC<<"Grid "<<_meshName<<" created in file "<<_fileName<<" !");
  }
  else if (dim != _ptrMesh->_spaceDimension) 
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Grid |" << _meshName.c_str() <<
                                 "| already exists in file |" << _fileName <<
                                 "| with dimension |" << dim <<
                                 "| but the dimension of the mesh we want to write is |"
                                 << _ptrMesh->_spaceDimension <<"|" )) ;

  // Recompose the <_spaceDimension> strings in 1 string 
  int lengthString ;
  string valueString ;
  for (int i=0;i<_ptrMesh->_spaceDimension;i++) {
    SCRUTE_MED(i);
    valueString = _ptrMesh->_coordinate->_coordinateName[i] ;
    lengthString = (MED_TAILLE_PNOM21<valueString.size())?MED_TAILLE_PNOM21:valueString.size() ;
    tmp_name.replace(i*MED_TAILLE_PNOM21,i*MED_TAILLE_PNOM21+lengthString,valueString,0,lengthString);
    valueString = _ptrMesh->_coordinate->_coordinateUnit[i];
    lengthString = (MED_TAILLE_PNOM21<valueString.size())?MED_TAILLE_PNOM21:valueString.size() ;
    tmp_unit.replace(i*MED_TAILLE_PNOM21,i*MED_TAILLE_PNOM21+lengthString,valueString,0,lengthString);
  }

  // Pourquoi le stocker sous forme de chaÃ®ne ?
  const string & coordinateSystem = _ptrMesh->_coordinate->_coordinateSystem;
  if      (coordinateSystem  == "CARTESIAN") 
    rep = med_2_1::MED_CART;
  else if ( coordinateSystem == "CYLINDRICAL")
    rep = med_2_1::MED_CYL;
  else if ( coordinateSystem == "SPHERICAL" )
    rep = med_2_1::MED_SPHER;
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Grid |" << _meshName.c_str() <<
                                 "| doesn't have a valid coordinate system : |" 
                                 << _ptrMesh->_coordinate->_coordinateSystem
                                 << "|" )) ;  

  med_2_1::med_int ArrayLen[] = { (med_2_1::med_int) ptrGrid->_iArrayLength,
                     (med_2_1::med_int) ptrGrid->_jArrayLength,
                     (med_2_1::med_int) ptrGrid->_kArrayLength  };
  
  // Write node coordinates for MED_BODY_FITTED grid
  if (ptrGrid->getGridType() == MED_EN::MED_BODY_FITTED)
  {

    // Write Coordinates and families
    double * coo = const_cast <double *>
      (_ptrMesh->_coordinate->getCoordinates(MED_EN::MED_FULL_INTERLACE));
    
    // Write unused families
    //CCRT unused => med_2_1::med_int
    med_2_1::med_int * MEDArrayNodeFamily = new med_2_1::med_int[_ptrMesh->_numberOfNodes] ;

    err = MEDbodyFittedEcr (_medIdt,
                            const_cast <char *> (_meshName.c_str()),
                            _ptrMesh->_spaceDimension,
                            coo,
                            ArrayLen,
                            med_2_1::MED_FULL_INTERLACE,
                            rep,
                            const_cast <char *> (tmp_name.c_str()),
                            const_cast <char *> (tmp_unit.c_str()),
                            MEDArrayNodeFamily,
                            _ptrMesh->_numberOfNodes,
                            med_2_1::MED_REMP);
    delete[] MEDArrayNodeFamily;

    if (err != MED_VALID)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"error in MEDbodyFittedEcr()"));

  }
  else
  {
    // Write Arrays of Cartesian or Polar Grid

    double * Array[] = { ptrGrid->_iArray,
                         ptrGrid->_jArray,
                         ptrGrid->_kArray };
    int idim;
    for (idim = 0; idim < _ptrMesh->_spaceDimension; ++idim)
    {
      err = med_2_1::MEDgridEcr (_medIdt,
                        const_cast <char *> (_meshName.c_str()),
                        _ptrMesh->_spaceDimension,
                        Array [idim],
                        ArrayLen [idim],
                        idim,
                        med_2_1::MED_FULL_INTERLACE,
                        rep,
                        const_cast <char *> (tmp_name.c_str()),
                        const_cast <char *> (tmp_unit.c_str()),
                        med_2_1::MED_REMP);
      if (err != MED_VALID)
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't read grid coordinates for "
                                     << idim << "-th dimention"));
    }

//      err = MEDfamGridEcr(_medIdt,
//                          const_cast <char *> (_meshName.c_str()),
//                          _ptrMesh->_MEDArrayNodeFamily,
//                          _ptrMesh->_numberOfNodes,
//                          med_2_1::MED_REMP,
//                          med_2_1::MED_NOEUD);
    if (err != MED_VALID)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"error in MEDfamGridEcr()"));

  } // end Write  Cartesian or Polar Grid

  END_OF_MED(LOC);
  return MED_VALID;
}

//=======================================================================
//function : writeCoordinates
//purpose  : 
//=======================================================================

int MED_MESH_WRONLY_DRIVER21::writeCoordinates() const {
 
  const char * LOC = "int MED_MESH_WRONLY_DRIVER21::writeCoordinates() const : ";
  BEGIN_OF_MED(LOC);

  med_2_1::med_err err = MED_ERROR;
  med_2_1::med_repere rep;
  string tmp_name(_ptrMesh->_spaceDimension*MED_TAILLE_PNOM21,' ');
  string tmp_unit(_ptrMesh->_spaceDimension*MED_TAILLE_PNOM21,' ');
    
  // Recompose the <_spaceDimension> strings in 1 string 
  int lengthString ;
  string valueString ;
  for (int i=0;i<_ptrMesh->_spaceDimension;i++) {
    valueString = _ptrMesh->_coordinate->_coordinateName[i] ;
    lengthString = (MED_TAILLE_PNOM21<valueString.size())?MED_TAILLE_PNOM21:valueString.size() ;
    tmp_name.replace(i*MED_TAILLE_PNOM21,i*MED_TAILLE_PNOM21+lengthString,valueString,0,lengthString);
    valueString = _ptrMesh->_coordinate->_coordinateUnit[i];
    lengthString = (MED_TAILLE_PNOM21<valueString.size())?MED_TAILLE_PNOM21:valueString.size() ;
    tmp_unit.replace(i*MED_TAILLE_PNOM21,i*MED_TAILLE_PNOM21+lengthString,valueString,0,lengthString);
  }

  // Test if the mesh <_meshName> already exists
  // If it doesn't exists create it
  // If it already exists verify if its dimension is the same as <_ptrMesh->_spaceDimension>
  // rem : <_meshName> is the driver meshName not <ptrMesh->_meshName>
  int dim = med_2_1::MEDdimLire(_medIdt, const_cast <char *> (_meshName.c_str()) );
  if (dim < MED_VALID)
    if (med_2_1::MEDmaaCr(_medIdt,const_cast <char *> (_meshName.c_str()),_ptrMesh->_spaceDimension) != 0 )
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Unable to create Mesh : |" << _meshName << "|"));
    else 
      {
        MESSAGE_MED(LOC<<"Mesh "<<_meshName<<" created in file "<<_fileName<<" !");
      }
  else if (dim != _ptrMesh->_spaceDimension) 
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Mesh |" << _meshName.c_str() << "| already exists in file |" << _fileName
                                 << "| with dimension |" << dim << "| but the dimension of the mesh we want to write is |"
                                 << _ptrMesh->_spaceDimension <<"|" )) ;
    
  // Pourquoi le stocker sous forme de chaÃ®ne ?
  const string & coordinateSystem = _ptrMesh->_coordinate->_coordinateSystem;
  if      (coordinateSystem  == "CARTESIAN") 
    rep = med_2_1::MED_CART;
  else if ( coordinateSystem == "CYLINDRICAL")
    rep = med_2_1::MED_CYL;
  else if ( coordinateSystem == "SPHERICAL" )
    rep = med_2_1::MED_SPHER;
  else
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Mesh |" << _meshName.c_str() << "| doesn't have a valid coordinate system : |" 
                                 << _ptrMesh->_coordinate->_coordinateSystem
                                 << "|" )) ;  
      
  err = MEDcoordEcr(_medIdt, const_cast <char *> (_meshName.c_str()),
                    _ptrMesh->_spaceDimension, 
                    //const_cast <double *> ( _ptrMesh->_coordinate->_coordinate->get(MED_EN::MED_FULL_INTERLACE) ), 
                    const_cast <double *> ( _ptrMesh->_coordinate->_coordinate.get(MED_EN::MED_FULL_INTERLACE) ), 
                    med_2_1::MED_FULL_INTERLACE, 
                    _ptrMesh->_numberOfNodes,                 //  _ptrMesh->_coordinate->_numberOfNodes
                    med_2_1::MED_REMP,    
                    rep,
                    const_cast <char *> (tmp_name.c_str()), 
                    const_cast <char *> (tmp_unit.c_str()) 
                    );  

  if (err<0) 
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write coordinates of mesh |" << _meshName.c_str() << "| in file |" << _fileName
                                 << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and" 
                                 << " with units names |"  << tmp_name
                                 << "| and units |"       << tmp_unit
                                 << " |")) ;    
    

       //////////////////////////////////////////////////////////////////////////////////////
       ///  Modification pour prise en compte de la numérotation optionnelle des noeuds   ///
       //////////////////////////////////////////////////////////////////////////////////////
       ///
       /// Ecrit les numéros optionnels des noeuds
       /// Le traitement est identique à la version originelle s'il n'y a pas de numérotation optionnelle


      if (_ptrMesh->_arePresentOptionnalNodesNumbers==1) {
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        int lgth=_ptrMesh->_coordinate->getNumberOfNodes();
        med_2_1::med_int *temp=new med_2_1::med_int[lgth];
        for(int i2=0;i2<lgth;i2++)
          temp[i2]=(med_2_1::med_int)(_ptrMesh->_coordinate->getNodesNumbers()[i2]);
        err =  MEDnumEcr(_medIdt,const_cast <char *> (_meshName.c_str()),
                         temp, 
                         _ptrMesh->_numberOfNodes, med_2_1::MED_REMP,
                         med_2_1::MED_NOEUD, med_2_1::med_geometrie_element(0) );
        delete [] temp;
#else        
        err =  MEDnumEcr(_medIdt,const_cast <char *> (_meshName.c_str()),
                         const_cast <int *> (_ptrMesh->_coordinate->getNodesNumbers() ), 
                         _ptrMesh->_numberOfNodes, med_2_1::MED_REMP,
                         med_2_1::MED_NOEUD, med_2_1::med_geometrie_element(0) );
#endif

        if (err<0) 
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write optionnal numbers of mesh |" << _meshName.c_str() 
                                       << "| in file |" << _fileName  << " |")) ;    
      }
      //////////////////////////////////////////////////////////////////////////////////////

  END_OF_MED(LOC);
    
  return MED_VALID;
}




int MED_MESH_WRONLY_DRIVER21::writeConnectivities(medEntityMesh entity) const {
  
  const char * LOC="int MED_MESH_WRONLY_DRIVER21::writeConnectivities() const : ";
  BEGIN_OF_MED(LOC);

  med_2_1::med_err err;
  
  // REM SI LA METHODE EST APPELEE DIRECTEMENT ET QUE LE MAILLAGE N'EST PAS CREE IL Y A PB !
  // PG : IMPOSSIBLE : LA METHODE EST PRIVEE !
    
    // A FAIRE : A tester surtout dans les methodes de MESH.
    //    if ( _ptrMesh->_connectivity == (CONNECTIVITY *) MED_INVALID )
  if ( _ptrMesh->_connectivity == (CONNECTIVITY *) NULL )
    throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "The connectivity is not defined in the MESH object ")) ;
    
  if   ( _ptrMesh->existConnectivity(MED_NODAL,entity) ) { 

    int numberOfTypes           = _ptrMesh->getNumberOfTypes (entity) ;
    const medGeometryElement  * types = _ptrMesh->getTypes         (entity) ;
    
    for (int i=0; i<numberOfTypes; i++) {
      
      int    numberOfElements = _ptrMesh->getNumberOfElements (entity,types[i]);
      const int * connectivity      = _ptrMesh->getConnectivity     (MED_EN::MED_FULL_INTERLACE, 
                                                                     MED_NODAL, entity, types[i]); // ?? et SI MED_NODAL n'existe pas, recalcul auto ??
      
      // Pour l'instant la class utilise le multi.....
      int multi = 0 ;
      if (entity==MED_EN::MED_CELL)
        if ( (types[i]/ 100) < _ptrMesh->_spaceDimension) 
          multi=1 ;
      int numberOfNodes = types[i]%100 ;
      int * connectivityArray = new int[numberOfElements*(numberOfNodes+multi)];
      
      // version originale sans prise en compte des numéros optionnels 
      //     
      for (int j=0 ; j<numberOfElements; j++) 
        {
          for (int k=0; k<numberOfNodes; k++)
            connectivityArray[j*(numberOfNodes+multi)+k]=connectivity[j*numberOfNodes+k] ;

          if (multi>0) connectivityArray[j*(numberOfNodes+multi)+numberOfNodes]=0;
        }
      
      //////////////////////////////////////////////////////////////////////////////////////
      ///  Modification pour prise en compte de la numérotation optionnelle des noeuds   ///
      //////////////////////////////////////////////////////////////////////////////////////
      ///
      /// Dénumérote les sommets des mailles pour leur rendre leurs numéros optionnels
      /// Le traitement est identique à la version originelle s'il n'y a pas de numérotation optionnelle

//      if (_ptrMesh->_arePresentOptionnalNodesNumbers==1) 
//              {
//              const int * nodesNumbers = _ptrMesh->_coordinate->getNodesNumbers();
//                      for (int j=0 ; j<numberOfElements; j++) 
//                      {
//                      for (int k=0; k<numberOfNodes; k++) connectivityArray[j*(numberOfNodes+multi)+k]=nodesNumbers[connectivity[j*numberOfNodes+k]-1] ;
//                      if (multi>0) connectivityArray[j*(numberOfNodes+multi)+numberOfNodes]=0;
//                      }
//              }
//      else
//              {
//                      for (int j=0 ; j<numberOfElements; j++) 
//                      {
//                      for (int k=0; k<numberOfNodes; k++) connectivityArray[j*(numberOfNodes+multi)+k]=connectivity[j*numberOfNodes+k] ;
//                      if (multi>0) connectivityArray[j*(numberOfNodes+multi)+numberOfNodes]=0;
//                      }
//              }

      //////////////////////////////////////////////////////////////////////////////////////
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      int lgth=numberOfElements*(numberOfNodes+multi);
      med_2_1::med_int *temp=new med_2_1::med_int[lgth];
      for(int i2=0;i2<lgth;i2++)
        temp[i2]=(med_2_1::med_int)(connectivityArray[i2]);
      err = MEDconnEcr( _medIdt, const_cast <char *> ( _meshName.c_str()), _ptrMesh->_spaceDimension,
                        temp, med_2_1::MED_FULL_INTERLACE , numberOfElements,
                        med_2_1::MED_REMP,
                        (med_2_1::med_entite_maillage  ) entity, 
                        (med_2_1::med_geometrie_element) types[i], med_2_1::MED_NOD );
      delete [] temp;
#else
      err = MEDconnEcr( _medIdt, const_cast <char *> ( _meshName.c_str()), _ptrMesh->_spaceDimension,
                        connectivityArray, med_2_1::MED_FULL_INTERLACE , numberOfElements,
                        med_2_1::MED_REMP,
                        (med_2_1::med_entite_maillage  ) entity, 
                        (med_2_1::med_geometrie_element) types[i], med_2_1::MED_NOD );
#endif
      delete[] connectivityArray ;
      if (err<0) // ETENDRE LES EXPLICATIONS
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write connectivities of mesh |" << _meshName.c_str() << "| in file |" << _fileName
                                     << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and" 
                                     )) ;
    }
  }
  // Connctivity descending :
  if   ( _ptrMesh->existConnectivity(MED_DESCENDING,entity) ) { 
      
    int numberOfTypes           = _ptrMesh->getNumberOfTypes (entity) ;
    const medGeometryElement  * types = _ptrMesh->getTypes         (entity) ;
      
    for (int i=0; i<numberOfTypes; i++) {
        
      int    numberOfElements = _ptrMesh->getNumberOfElements (entity,types[i]);
      const int * connectivity = _ptrMesh->getConnectivity(MED_EN::MED_FULL_INTERLACE, MED_DESCENDING, entity, types[i]); 
      
      // Pour l'instant la class utilise le multi.....
      int multi = 0 ;
      if (entity==MED_EN::MED_CELL)
        if ( (types[i]/ 100) < _ptrMesh->_spaceDimension) 
          multi=1 ;

      int dim, numberOfNod, numberOfDesc ;
      if (_MEDparametresGeometrie(med_2_1::med_entite_maillage(entity),
                                  med_2_1::med_geometrie_element(types[i]),
                                  &dim,&numberOfNod,&numberOfDesc) < 0 )
        continue;
      // descending connectivity can be partial
      int connLen = _ptrMesh->getConnectivityLength(MED_EN::MED_FULL_INTERLACE, MED_DESCENDING,
                                                    entity, types[i]);
      int nbEl = min( numberOfElements, connLen / numberOfDesc );
      int * connectivityArray = new int[numberOfElements*(numberOfDesc+multi)];
      for (int j=0 ; j<nbEl; j++)
        {
          for (int k=0; k<numberOfDesc; k++)
            connectivityArray[j*(numberOfDesc+multi)+k]=connectivity[j*numberOfDesc+k];

          if (multi>0) connectivityArray[j*(numberOfDesc+multi)+numberOfDesc]=0;
        }
      for (int j=connLen; j < numberOfDesc*numberOfElements; ++j)
        connectivityArray[j]=0;
      
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      int lgth=numberOfElements*(numberOfDesc+multi);
      med_2_1::med_int *temp=new med_2_1::med_int[lgth];
      for(int i2=0;i2<lgth;i2++)
        temp[i2]=(med_2_1::med_int)(connectivityArray[i2]);
      err = med_2_1::MEDconnEcr( _medIdt,
                                const_cast <char *> ( _meshName.c_str()),
                                _ptrMesh->_spaceDimension,
                                temp,
                                med_2_1::MED_FULL_INTERLACE,
                                numberOfElements,
                                med_2_1::MED_REMP,
                                (med_2_1::med_entite_maillage  ) entity, 
                                (med_2_1::med_geometrie_element) types[i],
                                med_2_1::MED_DESC );
      delete [] temp;
#else
      err = med_2_1::MEDconnEcr( _medIdt,
                                const_cast <char *> ( _meshName.c_str()),
                                _ptrMesh->_spaceDimension,
                                const_cast <int *> (connectivityArray),
                                med_2_1::MED_FULL_INTERLACE,
                                numberOfElements,
                                med_2_1::MED_REMP,
                                (med_2_1::med_entite_maillage  ) entity, 
                                (med_2_1::med_geometrie_element) types[i],
                                med_2_1::MED_DESC );
#endif
      delete[] connectivityArray ;
      if (err<0) // ETENDRE LES EXPLICATIONS
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) <<"Can't write connectivities of mesh |" << _meshName.c_str() << "| in file |" << _fileName
                                     << "| with dimension |"  << _ptrMesh->_spaceDimension <<"| and" 
                                     )) ;
            
    }
  }
  END_OF_MED(LOC);
  return MED_VALID;
}

int MED_MESH_WRONLY_DRIVER21::writeFamilyNumbers() const {
  
  const char * LOC="int MED_MESH_WRONLY_DRIVER21::writeFamilyNumbers() const : ";
  BEGIN_OF_MED(LOC);

  med_2_1::med_err err;
  
  // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArrayNodeFamily DOIT ETRE ENLEVER DE LA CLASSE MESH

  { // Node related block
    
    // We build the array from the families list objects :
    int NumberOfNodes = _ptrMesh->getNumberOfNodes() ;
    int * MEDArrayNodeFamily = new int[NumberOfNodes] ;
    // family 0 by default
    for (int i=0; i<NumberOfNodes; i++)
      MEDArrayNodeFamily[i]=0;
    //vector<FAMILY*> myFamilies = _ptrMesh->getFamilies(MED_NODE);
    vector<FAMILY*> * myFamilies = &_ptrMesh->_familyNode;
    int NumberOfNodesFamilies = myFamilies->size() ;
    //bool ToDestroy = false;
    if (0 == NumberOfNodesFamilies) {
      //ToDestroy = true ;
      vector<GROUP*> myGroups = _ptrMesh->getGroups(MED_NODE);
      int NumberOfGroups = myGroups.size() ;
      // build families from groups
      for (int i=0; i<NumberOfGroups; i++) {
        SUPPORT * mySupport = myGroups[i] ;
        FAMILY* myFamily = new FAMILY(*mySupport);
        myFamily->setIdentifier(i+1);
        myFamilies->push_back(myFamily);
      }
      NumberOfNodesFamilies=myFamilies->size() ;
    }
    for (int i=0 ; i<NumberOfNodesFamilies; i++) {
      //SCRUTE_MED(i);
      //SCRUTE_MED(myFamilies[i]->getName());
      int FamilyIdentifier = (*myFamilies)[i]->getIdentifier() ;
      int TotalNumber = (*myFamilies)[i]->getNumberOfElements(MED_ALL_ELEMENTS) ;
      if ((*myFamilies)[i]->isOnAllElements())
        for (int j=0; j<TotalNumber; j++)
          MEDArrayNodeFamily[j]=FamilyIdentifier;
      else {
        const int * Number = (*myFamilies)[i]->getNumber(MED_ALL_ELEMENTS) ;
        for (int j=0; j<TotalNumber; j++)
          MEDArrayNodeFamily[Number[j]-1]=FamilyIdentifier ;
      }
    }
    for(int j=0; j<NumberOfNodes; j++) {
      SCRUTE_MED(MEDArrayNodeFamily[j]);
    }
    if ( !_ptrMesh->getIsAGrid() ){
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      int lgth=NumberOfNodes;
      med_2_1::med_int *temp=new med_2_1::med_int[lgth];
      for(int i2=0;i2<lgth;i2++)
        temp[i2]=(med_2_1::med_int)(MEDArrayNodeFamily[i2]);
      err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
                      temp, NumberOfNodes,med_2_1::MED_REMP ,
                      med_2_1::MED_NOEUD,
                      (med_2_1::med_geometrie_element) MED_NONE);
      delete [] temp;
#else
      err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
                      MEDArrayNodeFamily, NumberOfNodes,med_2_1::MED_REMP ,
                      med_2_1::MED_NOEUD,
                      (med_2_1::med_geometrie_element) MED_NONE); 
#endif
    }
    else{
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      int lgth=NumberOfNodes;
      med_2_1::med_int *temp=new med_2_1::med_int[lgth];
      for(int i2=0;i2<lgth;i2++)
        temp[i2]=(med_2_1::med_int)(MEDArrayNodeFamily[i2]);
      err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
                      temp, NumberOfNodes,med_2_1::MED_REMP ,
                      med_2_1::MED_NOEUD,
                      (med_2_1::med_geometrie_element) MED_NONE);
      delete [] temp;
#else
      err = MEDfamGridEcr(_medIdt,
                          const_cast <char *> (_meshName.c_str()),
                          MEDArrayNodeFamily,
                          NumberOfNodes,
                          med_2_1::MED_REMP,
                          med_2_1::MED_NOEUD);
#endif
    }

    if ( err != MED_VALID) 
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write node family for the |"<< NumberOfNodes
                                   << "| nodes in mesh |" 
                                   << _ptrMesh->_name.c_str() << "|" ));
    delete[] MEDArrayNodeFamily;
    //if (true == ToDestroy)
    //  for (int i=0; i<NumberOfNodesFamilies; i++)
    //    delete myFamilies[i];
  }
    
  { // CELLS RELATED BLOCK
    medEntityMesh entity=MED_EN::MED_CELL;
    // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArray____Family DOIT ETRE ENLEVER DE LA CLASSE MESH
    if  ( ( _ptrMesh->existConnectivity(MED_NODAL,entity) )|( _ptrMesh->existConnectivity(MED_DESCENDING,entity) ) ) { 

      int numberOfTypes           = _ptrMesh->getNumberOfTypes (entity) ;
      const medGeometryElement  * types = _ptrMesh->getTypes         (entity) ;

      // We build the array from the families list objects :
      int NumberOfElements = _ptrMesh->getNumberOfElements(entity, MED_ALL_ELEMENTS);
      int * MEDArrayFamily = new int[NumberOfElements] ;
      // family 0 by default
      for (int i=0; i<NumberOfElements; i++)
        MEDArrayFamily[i]=0;
      //vector<FAMILY*> myFamilies = _ptrMesh->getFamilies(entity);
      vector<FAMILY*> * myFamilies = &_ptrMesh->_familyCell ;
      int NumberOfFamilies = myFamilies->size() ;
      //bool ToDestroy = false;
      if (0 == NumberOfFamilies) {
        //ToDestroy = true ;
        vector<GROUP*> myGroups = _ptrMesh->getGroups(entity);
        int NumberOfGroups = myGroups.size() ;
        // build families from groups
        for (int i=0; i<NumberOfGroups; i++) {
          SCRUTE_MED( myGroups[i]->getName() );
          SUPPORT * mySupport = myGroups[i] ;
          FAMILY* myFamily = new FAMILY(*mySupport);
          myFamily->setIdentifier(-i-1);
          myFamilies->push_back(myFamily);
        }
        NumberOfFamilies=myFamilies->size() ;
      }
      for (int i=0 ; i<NumberOfFamilies; i++) {
        int FamilyIdentifier = (*myFamilies)[i]->getIdentifier() ;
        int TotalNumber = (*myFamilies)[i]->getNumberOfElements(MED_ALL_ELEMENTS) ;
        if ((*myFamilies)[i]->isOnAllElements())
          for (int ii=0; ii<TotalNumber; ii++)
            MEDArrayFamily[ii]=FamilyIdentifier;
        else {
          const int * Number = (*myFamilies)[i]->getNumber(MED_ALL_ELEMENTS) ;
          for (int ii=0; ii<TotalNumber; ii++)
            MEDArrayFamily[Number[ii]-1]=FamilyIdentifier ;
        }
      }

      const int * typeCount = _ptrMesh->getGlobalNumberingIndex(entity) ;
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        int lgth=NumberOfElements;
        med_2_1::med_int *temp=new med_2_1::med_int[lgth];
        for(int i2=0;i2<lgth;i2++)
          temp[i2]=(med_2_1::med_int) (MEDArrayFamily[i2]);
#endif
      for (int i=0; i<numberOfTypes; i++) {
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
                        temp+typeCount[i]-1,typeCount[i+1]-typeCount[i],
                        med_2_1::MED_REMP ,
                        (med_2_1::med_entite_maillage) entity,
                        (med_2_1::med_geometrie_element) types[i]
);
#else
        err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
                        MEDArrayFamily+typeCount[i]-1,typeCount[i+1]-typeCount[i],
                        med_2_1::MED_REMP ,
                        (med_2_1::med_entite_maillage) entity,
                        (med_2_1::med_geometrie_element) types[i]
); 
#endif
        MESSAGE_MED("OK "<<i);
        if ( err != MED_VALID) 
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write family for the |"<< _ptrMesh->getNumberOfElements(entity, types[i])
                                       << "| cells of geometric type |" << geoNames[ types[i]] <<"|in mesh |"      
                                       << _ptrMesh->_name.c_str() << "|" ));   
      }
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      delete [] temp;
#endif
      delete[] MEDArrayFamily ;
      //if (true == ToDestroy) {
      //  int NumberOfFamilies = myFamilies->size();
      //  for (int i=0; i<NumberOfFamilies; i++)
      //    delete myFamilies[i];
      //}
    }
  }

  { // FACE RELATED BLOCK
    medEntityMesh entity=MED_EN::MED_FACE;
    // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArray____Family DOIT ETRE ENLEVER DE LA CLASSE MESH
    if  ( ( _ptrMesh->existConnectivity(MED_NODAL,entity) )|( _ptrMesh->existConnectivity(MED_DESCENDING,entity) ) ) { 

      int numberOfTypes           = _ptrMesh->getNumberOfTypes (entity) ;
      const medGeometryElement  * types = _ptrMesh->getTypes         (entity) ;
      SCRUTE_MED(numberOfTypes);
      
      int numberOfElements = _ptrMesh->getNumberOfElements(entity, MED_ALL_ELEMENTS) ;
      int * familyArray = new int[numberOfElements] ;
      for (int i=0;i<numberOfElements;i++)
        familyArray[i]=0;

      int numberOfFamilies = _ptrMesh->getNumberOfFamilies(entity) ;
      //vector<FAMILY*> myFamilies = _ptrMesh->getFamilies(entity) ;
      vector<FAMILY*> * myFamilies = &_ptrMesh->_familyFace ;
      //bool ToDestroy = false;
      if (0 == numberOfFamilies) {
        //ToDestroy = true ;
        vector<GROUP*> myGroups = _ptrMesh->getGroups(entity);
        int NumberOfGroups = myGroups.size() ;
        // build families from groups
        for (int i=0; i<NumberOfGroups; i++) {
          SCRUTE_MED( myGroups[i]->getName() );
          SUPPORT * mySupport = myGroups[i] ;
          FAMILY* myFamily = new FAMILY(*mySupport);
          myFamily->setIdentifier(-i-1000);
          myFamilies->push_back(myFamily);
        }
        numberOfFamilies=myFamilies->size() ;
      }
      for (int i=0;i<numberOfFamilies;i++) {
        int familyNumber = (*myFamilies)[i]->getIdentifier() ;
        int numberOfFamilyElements = (*myFamilies)[i]->getNumberOfElements(MED_ALL_ELEMENTS) ;
        if ((*myFamilies)[i]->isOnAllElements())
          for (int ii=0; ii<numberOfFamilyElements; ii++)
            familyArray[ii]=familyNumber;
        else {
          const int * myFamilyElements = (*myFamilies)[i]->getNumber(MED_ALL_ELEMENTS) ;
          for (int ii=0;ii<numberOfFamilyElements;ii++)
            familyArray[myFamilyElements[ii]-1]=familyNumber;
        }
      }

      const int * typeCount = _ptrMesh->getGlobalNumberingIndex(entity) ;
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        int lgth=numberOfElements;
        med_2_1::med_int *temp=new med_2_1::med_int[lgth];
        for(int i2=0;i2<lgth;i2++)
          temp[i2]=(med_2_1::med_int) (familyArray[i2]);
#endif
      for (int i=0; i<numberOfTypes; i++) {

        int typeNumberOfElements = typeCount[i+1] - typeCount[i] ;
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
                        temp+typeCount[i]-1, typeNumberOfElements,
                        med_2_1::MED_REMP ,
                        (med_2_1::med_entite_maillage) entity,
                        (med_2_1::med_geometrie_element) types[i]);
#else
        err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
                        familyArray+typeCount[i]-1, typeNumberOfElements,
                        med_2_1::MED_REMP ,
                        (med_2_1::med_entite_maillage) entity,
                        (med_2_1::med_geometrie_element) types[i]); 
#endif
        if ( err != MED_VALID) 
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write family for the |"<< _ptrMesh->getNumberOfElements(entity, types[i])
                                       << "| faces of geometric type |" << geoNames[types[i]] <<"|in mesh |"      
                                       << _ptrMesh->_name.c_str() << "|" ));   
      }
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      delete [] temp;
#endif
      delete[] familyArray ;
      //if (true == ToDestroy) {
      //  int NumberOfFamilies = myFamilies->size();
      //    for (int i=0; i<NumberOfFamilies; i++)
      //      delete myFamilies[i];
      //}
    }
  }

  { // EDGE RELATED BLOCK
    //medEntityMesh entity=MED_EN::MED_FACE;
    medEntityMesh entity=MED_EN::MED_EDGE;
    // SOLUTION TEMPORAIRE CAR _ptrMesh->_MEDArray____Family DOIT ETRE ENLEVER DE LA CLASSE MESH
    if  ( ( _ptrMesh->existConnectivity(MED_NODAL,entity) )|( _ptrMesh->existConnectivity(MED_DESCENDING,entity) ) ) { 

      int numberOfTypes           = _ptrMesh->getNumberOfTypes (entity) ;
      const medGeometryElement  * types = _ptrMesh->getTypes         (entity) ;
      
      int numberOfElements = _ptrMesh->getNumberOfElements(entity, MED_ALL_ELEMENTS) ;
      int * familyArray = new int[numberOfElements] ;
      for (int i=0;i<numberOfElements;i++)
        familyArray[i]=0;

      int numberOfFamilies = _ptrMesh->getNumberOfFamilies(entity) ;
      //vector<FAMILY*> myFamilies = _ptrMesh->getFamilies(entity) ;
      vector<FAMILY*> * myFamilies = &_ptrMesh->_familyEdge ;
      //bool ToDestroy = false;
      if (0 == numberOfFamilies) {
        //ToDestroy = true ;
        vector<GROUP*> myGroups = _ptrMesh->getGroups(entity);
        int NumberOfGroups = myGroups.size() ;
        // build families from groups
        for (int i=0; i<NumberOfGroups; i++) {
          SCRUTE_MED( myGroups[i]->getName() );
          SUPPORT * mySupport = myGroups[i] ;
          FAMILY* myFamily = new FAMILY(*mySupport);
          myFamily->setIdentifier(-i-2000);
          myFamilies->push_back(myFamily);
        }
        numberOfFamilies=myFamilies->size() ;
      }
      for (int i=0;i<numberOfFamilies;i++) {
        int familyNumber = (*myFamilies)[i]->getIdentifier() ;
        int numberOfFamilyElements = (*myFamilies)[i]->getNumberOfElements(MED_ALL_ELEMENTS) ;
        if ((*myFamilies)[i]->isOnAllElements())
          for (int ii=0; ii<numberOfFamilyElements; ii++)
            familyArray[ii]=familyNumber;
        else {
          const int * myFamilyElements = (*myFamilies)[i]->getNumber(MED_ALL_ELEMENTS) ;
          for (int ii=0;ii<numberOfFamilyElements;ii++)
            familyArray[myFamilyElements[ii]-1]=familyNumber;
        }
      }

      const int * typeCount = _ptrMesh->getGlobalNumberingIndex(entity) ;
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        int lgth=numberOfElements;
        med_2_1::med_int *temp=new med_2_1::med_int[lgth];
        for(int i2=0;i2<lgth;i2++)
          temp[i2]=(med_2_1::med_int) (familyArray[i2]);
#endif
      for (int i=0; i<numberOfTypes; i++) {

        int typeNumberOfElements = typeCount[i+1] - typeCount[i] ;
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
        err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
                        temp+typeCount[i]-1, typeNumberOfElements,
                        med_2_1::MED_REMP ,
                        (med_2_1::med_entite_maillage) entity,
                        (med_2_1::med_geometrie_element) types[i]);
#else
        err = MEDfamEcr(_medIdt, const_cast <char *> ( _meshName.c_str() ),
                        familyArray+typeCount[i]-1, typeNumberOfElements,
                        med_2_1::MED_REMP ,
                        (med_2_1::med_entite_maillage) entity,
                        (med_2_1::med_geometrie_element) types[i]); 
#endif
        if ( err != MED_VALID) 
          throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't write family for the |"<< _ptrMesh->getNumberOfElements(entity, types[i])
                                       << "| edges of geometric type |" << geoNames[types[i]] <<"|in mesh |"      
                                       << _ptrMesh->_name.c_str() << "|" ));   
      }
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      delete [] temp;
#endif
      delete[] familyArray ;
      //if (true == ToDestroy) {
      //  int NumberOfFamilies = myFamilies->size();
      //  for (int i=0; i<NumberOfFamilies; i++)
      //    delete myFamilies[i];
      //}
    }
  }
    
  END_OF_MED(LOC);
  return MED_VALID;
}

int MED_MESH_WRONLY_DRIVER21::writeFamilies(vector<FAMILY*> & families ) const {
  
  const char * LOC="int MED_MESH_WRONLY_DRIVER21::writeFamilies(vector<FAMILY*> families) const : ";
  BEGIN_OF_MED(LOC);

  med_2_1::med_err err;
  
  MESSAGE_MED(LOC<<" families.size() :"<<families.size());

  for (unsigned int i=0; i< families.size(); i++) {

    int      numberOfAttributes         = families[i]->getNumberOfAttributes ();
    string   attributesDescriptions (numberOfAttributes*MED_TAILLE_DESC,'\0');
    //string   attributesDescriptions     = "";

    // Recompose the attributes descriptions arg for MED
    for (int j=0; j < numberOfAttributes; j++) {
        
      string attributeDescription = families[i]->getAttributeDescription(j+1);
        
      if ( attributeDescription.size() > MED_TAILLE_DESC )
        throw MEDEXCEPTION( LOCALIZED(STRING(LOC) << "The size of the attribute description nÂ° |" << j+1 << "| of the family |" << families[i]->getName()
                                      << "| with identifier |" << families[i]->getIdentifier()  << "| is |" 
                                      <<  attributeDescription.size()  <<"| and is more than |" <<  MED_TAILLE_DESC << "|")) ;
        
      int length = min(MED_TAILLE_LNOM,(int)attributeDescription.size());
      attributesDescriptions.replace(j*MED_TAILLE_DESC,length, attributeDescription,0,length);
      //attributesDescriptions += attributeDescription;
    }
      

    int      numberOfGroups  = families[i]->getNumberOfGroups();
    string   groupsNames(numberOfGroups*MED_TAILLE_LNOM,'\0') ;
    // Recompose the groups names arg for MED
    for (int j=0; j < numberOfGroups; j++) {

      string groupName = families[i]->getGroupName(j+1);
       
      if ( groupName.size() > MED_TAILLE_LNOM )
        throw MEDEXCEPTION( LOCALIZED(STRING(LOC) << "The size of the group name  nÂ° |" << j+1 << "| of the family |" << families[i]->getName()
                                      << "| with identifier |" << families[i]->getIdentifier()  << "| is |" 
                                      <<  groupName.size()  <<"| and is more than |" << MED_TAILLE_LNOM << "|")) ;
        

      int length = min(MED_TAILLE_LNOM,(int)groupName.size());
      groupsNames.replace(j*MED_TAILLE_LNOM,length, groupName,0,length);
      
    }

    // test if the family already exists (HDF trick waiting a MED evolution to be replaced)
    string dataGroupFam = "/ENS_MAA/"+_meshName+"/FAS/"+families[i]->getName()+"/";  
    SCRUTE_MED("|"<<dataGroupFam<<"|");
    err =med_2_1::_MEDdatagroupOuvrir(_medIdt,const_cast <char *> (dataGroupFam.c_str()) ) ;
    if ( err < MED_VALID ) {
      SCRUTE_MED(err);
      if ( families[i]->getName().size() > MED_TAILLE_NOM )
        throw MEDEXCEPTION
          ( LOCALIZED(STRING(LOC) << "The size of the name of the family |" << i+1
                      << "| |" << families[i]->getName()
                      << "| with identifier |" << families[i]->getIdentifier()  << "| is |" 
                      <<  families[i]->getName().size()  <<"| and is more than |"
                      << MED_TAILLE_NOM << "|")) ;

      MESSAGE_MED(LOC<<"families[i]->getName().c_str() : "<<families[i]->getName().c_str());
      MESSAGE_MED(LOC<<"_meshName.c_str() : "<<_meshName.c_str());
      MESSAGE_MED(LOC<<"families[i]->getIdentifier() : "<<families[i]->getIdentifier());
      MESSAGE_MED(LOC<<"numberOfAttributes : "<<numberOfAttributes);
        
      //MESSAGE_MED(LOC<<"families[i]->getAttributesIdentifiers() : "<<families[i]->getAttributesIdentifiers()[0]);
      //MESSAGE_MED(LOC<<"families[i]->getAttributesValues() : "<<families[i]->getAttributesValues()[0]);
      MESSAGE_MED(LOC<<"attributesDescriptions.c_str() : "<<attributesDescriptions.c_str());
      MESSAGE_MED(LOC<<"numberOfGroups : "<<numberOfGroups);
      MESSAGE_MED(LOC<<"groupsNames.c_str() : "<<groupsNames.c_str());
#if defined(IRIX64) || defined(OSF1) || defined(VPP5000) || defined(PCLINUX64)
      int lgth=families[i]->getNumberOfAttributes();
      med_2_1::med_int *  AttributesIdentifier2 = new med_2_1::med_int[lgth] ;
      med_2_1::med_int *  AttributesValues2     = new med_2_1::med_int[lgth] ;
      for(med_2_1::med_int i2=0;i2<lgth;i2++)
        {
          AttributesIdentifier2[i2]=(med_2_1::med_int)(families[i]->getAttributesIdentifiers()[i2]);
          AttributesValues2[i2]=(med_2_1::med_int)(families[i]->getAttributesValues()[i2]);
        }
      err = med_2_1::MEDfamCr( _medIdt, 
                              const_cast <char *> ( _meshName.c_str() ),
                              const_cast <char *> ( families[i]->getName().c_str() ),
                              families[i]->getIdentifier(), 
                              AttributesIdentifier2,
                              AttributesValues2,
                              const_cast <char *> (attributesDescriptions.c_str()), 
                              numberOfAttributes,  
                              const_cast <char *> (groupsNames.c_str()), 
                              numberOfGroups);
      delete [] AttributesIdentifier2;
      delete [] AttributesValues2;
#else
      err = med_2_1::MEDfamCr( _medIdt, 
                              const_cast <char *> ( _meshName.c_str() ),
                              const_cast <char *> ( families[i]->getName().c_str() ),
                              families[i]->getIdentifier(), 
                              (med_2_1::med_int*) families[i]->getAttributesIdentifiers(),
                              (med_2_1::med_int*) families[i]->getAttributesValues(),
                              const_cast <char *> (attributesDescriptions.c_str()), 
                              numberOfAttributes,  
                              const_cast <char *> (groupsNames.c_str()), 
                              numberOfGroups);
#endif
      SCRUTE_MED(err);
      if ( err != MED_VALID) 
        throw MEDEXCEPTION(LOCALIZED(STRING(LOC) << "Can't create family |" << families[i]->getName()
                                     << "| with identifier |" << families[i]->getIdentifier()  << "| groups names |" 
                                     << groupsNames  <<"| and  attributes descriptions |" << attributesDescriptions << "|")) ;
    }
    else
      med_2_1::_MEDdatagroupFermer(_medIdt);


  }

  END_OF_MED(LOC);
    
  return MED_VALID;
}


// A FAIRE POUR LES NOEUDS ET LES ELEMENTS ret = MEDfamEcr(



/*--------------------- RDWR PART -------------------------------*/

MED_MESH_RDWR_DRIVER21::MED_MESH_RDWR_DRIVER21()
{
  this->GENDRIVER::_accessMode = MED_EN::RDWR;
}

MED_MESH_RDWR_DRIVER21::MED_MESH_RDWR_DRIVER21(const string & fileName,
                                               MESH * ptrMesh):
  MED_MESH_DRIVER(fileName,ptrMesh,RDWR),
  IMED_MESH_RDONLY_DRIVER(fileName,ptrMesh),
  IMED_MESH_WRONLY_DRIVER(fileName,ptrMesh),
  IMED_MESH_RDWR_DRIVER(fileName,ptrMesh),
  MED_MESH_DRIVER21(fileName,ptrMesh,RDWR),
  MED_MESH_RDONLY_DRIVER21(fileName,ptrMesh),
  MED_MESH_WRONLY_DRIVER21(fileName,ptrMesh)
{
  MESSAGE_MED("MED_MESH_RDWR_DRIVER21::MED_MESH_RDWR_DRIVER21(const string & fileName, MESH * ptrMesh) has been created");
}

MED_MESH_RDWR_DRIVER21::MED_MESH_RDWR_DRIVER21(const MED_MESH_RDWR_DRIVER21 & driver):
  MED_MESH_DRIVER(driver),
  IMED_MESH_RDONLY_DRIVER(driver),
  IMED_MESH_WRONLY_DRIVER(driver),
  IMED_MESH_RDWR_DRIVER(driver),
  MED_MESH_DRIVER21(driver),
  MED_MESH_RDONLY_DRIVER21(driver),
  MED_MESH_WRONLY_DRIVER21(driver)
{
}

MED_MESH_RDWR_DRIVER21::~MED_MESH_RDWR_DRIVER21() {
  //MESSAGE_MED("MED_MESH_RDWR_DRIVER21::MED_MESH_RDWR_DRIVER21(const string & fileName, MESH * ptrMesh) has been destroyed");
}

GENDRIVER * MED_MESH_RDWR_DRIVER21::copy(void) const
{
  return new MED_MESH_RDWR_DRIVER21(*this);
}

void MED_MESH_RDWR_DRIVER21::write(void) const
{
  MED_MESH_WRONLY_DRIVER21::write();
}
void MED_MESH_RDWR_DRIVER21::read (void)
{
  MED_MESH_RDONLY_DRIVER21::read();
}
