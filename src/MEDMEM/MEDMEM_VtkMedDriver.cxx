using namespace std;
#include "MEDMEM_VtkMedDriver.hxx"

#include "MEDMEM_Med.hxx"

VTK_MED_DRIVER::VTK_MED_DRIVER(): GENDRIVER(), 
                                  _ptrMed((MED * const)MED_NULL), 
                                  _vtkFile(MED_INVALID) 
{
  // What about _id in Gendriver ?
  // _driverType ???
}


VTK_MED_DRIVER::VTK_MED_DRIVER(const string & fileName,  MED * const ptrMed):
  GENDRIVER(fileName,MED_EN::MED_RDWR), _ptrMed(ptrMed), _vtkFile(MED_INVALID)
{
  // What about _id in Gendriver ?
  // _driverType ???
}

//REM :  As t'on besoin du champ _status :  _vtkFile <-> _status  ?  Oui


void VTK_MED_DRIVER::open() {

  const char * LOC ="VTK_MED_DRIVER::open() : ";
  BEGIN_OF(LOC);

  // REFLECHIR SUR CE TEST PAR RAPPORT A L'OUVERTURE/FERMETURE
//    if ( _vtkFile != MED_INVALID ) 
//      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
//                                       << "_vtkFile is already in use, please close the file |" 
//                                       << _fileName << "| before calling open()"
//                                       )
//                            );   
  
  if ( _status != MED_CLOSED ) 
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "_status is not closed, please close the file |"
                                     << _fileName << "| before calling open()"
                                     )
                          );
  
  if ( _fileName == "" )
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) 
                                     << "_fileName is |\"\"|, please set a correct fileName before calling open()"
                                     )
                          );
  _vtkFile.open(filename.c_str()) ; // ? if error ????
  _status = MED_OPENED ;
  
//    if (_vtkFile > 0) _status=MED_OPENED; 
//    else {
//      _status = MED_CLOSED;
//      _vtkFile = MED_INVALID;
//      throw MED_EXCEPTION (LOCALIZED( STRING(LOC) 
//                                      << "Can't open |"  << _fileName 
//                                      << "|, _vtkFile : " << _vtkFile
//                                      )
//                           );
//  }
  
  END_OF(LOC);
}


void VTK_MED_DRIVER::close() {

  const char * LOC = "MED_MED_DRIVER::close() : ";
  BEGIN_OF(LOC);
  
  if ( _status == MED_CLOSED)
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << ": the file |" 
				     << _fileName << "| is already closed"
				     )
			  );
   
  //    if ( _vtkFile == MED_INVALID ) 
//      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "_vtkFile invalid, but the file |" 
//                                       << _fileName << "| seems to be openned !"
//                                       )
//                            );   
  vtkFile_.close();
  
  _status = MED_CLOSED;
  //  _vtkFile = MED_INVALID;

//    if (err != MED_VALID) 
//      throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) << "the file |" 
//                                       << _fileName << "| couldn't be closed"
//                                       )
//                            );   
  
  END_OF(LOC);
}


void VTK_MED_DRIVER::write() {

  const char * LOC = "MED_MED_DRIVER::write() : ";
  BEGIN_OF(LOC);

  // could we put more than one Mesh ?????
  _vtkFile << "# vtk DataFile Version 2.0" << endl 
	   << "maillage SALOLME"  << endl ;
  // only ASCII for the moment (binary came latest :-)
  _vtkFile << "ASCII" << endl ;

  int NumberOfMeshes = _ptrMED->getNumberOfMeshes() ;
  string * MeshName = new string[NumberOfMeshes] ;
  _ptrMED->getMeshNames(MeshName) ;
  // In fact, we must take care of all supports 
  // We restrict Field on all nodes or cells
  for (int i=0; i<NumberOfMeshes; i++) {
    MESH * myMesh = _ptrMED->getMesh(MeshName[i]) ;
    writeMesh(myMesh) ;
    // get all field which values are on this mesh => revoir api de Med !!!
    _vtkFile << "NODE" << endl ;
    // first : field on node
    for (int j=0; j<NumberOfFields; j++) {
      FIELD_ * myField = _ptrMED->getField() ;
      if (myField->getSupport()->getEntity()!=MED_NODE)
	if (myField->getSupport()->isOnAllElements())
	  writeField(myField) ;
      
    }
    _vtkFile << "CELL" << endl ;
    // second : field on cell
    for (int j=0; j<NumberOfFields; j++) {
      FIELD_ * myField = _ptrMED->getField() ;
      if (myField->getSupport()->getEntity()!=MED_CELL)
	if (myField->getSupport()->isOnAllElements())
	  writeField(myField) ;
      
    }
    
  }

  END_OF(LOC);
}

void VTK_MED_DRIVER::writeMesh(MESH * myMesh) {

  const char * LOC = "MED_MED_DRIVER::writeMesh() : ";
  BEGIN_OF(LOC);

  _vtkFile << "DATASET UNSTRUCTURED_GRID" << endl ;
  // put points (all point are in 3D, so if we are in 1D or 2D, we complete by zero !
  int SpaceDimension = myMesh->getSpaceDimension() ;
  int NumberOfNodes = myMesh->getNumberOfNodes() ;
  _vtkFile << "POINTS " << NumberOfNodes << " float" << endl ;
  double *coordinate = myMesh->getCoordinates(MED_FULL_ENTERLACE) ;
  if (SpaceDimension<3) { // 1D or 2D
    coordinate_z = new double[NumberOfNodes] ;
    // we put zero :
    for (int i=0;i<NumberOfNodes;i++)
      coordinate_z[i] = 0.0 ;
    if (SpaceDimension==1) 
      coordinate_y = coordinate_z ; // only one array of zero !
    else
      coordinate_y = coordinate_x + NumberOfNodes ;
  } else {
    coordinate_y = coordinate_x + NumberOfNodes ;
    coordinate_z = coordinate_y + NumberOfNodes ;
  }
  for (int i=0;i<NumberOfNodes;i++) 
    for (int j=0;j<SpaceDimension;j++)
      _vtkFile << coordinate[i*SpaceDimension+j] << " " ;
  if (SpaceDimension==1) 
    _vtkFile << "0 0" ;
  if (SpaceDimension==2) 
    _vtkFile << "0" ;
  _vtkFile << endl ;

  // we put connectivity
  // how many cells and how many value in connectivity :
  int cells_types_count = myMesh->getNumberOfTypes(MED_CELL) ;
  int * cells_count = myMesh->get_cells_count() ;
  int cells_sum = cells_count[cells_types_count] ;
  CellModel * cells_type = myMesh->get_cells_type() ; 
  int connectivity_sum = 0 ;

  int * connectivity = myMesh->getConnectivity(MED_FULL_ENTERLACE,MED_CELL,MED_ALL_ELEMENTS) ;
  int * connectivityIndex = myMesh->getConnectivityIndex(MED_CELL) ;

  for (int i=0;i<cells_types_count;i++) {
    int nodes_cell = cells_type[i].getNumberOfNodes();
    connectivity_sum+= (cells_count[i+1]-cells_count[i])*(nodes_cell + 1);
    // we add 1 because we put nodes count in vtk file !
  }
  _vtkFile << "CELLS " << cells_sum << " " << connectivity_sum << endl ;
  // we put connectivity
  for (int i=0;i<cells_types_count;i++) {
    int *filter = (int*) NULL ; // index in vtk connectivity
    switch (cells_type[i].get_type())
      {
      case MED_POINT1  : {
	filter = new int[1] ;
	filter[0] = 0 ;
        break ;
      }
      case MED_SEG2    : {
        filter = new int[2] ;
	filter[0] = 0 ;
        filter[1] = 1 ;
        break ;
      }
      case MED_SEG3    : {  
        break ;
      }
      case MED_TRIA3   : {
        filter = new int[3] ;
        filter[0] = 0 ;
        filter[1] = 1 ;
 	filter[2] = 2 ;
        break ;
      }
      case MED_QUAD4   : {
        filter = new int[4] ;
        filter[0] = 0 ;
        filter[1] = 1 ;
        filter[2] = 2 ;
  	filter[3] = 3 ;
        break ;
      }
      case MED_TRIA6   : {
        break ;
      }
      case MED_QUAD8   : {
        break ;
      }
      case MED_TETRA4  : {
        filter = new int[4] ;
        filter[0] = 0 ;
        filter[1] = 1 ;
        filter[2] = 3 ;  // 3td element in med are 4th in vtk (array begin at 0 !)
        filter[3] = 2 ;  // 4th element in med are 3rd in vtk (array begin at 0 !)
        break ;
      }
      case MED_PYRA5   : {
        filter = new int[5] ;
        filter[0] = 0 ;
        filter[1] = 3 ;  // 2nd element in med are 4th in vtk (array begin at 0 !)
        filter[2] = 2 ;
        filter[3] = 1 ;  // 4th element in med are 2nd in vtk (array begin at 0 !)
  	filter[4] = 4 ;
        break ;
      }
      case MED_PENTA6  : {
        filter = new int[6] ;
        filter[0] = 0 ;
        filter[1] = 1 ;
        filter[2] = 2 ;
        filter[3] = 3 ;
        filter[4] = 4 ;
  	filter[5] = 5 ;
	break ;
      }
      case MED_HEXA8   : {
        filter = new int[8] ;
        filter[0] = 0 ;
        filter[1] = 3 ;
        filter[2] = 2 ;
        filter[3] = 1 ;
        filter[4] = 4 ;
        filter[5] = 7 ;
  	filter[6] = 6 ;
	filter[7] = 5 ;
        break ;
      }
      case MED_TETRA10 : {
        break ;
      }
      case MED_PYRA13  : {
        break ;
      }
      case MED_PENTA15 : {
        break ;
      }
      case MED_HEXA20  : {
        break ;
      }
      default : { 
        break ;
      }
      }
    if (filter==NULL) 
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": MED element type not supported yet : " << cells_type[i].get_type() ) ) ;
    int nodes_cell = cells_type[i].get_NumberOfNodes();
    for (int j=0;j<cells_count[i+1]-cells_count[i];j++) {
      _vtkFile << nodes_cell << " " ;
      for (int k=0;k<nodes_cell;k++)
	_vtkFile << (myMesh->get_nodal_connectivity(i+1))[j*nodes_cell+filter[k]] - 1 << " " ;
      _vtkFile << endl ;
    }
  }
  _vtkFile << endl ;
  // we put cells type
  _vtkFile << "CELL_TYPES " << cells_sum << endl ;
  for (int i=0;i<cells_types_count;i++) {
    int vtkType = 0 ;
    switch (cells_type[i].get_type())
      {
      case MED_POINT1  : {
	vtkType = 1 ;
        break ;
      }
      case MED_SEG2    : {
	vtkType = 3 ;
        break ;
      }
      case MED_SEG3    : {  
	vtkType = 0 ;
        break ;
      }
      case MED_TRIA3   : {
	vtkType = 5 ;
        break ;
      }
      case MED_QUAD4   : {
	vtkType = 9 ;
        break ;
      }
      case MED_TRIA6   : {
	vtkType = 0 ;
        break ;
      }
      case MED_QUAD8   : {
	vtkType = 0 ;
        break ;
      }
      case MED_TETRA4  : {
	vtkType = 10 ;
        break ;
      }
      case MED_PYRA5   : {
	vtkType = 14 ;
        break ;
      }
      case MED_PENTA6  : {
	vtkType = 13 ;
	break ;
      }
      case MED_HEXA8   : {
	vtkType = 12 ;
        break ;
      }
      case MED_TETRA10 : {
	vtkType = 0 ;
        break ;
      }
      case MED_PYRA13  : {
	vtkType = 0 ;
        break ;
      }
      case MED_PENTA15 : {
	vtkType = 0 ;
        break ;
      }
      case MED_HEXA20  : {
	vtkType = 0 ;
        break ;
      }
      default : { 
	vtkType = 0 ;
        break ;
      }
      }
    if (vtkType == 0)
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<": MED element type not supported yet : " << cells_type[i].get_type() ) ) ;
    for (int j=0;j<cells_count[i+1]-cells_count[i];j++)
      _vtkFile << vtkType << endl ;
    if (filter != NULL)
      delete[] filter ;
  }

  // add a constant field on all node to test !
  //    _vtkFile << "POINT_DATA " << NumberOfNodes << endl ;
  //    _vtkFile << "SCALARS example_scalaire float 1" << endl ;
  //    _vtkFile << "LOOKUP_TABLE default" << endl ;
  //    for (int i=0;i<NumberOfNodes;i++)
  //      _vtkFile << i << endl ;
  
  return 1 ;


  END_OF(LOC);
}

void VTK_MED_DRIVER::writeField(FIELD * myField) {

  const char * LOC = "MED_MED_DRIVER::writeField() : ";
  BEGIN_OF(LOC);

  

  END_OF(LOC);
}
