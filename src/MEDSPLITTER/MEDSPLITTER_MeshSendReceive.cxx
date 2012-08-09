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
// File      : MEDSPLITTER_MeshSendReceive.cxx
// Created   : Tue Jun 30 20:39:33 2009
// Author    : Edward AGAPOV (eap)


#include "MEDSPLITTER_MeshSendReceive.hxx"

#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_Group.hxx"

using namespace MEDSPLITTER;



#ifdef HAVE_MPI2

//=======================================================================
//function : MeshSendReceive
//purpose  : 
//=======================================================================

MeshSendReceive::MeshSendReceive():_mesh(0)
{
  _int_request = _coord_request = _char_request = MPI_REQUEST_NULL;
  _node_nums_request = _cell_nums_request = _face_nums_request = MPI_REQUEST_NULL;
}

//================================================================================
/*!
 * \brief Do not die until everything is sent
 */
//================================================================================

MeshSendReceive::~MeshSendReceive()
{
  clear();
}

//================================================================================
/*!
 * \brief Wait for ending send() before freeing buffers
 */
//================================================================================

void MeshSendReceive::clear()
{
  MPI_Request* request[6] = { &_int_request, &_coord_request, &_char_request,
                              &_node_nums_request, &_cell_nums_request, &_face_nums_request };

  for ( int i = 0; i < 6; ++i )
  {
    if ( *request[i] != MPI_REQUEST_NULL ) {
      MPI_Status status;
      MPI_Wait( request[i], &status);
    }
    switch ( i ) {
    case 0: _int_buf.clear(); break;
    case 1: /*_coord_buf.clear()*/; break;
    case 2: _char_buf.clear(); break;
    case 3: _node_glob_numbers.clear(); break;
    case 4: _cell_glob_numbers.clear(); break;
    case 5: _face_glob_numbers.clear(); break;
    }
  }
}

//================================================================================
/*!
 * \brief Return true if mesh is already sent
 */
//================================================================================

bool MeshSendReceive::isSent()
{
  MPI_Request* request[6] = { &_int_request, &_coord_request, &_char_request,
                              &_node_nums_request, &_cell_nums_request, &_face_nums_request };

  for ( int i = 0; i < 6; ++i )
    if ( *request[i] != MPI_REQUEST_NULL )
    {
      int complete;
      MPI_Status status;
      MPI_Request_get_status(*request[i], &complete, &status);
      if ( !complete )
        return false;
    }
  return true;
}

//================================================================================
namespace { // local utils
//================================================================================

  using namespace MED_EN;
  /*!
   * \brief Pointer to data and size of data
   */
  template< typename T > struct TSlice
  {
    typedef MEDMEM::PointerOf<T> TPtr;
    TPtr _pointer;
    int  _size;

    TSlice(): _size(0) {}
    TSlice& operator=(const TSlice& s){_size = s._size; _pointer.set(_size,s._pointer); return *this;}
    TSlice(const TSlice& other): _size(other._size),_pointer(other._size, other._pointer){} // copy
    void resize(int nb) { _size=nb; _pointer.set(nb); }
    void setPtr(int nb, const T* ptr) { _size=nb; _pointer.set(ptr); } // shallowcopy
    void copy  (int nb, const T* ptr) { _size=nb; _pointer.set(nb,ptr); } // deep copy
    void clear() { _size=0; _pointer.set(0); }
    T& operator[](const int i) { return _pointer[i]; }
    string dumpPtr() const; // debug
  };
  template<> string TSlice<char>::dumpPtr() const
  {
    return _size > 0 ? string(_pointer) : string("");
  }
  template<typename T> string TSlice<T>::dumpPtr() const
  {
    MEDMEM::STRING res("");
    for ( int i = 0; i < _size && res.size() < 200; ++i )
      res << _pointer[i] << " ";
    return res;
  }
  typedef TSlice<int>    TIntSlice;
  typedef TSlice<char>   TCharSlice;
  typedef TSlice<double> TDoubleSlice;

  //================================================================================
  /*!
   * \brief Make TCharSlice from string 
   */
  //================================================================================

  TCharSlice toCharSlice( const string& str )
  {
    TCharSlice slice;
    if ( !str.empty() )
      slice.setPtr( str.size() + 1, str.c_str());
    return slice;
  }

  //================================================================================
  /*!
   * \brief Make string from TCharSlice
   */
  //================================================================================

  string fromCharSlice( TCharSlice& slice  )
  {
    return slice._size < 1 ? string() : string( slice._pointer );
  }

#define DEB_OUT(txt) \
//cout << txt << endl;
  //================================================================================
  /*!
   * \brief Put list of slices into one array
   *  \param data - slices of data
   *  \param buffer - output array
   */
  //================================================================================
  template< typename T >
  void serialize( vector< TSlice< T > >& data, vector< T >& buffer)
  {
    // mesure data
    int shift = max( 1, int(sizeof(int)/sizeof(T))); // nb of T to store int
    int size = shift; // to store nb of slices in data
    typename vector< TSlice< T > >::iterator slice = data.begin();
    for ( ; slice != data.end(); ++slice )
      size += slice->_size + shift; // +shift - to store each slice size in the buffer

    // fill in the buffer

    buffer.resize( size );
    T* ptr = & buffer[0];
    // store nb of slices as the first number
    *((int*) ptr) = (int) data.size();
    ptr += shift;

    DEB_OUT( "\n *** Nb slices: " << data.size());
    for ( slice = data.begin(); slice != data.end(); ++slice )
    {
      *((int*) ptr) = (int) slice->_size;
      ptr += shift;
      if ( slice->_size )
        memcpy( ptr, slice->_pointer, slice->_size * sizeof(T));
      ptr += slice->_size;
      DEB_OUT("#"<< slice - data.begin() << ": \tsize " << slice->_size << " \t" << slice->dumpPtr());
      slice->clear();
    }
    data.clear();
  }
  //================================================================================
  /*!
   * \brief Retrieve data slices from array
   *  \param data - output slices of data
   *  \param buffer - data array
   *  \retval int - zero or slice index (counting from 1) where deserialization failed
   */
  //================================================================================

  template< typename T >
  int deserialize( vector< T >& buffer, vector< TSlice< T > >& data )
  {
    T* ptr = & buffer[0];
    T* end = ptr + buffer.size();

    int nb_slices = *((int*) ptr);
    data.resize( nb_slices );
    int shift = max( 1, int(sizeof(int)/sizeof(T))); // nb of T to store int
    ptr += shift;

    DEB_OUT( "Nb slices: " << data.size());
    for ( int i = 0; i < nb_slices; ++i )
    {
      data[i]._size = *((int*) ptr);
      ptr += shift;
      data[i]._pointer.set( ptr );
      DEB_OUT("#"<< i << ": \tsize " << data[i]._size << " \t" << data[i].dumpPtr());
      if ( data[i]._size < 0 || data[i]._size > end - ptr )
        return i + 1; // error
      ptr += data[i]._size;
    }
    DEB_OUT(" *** END" << endl )
    return 0;
  }

  //================================================================================
  /*!
   * \brief Return all groups and families of mesh
   */
  //================================================================================

  void getGroupsAndFamilies( MEDMEM::MESH*              mesh,
                             vector< MEDMEM::GROUP*>&   groups,
                             vector< MEDMEM::FAMILY*> & families)
  {
    medEntityMesh face_entity = mesh->getMeshDimension()==3 ? MED_FACE : MED_EDGE;

    // get all groups
    groups = mesh->getGroups( MED_CELL );
    vector< MEDMEM::GROUP* > face_groups = mesh->getGroups( face_entity );
    vector< MEDMEM::GROUP* > node_groups = mesh->getGroups( MED_NODE );
    groups.insert( groups.end(), face_groups.begin(), face_groups.end());
    groups.insert( groups.end(), node_groups.begin(), node_groups.end());

    // get all families
    families = mesh->getFamilies( MED_CELL );
    vector< MEDMEM::FAMILY* > face_families = mesh->getFamilies( face_entity );
    vector< MEDMEM::FAMILY* > node_families = mesh->getFamilies( MED_NODE );
    families.insert( families.end(), face_families.begin(), face_families.end() );
    families.insert( families.end(), node_families.begin(), node_families.end() );
  }

  enum SliceEnum // indices of fixed slices and sub-indices within slices
    {
      // INT slices
      SLC_GENERAL_INFO = 0,
      SLC_GEOM_TYPES,
      SLC_FAMILY_GEN_DATA,
      SLC_FAMILY_GROUP_IDS,
      SLC_POLYGON_INDEX,
      SLC_POLYHED_INDEX,
      SLC_FIRST_STD_CONN, // index of the first of non-fixed int slices:
      // - connectivities of std types and
      // - elements of families

      // sub-indices of SLC_GENERAL_INFO:
      I_SPACEDIM = 0, I_MESHDIM,
      I_NB_CELL_TYPES,  I_NB_FACE_TYPES,  I_NB_NODES, I_NB_GROUPS,
      I_NB_CELL_FAMILY, I_NB_FACE_FAMILY, I_NB_NODES_FAMILY,
      I_HAD_FAMILIES,
      I_MIN_SIZE, // size of SLC_GENERAL_INFO

      // sub-indices repeated within SLC_FAMILY_DATA for each group:
      I_FAM_ID = 0, I_NB_FAM_TYPES, I_NB_FAM_GROUPS,
      I_FAM_DATA_SIZE,

      // CHAR slices
      SLC_MESH_NAME = 0,
      SLC_MESH_DESC,
      SLC_COORD_SYS,
      SLC_FIRST_COORD_NAME // index of the first of non-fixed char slices:
      // - coord names
      // - coord unites
      // - family names
      // - group names
    };
  enum TagEnum
    {
      // MPI message tags
      TAG_INT = 1032, TAG_COORD = 2064, TAG_CHAR = 3008,
      TAG_NODE_NUMS = 4032, TAG_CELL_NUMS = 5032, TAG_FACE_NUMS = 6032,

      // miscellaneous
      IS_ON_ALL_FLAG = -1
    };

  //================================================================================
  /*!
   * \brief Enrich MEDMEM::MESHING with addFamily() etc
   */
  //================================================================================

  struct TMESHING : public MEDMEM::MESHING
  {
    MEDMEM::FAMILY* addFamily(MEDMEM::FAMILY* family, const medEntityMesh& entity)
    {
      switch ( entity ) {
      case MED_CELL: _familyCell.push_back(family); break;
      case MED_FACE: _familyFace.push_back(family); break;
      case MED_EDGE: _familyEdge.push_back(family); break;
      case MED_NODE: _familyNode.push_back(family); break;
      default :
        throw MED_EXCEPTION(LOCALIZED("Bad Entity !"));
      }
      family->setMesh( this );
      family->setEntity( entity );
      return family;
    }
    //================================================================================
    /*!
     * \brief Remove all families that prevent correct writting groups that not pass through
     * send/recieve as no families exist for them
     */
    //================================================================================

    void clearFamilies()
    {
      vector< MEDMEM::GROUP*> groups;
      vector< MEDMEM::FAMILY*> families;
      getGroupsAndFamilies(this, groups, families);

      for ( int i = 0; i < families.size(); ++i )
      {
        addReference();
        families[i]->removeReference();
      }
      _familyCell.clear();
      _familyFace.clear();
      _familyEdge.clear();
      _familyNode.clear();

      families.clear();
      for ( int i = 0; i < groups.size(); ++i )
        groups[i]->setNumberOfFamilies(0), groups[i]->setFamilies(families);
    }

    //================================================================================
    /*!
     * \brief Sets type of coordinate system
     */
    //================================================================================

    void setCoordinatesSystem(const std::string& system)
    {
      _coordinate->setCoordinatesSystem(system);
    }

    //================================================================================
    /*!
     * \brief Sets space dimension
     */
    //================================================================================

    void setSpaceDimension( int dim )
    {
      _spaceDimension = dim;
    }

    //================================================================================
    /*!
     * \brief Sets the number of nodes in the mesh
     */
    //================================================================================

    void setNumberOfNodes(const int NumberOfNodes)
    {
      _numberOfNodes = NumberOfNodes ;
    }
  };

  //================================================================================
  /*!
   * \brief Set string data to mesh
   */
  //================================================================================

  void setNames( vector< TCharSlice> & char_data, MEDMEM::MESH* mesh )
  {
    if ( char_data.size() < SLC_FIRST_COORD_NAME + 2 * mesh->getSpaceDimension() )
      throw MED_EXCEPTION(LOCALIZED("Internal error in MeshSendReceive::recv()"));

    TMESHING* meshing = (TMESHING*) mesh;

    meshing->setName             ( fromCharSlice( char_data[ SLC_MESH_NAME ]));
    meshing->setDescription      ( fromCharSlice( char_data[ SLC_MESH_DESC ]));
    meshing->setCoordinatesSystem( fromCharSlice( char_data[ SLC_COORD_SYS ]));

    int char_slice = SLC_FIRST_COORD_NAME;
    for ( int ax = 0; ax < meshing->getSpaceDimension(); ++ax )
    {
      meshing->setCoordinateName( fromCharSlice( char_data[ char_slice++ ]), ax );
      meshing->setCoordinateUnit( fromCharSlice( char_data[ char_slice++ ]), ax );
    }

    // names of groups and families

    vector< MEDMEM::GROUP* > groups;
    vector< MEDMEM::FAMILY*> families;
    getGroupsAndFamilies( mesh, groups, families );

    int group_slice_0 = char_slice + families.size();
    for ( int f = 0; char_slice < char_data.size() && f < families.size(); ++char_slice, ++f )
    {
      families[f]->setName ( fromCharSlice( char_data[ char_slice ]));
      // group names in families
      const string* group_ids = families[f]->getGroupsNames();
      vector< string > group_names( families[f]->getNumberOfGroups() );
      for ( int i = 0; i < families[f]->getNumberOfGroups(); ++i )
      {
        int group_id = atoi( group_ids[i].c_str() );
        group_names[i] = fromCharSlice( char_data[ group_slice_0 + group_id ]);
      }
      families[f]->setGroupsNames( & group_names[0] );
    }
    // group names
    for ( int g = 0; char_slice < char_data.size() && g < groups.size(); ++char_slice, ++g )
      groups[g]->setName( fromCharSlice( char_data[ char_slice ]));
  }


  //================================================================================
  /*!
   * \brief Creates families and groups in the mesh
   */
  //================================================================================

  void makeGroupsAndFamilies ( TMESHING* mesh, vector< TIntSlice >& int_data, int first_elem_slice )
  {
    int nb_cell_fams = int_data [ SLC_GENERAL_INFO ][ I_NB_CELL_FAMILY  ];
    int nb_face_fams = int_data [ SLC_GENERAL_INFO ][ I_NB_FACE_FAMILY  ];
    int nb_node_fams = int_data [ SLC_GENERAL_INFO ][ I_NB_NODES_FAMILY ];

    medEntityMesh face_entity = mesh->getMeshDimension()==3 ? MED_FACE : MED_EDGE;

    // make families
    vector< MEDMEM::FAMILY*> families;
    for ( int g = 0; g < nb_cell_fams; ++g )
      families.push_back( mesh->addFamily( new MEDMEM::FAMILY, MED_CELL ));
    for ( int g = 0; g < nb_face_fams; ++g ) 
      families.push_back( mesh->addFamily( new MEDMEM::FAMILY, face_entity ));
    for ( int g = 0; g < nb_node_fams; ++g )
      families.push_back( mesh->addFamily( new MEDMEM::FAMILY, MED_NODE ));

    vector< list< MEDMEM::FAMILY* > > fams_of_group( int_data[ SLC_GENERAL_INFO ][ I_NB_GROUPS ]);

    const int* fam_data   = int_data [ SLC_FAMILY_GEN_DATA  ]._pointer;
    const int* fam_groups = int_data [ SLC_FAMILY_GROUP_IDS ]._pointer;
    int i_group = 0, fam_elem_slice = first_elem_slice;

    // set family data
    for ( int f = 0; f < families.size(); ++f, fam_data += I_FAM_DATA_SIZE )
    {
      families[f]->setIdentifier( fam_data[ I_FAM_ID ] );
      // set types and elements
      int nb_geom_types = fam_data[ I_NB_FAM_TYPES ];
      families[f]->setNumberOfGeometricType( nb_geom_types );
      if ( nb_geom_types < 1 || int_data[ fam_elem_slice ]._size < 1 )
      {
      }
      else if ( int_data[ fam_elem_slice ][0] == IS_ON_ALL_FLAG )
      {
        families[f]->setAll(true);
        families[f]->update();
      }
      else
      {
        vector< medGeometryElement > geom_types( nb_geom_types );
        vector< int >               nb_elements( nb_geom_types );
        vector< int >                     index( nb_geom_types + 1);
        index[0] = 1;
        for ( int t = 0; t < nb_geom_types; ++t )
        {
          if ( families[f]->getEntity() != MED_NODE )
            geom_types[t] = mesh->getElementType( families[f]->getEntity(),
                                                  int_data[ fam_elem_slice + t ][0] );
          else
            geom_types[t] = MED_NONE;
          nb_elements[t] = int_data[ fam_elem_slice ]._size;
          index[t+1] = index[t] + nb_elements[t];
        }
        families[f]->setGeometricType( &geom_types[0] );
        families[f]->setNumberOfElements( &nb_elements[0] );
        families[f]->setNumber(new MEDMEM::MEDSKYLINEARRAY(nb_geom_types, index[nb_geom_types]-1));
        families[f]->getnumber()->setIndex( & index[0] );
        for ( int t = 0; t < nb_geom_types; ++t )
          families[f]->getnumber()->setI( t+1, int_data[ fam_elem_slice + t ]._pointer );
      }
      fam_elem_slice += nb_geom_types;

      // set groups of a family
      vector< string > group_names( fam_data[ I_NB_FAM_GROUPS] );
      for ( int g = 0; g < group_names.size(); ++g )
      {
        int group_id = fam_groups[ i_group++ ];
        fams_of_group[ group_id ].push_back( families[f] );
        // store group id as string until real names are read
        group_names[g] = MEDMEM::STRING( group_id );
      }
      families[f]->setNumberOfGroups( group_names.size() );
      families[f]->setGroupsNames( &group_names[0] );
    }
    // create groups

    for ( int g = 0; g < fams_of_group.size(); ++g )
    {
      MEDMEM::GROUP* group = new MEDMEM::GROUP( "", fams_of_group[ g ]);
      mesh->addGroup( *group );
      group->removeReference();
    }
  }

  //================================================================================
  /*!
   * \brief Restore mesh from received data
   */
  //================================================================================

  MEDMEM::MESH* makeMesh( vector< TIntSlice >& int_data, bool& toClearFamilies )
  {
    if ( int_data.empty() || int_data[ SLC_GENERAL_INFO ]._size < I_MIN_SIZE ||
         int_data[ SLC_GENERAL_INFO ][ I_NB_NODES ] < 1 )
      return 0;

    TMESHING* mesh = new TMESHING();
    // general info
    mesh->setSpaceDimension( int_data[ SLC_GENERAL_INFO ][ I_SPACEDIM ]);
    mesh->setNumberOfNodes ( int_data[ SLC_GENERAL_INFO ][ I_NB_NODES ]);
    int meshDimension = int_data[ SLC_GENERAL_INFO ][ I_MESHDIM ];

    // nb of types
    medEntityMesh face_entity = meshDimension==3 ? MED_FACE : MED_EDGE;
    int nb_cell_types = int_data[ SLC_GENERAL_INFO ][ I_NB_CELL_TYPES ];
    int nb_face_types = int_data[ SLC_GENERAL_INFO ][ I_NB_FACE_TYPES ];
    mesh->setNumberOfTypes ( nb_cell_types, MED_CELL );
    mesh->setNumberOfTypes ( nb_face_types, face_entity );

    // types and nb of elems
    vector< medGeometryElement > types( nb_cell_types + nb_face_types );
    vector< int >                nbElementsByType( types.size() ) ;
    int t = 0, conn_slice = SLC_FIRST_STD_CONN;
    for ( ; t < types.size(); ++t )
    {
      types[t] = medGeometryElement( int_data[ SLC_GEOM_TYPES ][t] );
      if ( types[t] == MED_POLYGON )
      {
        nbElementsByType[t] = int_data[ SLC_POLYGON_INDEX ]._size - 1;
      }
      else if ( types[t] == MED_POLYHEDRA )
      {
        nbElementsByType[t] = int_data[ SLC_POLYHED_INDEX ]._size - 1;
      }
      else
      {
        int nn = types[t] % 100;
        nbElementsByType[t] = int_data[ conn_slice++ ]._size / nn;
      }
    }
    mesh->setTypes( &types[0], MED_CELL );
    mesh->setTypes( &types[0] + nb_cell_types, face_entity );
    mesh->setNumberOfElements( &nbElementsByType[0], MED_CELL );
    mesh->setNumberOfElements( &nbElementsByType[0] + nb_cell_types, face_entity );

    // connectivity
    const int * index = 0;
    for ( t = 0, conn_slice = SLC_FIRST_STD_CONN; t < types.size(); ++t )
    {
      if ( types[t] == MED_POLYGON )
      {
        index = int_data[ SLC_POLYGON_INDEX ]._pointer;
      }
      else if ( types[t] == MED_POLYHEDRA )
      {
        index = int_data[ SLC_POLYHED_INDEX ]._pointer;
      }

      mesh->setConnectivity( t < nb_cell_types ? MED_CELL : face_entity,
                             types[ t ],
                             int_data[ conn_slice++ ]._pointer,
                             index);
    }

    // make families and groups

    makeGroupsAndFamilies( mesh, int_data, SLC_FIRST_STD_CONN + types.size() );

    toClearFamilies = ! int_data[ SLC_GENERAL_INFO ][ I_HAD_FAMILIES ];

    return mesh;
  }

  //================================================================================
  /*!
   * \brief Retrieve data of groups and families
   *  \retval int - number of geom types in all families
   */
  //================================================================================

  int getFamilyData( MEDMEM::MESH*             mesh,
                     vector<MEDMEM::FAMILY*> & families,
                     vector<MEDMEM::GROUP *> & groups,
                     vector<int> &             family_gen_data,
                     vector<int> &             family_group_ids )
  {
    // get all groups and families of mesh
    getGroupsAndFamilies( mesh, groups, families );
    if ( families.empty() ) return 0;

    // assign ids to groups
    map < string, int > group_ids;
    map < string, int >::iterator name_id;
    for ( int id = 0; id < groups.size(); ++id )
      if ( !group_ids.insert( make_pair( groups[id]->getName(), id)).second )
        throw MED_EXCEPTION(LOCALIZED("Equal group names"));


    family_gen_data.resize( families.size() * I_FAM_DATA_SIZE );
    family_group_ids.clear();
    int nb_fam_types = 0, *fam_data = &family_gen_data[0];

    for ( int f = 0; f < families.size(); ++f, fam_data += I_FAM_DATA_SIZE )
    {
      // sum up number of geom types of families
      int nb_types =
        families[f]->getEntity() == MED_NODE || families[f]->isOnAllElements() ?
        1 :
        families[f]->getNumberOfTypes();
      nb_fam_types += nb_types;

      // fill in family_group_ids
      for ( int g = 0; g < families[f]->getNumberOfGroups(); ++g )
      {
        name_id = group_ids.find( families[f]->getGroupName( g+1 ));
        if ( name_id == group_ids.end() )
          throw  MED_EXCEPTION (LOCALIZED(MEDMEM::STRING("Non-exiting group name:>")<<
                                          families[f]->getGroupName( g )<<"<"));
        family_group_ids.push_back( name_id->second );
      }
      // fill in family_gen_data
      fam_data[ I_FAM_ID ]        = families[f]->getIdentifier();
      fam_data[ I_NB_FAM_TYPES ]  = nb_types;
      fam_data[ I_NB_FAM_GROUPS ] = families[f]->getNumberOfGroups();
    }
    return nb_fam_types;
  }

  //================================================================================
  /*!
   * \brief Store mesh as arrayrs of ints, doubles and chars
   */
  //================================================================================

  bool getMeshData(MEDMEM::MESH*         mesh,
                   vector< TIntSlice >&  int_data,
                   vector< TCharSlice >& char_data,
                   TDoubleSlice&         coords)
  {
    if ( !mesh || mesh->getNumberOfNodes() < 1 )
      return false;
    medEntityMesh face_entity = mesh->getMeshDimension()==3 ? MED_FACE : MED_EDGE;
    medConnectivity conn = MED_NODAL;
    bool have_faces = mesh->existConnectivity( conn, face_entity );
    bool have_families = ( mesh->getNumberOfFamilies( MED_CELL ) +
                           mesh->getNumberOfFamilies( face_entity ) +
                           mesh->getNumberOfFamilies( MED_NODE ));

    // create missing families
    if ( !have_families && ( mesh->getNumberOfGroups( MED_CELL ) > 0 ||
                             mesh->getNumberOfGroups(face_entity) > 0 ||
                             mesh->getNumberOfGroups( MED_NODE) > 0 ))
      mesh->createFamilies();

    // general info

    int general_info[ I_MIN_SIZE ] = { // SLC_GENERAL_INFO
      mesh->getSpaceDimension(),
      mesh->getMeshDimension(),
      mesh->getNumberOfTypes( MED_CELL ),
      have_faces ? mesh->getNumberOfTypes( face_entity ) : 0,
      mesh->getNumberOfNodes(),
      mesh->getNumberOfGroups( MED_CELL )
      + mesh->getNumberOfGroups( face_entity )
      + mesh->getNumberOfGroups( MED_NODE ),
      mesh->getNumberOfFamilies( MED_CELL ),
      mesh->getNumberOfFamilies( face_entity ),
      mesh->getNumberOfFamilies( MED_NODE ),
      int( have_families )
    };
    vector<MEDMEM::FAMILY*> families;
    vector<MEDMEM::GROUP *> groups;
    vector<int> family_gen_data, family_group_ids;
    int nb_fam_types = getFamilyData( mesh, families, groups, family_gen_data, family_group_ids );

    // count nb of int data slices taking into account non-fixed slices:
    // 1) connectivities of std types and 2) elements per type of family
    int nb_int_slices =
      SLC_FIRST_STD_CONN +
      general_info[ I_NB_CELL_TYPES ] +
      general_info[ I_NB_FACE_TYPES ] +
      nb_fam_types;

    // nb of char slices (names)
    int nb_char_slices =
      SLC_FIRST_COORD_NAME +
      2 * general_info[ I_SPACEDIM ] + // coord names + coord unites
      families.size() +                // family names
      groups.size();                   // group names

    int_data. resize( nb_int_slices );
    char_data.resize( nb_char_slices );

    // general info and strings of mesh

    int_data[ SLC_GENERAL_INFO ].copy( I_MIN_SIZE, general_info );
    char_data[ SLC_MESH_NAME ] = toCharSlice( mesh->getName() );
    char_data[ SLC_MESH_DESC ] = toCharSlice( mesh->getDescription() );
    char_data[ SLC_COORD_SYS ] = toCharSlice( mesh->getCoordinatesSystem() );
    int char_slice = SLC_FIRST_COORD_NAME; // next non-filled char slice
    for ( int ax = 0; ax < general_info[ I_SPACEDIM ]; ++ax )
    {
      char_data[ char_slice++ ] = toCharSlice( mesh->getCoordinatesNames()[ax] );
      char_data[ char_slice++ ] = toCharSlice( mesh->getCoordinatesUnits()[ax] );
    }
    int_data[ SLC_FAMILY_GEN_DATA  ].copy( family_gen_data.size(),  &family_gen_data[0] );
    int_data[ SLC_FAMILY_GROUP_IDS ].copy( family_group_ids.size(), &family_group_ids[0] );
    family_gen_data.clear(); family_group_ids.clear();

    // geom types and their connectvity

    int_data[ SLC_GEOM_TYPES ].resize( general_info[I_NB_CELL_TYPES] +
                                       general_info[I_NB_FACE_TYPES] );
    int* type_ptr = int_data[ SLC_GEOM_TYPES]._pointer;
    int i_slice = SLC_FIRST_STD_CONN;

    const medGeometryElement * cell_types = mesh->getTypes(MED_CELL);
    for ( int t = 0; t < general_info[I_NB_CELL_TYPES]; ++t )
    {
      *type_ptr++ = cell_types[t];
      int_data[ i_slice++ ].setPtr( mesh->getConnectivityLength( conn, MED_CELL, cell_types[t]),
                                    mesh->getConnectivity      ( conn, MED_CELL, cell_types[t]));
    }
    if ( have_faces )
    {
      const medGeometryElement * face_types = mesh->getTypes( face_entity );
      for ( int t = 0; t < general_info[I_NB_FACE_TYPES]; ++t )
      {
        *type_ptr++ = face_types[t];
        int_data[ i_slice++ ].setPtr(mesh->getConnectivityLength( conn, face_entity, face_types[t]),
                                     mesh->getConnectivity( conn, face_entity, face_types[t]));
      }
    }
    // polygon connectivity index

    medEntityMesh polygon_entity = mesh->getMeshDimension()==2 ? MED_CELL : MED_FACE;
    if ( int nb_polygon = mesh->getNumberOfElements( polygon_entity, MED_POLYGON ))
    {
      const int nbTypes = mesh->getNumberOfTypes( polygon_entity );
      const int * index = mesh->getConnectivityIndex( conn, polygon_entity );
      index += mesh->getGlobalNumberingIndex( polygon_entity )[ nbTypes-1 ] - 1;
      int_data[SLC_POLYGON_INDEX].setPtr( nb_polygon + 1, index );
    }
    // polyherdra connectivity index

    if ( int nb_polyhedra = mesh->getNumberOfElements( MED_CELL, MED_POLYHEDRA ))
    {
      const int nbTypes = mesh->getNumberOfTypes( MED_CELL );
      const int * index = mesh->getConnectivityIndex( conn, MED_CELL );
      index += mesh->getGlobalNumberingIndex( MED_CELL )[ nbTypes-1 ] - 1;
      int_data[SLC_POLYHED_INDEX].setPtr( nb_polyhedra + 1, index );
    }
    // coordinates

    coords.setPtr( general_info[I_NB_NODES] * general_info[I_SPACEDIM],
                   mesh->getCoordinates( MED_FULL_INTERLACE ));

    // elements and names of families

    int is_on_all_elements[] = { IS_ON_ALL_FLAG };
    for ( int f = 0; f < families.size(); ++f )
    {
      char_data[ char_slice++ ] = toCharSlice( families[f]->getName() );

      if ( families[f]->isOnAllElements() )
      {
        int_data[ i_slice++ ].copy( 1, is_on_all_elements);
      }
      else
      {
        bool is_nodal = ( families[f]->getEntity() == MED_NODE );
        int nb_types = is_nodal ? 1 : families[f]->getNumberOfTypes();
        for ( int t = 0; t < nb_types; ++t )
        {
          medGeometryElement type = is_nodal ? MED_ALL_ELEMENTS : families[f]->getTypes()[t];
          int_data[ i_slice++ ].setPtr( families[f]->getNumberOfElements( type ),
                                        families[f]->getNumber( type ));
        }
      }
    }
    // names of groups

    for ( int g = 0; g < groups.size(); ++g )
      char_data[ char_slice++ ] = toCharSlice( groups[g]->getName() );

    return true;
  }

}

//================================================================================
/*!
 * \brief Send mesh to processor irank
 *
 * WARNING: do NOT delete the mesh until this->isSent()!
 */
//================================================================================

void MeshSendReceive::send(int                irank,
                           int                idomain,
                           MEDMEM::MESH*      mesh,
                           const vector<int>& cell_glob_numbers,
                           const vector<int>& face_glob_numbers,
                           const vector<int>& node_glob_numbers)
{
  _mesh = mesh;
  vector< TIntSlice  > int_data;
  vector< TCharSlice > char_data;
  TDoubleSlice         coords;
  if ( _mesh && getMeshData( _mesh, int_data, char_data, coords ))
  {
    serialize( int_data, _int_buf );
    serialize( char_data, _char_buf );
    _node_glob_numbers = node_glob_numbers;
    _cell_glob_numbers = cell_glob_numbers;
    _face_glob_numbers = face_glob_numbers;

    // sending
    MPI_Isend((void*)&_int_buf[0], _int_buf.size(), MPI_INT, irank,
              idomain + TAG_INT, MPI_COMM_WORLD, &_int_request);

    MPI_Isend((void*)coords._pointer, coords._size, MPI_DOUBLE, irank,
              idomain + TAG_COORD, MPI_COMM_WORLD, &_coord_request);

    MPI_Isend((void*)&_char_buf[0], _char_buf.size(), MPI_CHAR, irank,
              idomain + TAG_CHAR, MPI_COMM_WORLD, &_char_request);

    MPI_Isend((void*)&_cell_glob_numbers[0], _cell_glob_numbers.size(), MPI_INT, irank,
              idomain + TAG_CELL_NUMS, MPI_COMM_WORLD, &_cell_nums_request);

    MPI_Isend((void*)&_face_glob_numbers[0], _face_glob_numbers.size(), MPI_INT, irank,
              idomain + TAG_FACE_NUMS, MPI_COMM_WORLD, &_face_nums_request);

    MPI_Isend((void*)&_node_glob_numbers[0], _node_glob_numbers.size(), MPI_INT, irank,
              idomain + TAG_NODE_NUMS, MPI_COMM_WORLD, &_node_nums_request);
  }
  else
  {
    // send anything not to block recv()
    MPI_Isend((void*)&irank, 1, MPI_INT, irank,
              idomain + TAG_INT, MPI_COMM_WORLD, &_int_request);
  }
}

//================================================================================
/*!
 * \brief Receive mesh from irank processor synchronously
 */
//================================================================================

MEDMEM::MESH* MeshSendReceive::recv(int irank, int idomain,
                                    vector<int>& cell_glob_numbers,
                                    vector<int>& face_glob_numbers,
                                    vector<int>& node_glob_numbers)
{
  _mesh = 0;

  // receive INT data

  MPI_Status status;
  MPI_Probe(irank, idomain + TAG_INT, MPI_COMM_WORLD, &status); // wait for the message
  int msg_size = 0;
  MPI_Get_count( &status, MPI_INT, &msg_size ); // size of arrived message
  _int_buf.resize( msg_size );
  MPI_Recv((void*)&_int_buf[0], _int_buf.size(), MPI_INT, irank,
           idomain + TAG_INT, MPI_COMM_WORLD, &status);
  if ( msg_size <= I_MIN_SIZE )
    return 0; // mesh not sent

  // receive coordinates

  MPI_Probe(irank, idomain + TAG_COORD, MPI_COMM_WORLD, &status); // wait for the message
  MPI_Get_count( &status, MPI_DOUBLE, &msg_size ); // size of arrived message
  vector< double > coord_buf( msg_size );
  MPI_Recv((void*)&coord_buf[0], msg_size, MPI_DOUBLE, irank,
           idomain + TAG_COORD, MPI_COMM_WORLD, &status);

  // receive chars

  MPI_Probe(irank, idomain + TAG_CHAR, MPI_COMM_WORLD, &status); // wait for the message
  MPI_Get_count( &status, MPI_CHAR, &msg_size ); // size of arrived message
  _char_buf.resize( msg_size );
  MPI_Recv((void*)&_char_buf[0], _char_buf.size(), MPI_CHAR, irank,
           idomain + TAG_CHAR, MPI_COMM_WORLD, &status);

  // receive global cell numbers

  MPI_Probe(irank, idomain + TAG_CELL_NUMS, MPI_COMM_WORLD, &status); // wait for the message
  MPI_Get_count( &status, MPI_INT, &msg_size ); // size of arrived message
  cell_glob_numbers.resize( msg_size );
  MPI_Recv((void*)&cell_glob_numbers[0], cell_glob_numbers.size(), MPI_INT, irank,
           idomain + TAG_CELL_NUMS, MPI_COMM_WORLD, &status);

  // receive global face numbers

  MPI_Probe(irank, idomain + TAG_FACE_NUMS, MPI_COMM_WORLD, &status); // wait for the message
  MPI_Get_count( &status, MPI_INT, &msg_size ); // size of arrived message
  face_glob_numbers.resize( msg_size );
  MPI_Recv((void*)&face_glob_numbers[0], face_glob_numbers.size(), MPI_INT, irank,
           idomain + TAG_FACE_NUMS, MPI_COMM_WORLD, &status);

  // receive global node numbers

  MPI_Probe(irank, idomain + TAG_NODE_NUMS, MPI_COMM_WORLD, &status); // wait for the message
  MPI_Get_count( &status, MPI_INT, &msg_size ); // size of arrived message
  node_glob_numbers.resize( msg_size );
  MPI_Recv((void*)&node_glob_numbers[0], node_glob_numbers.size(), MPI_INT, irank,
           idomain + TAG_NODE_NUMS, MPI_COMM_WORLD, &status);

  // make mesh

  vector< TIntSlice > int_data;
  if ( int badSlice = deserialize( _int_buf, int_data ))
    throw MED_EXCEPTION(MEDMEM::STRING("MeshSendReceive::recv(")<<irank<<"): "
                                  << "int deserialization error on slice "<< badSlice);

  bool toClearFamilies;
  _mesh = makeMesh( int_data, toClearFamilies );
  _int_buf.clear();
  if ( !_mesh )
    return 0;

  // set coordinates

  ((MEDMEM::MESHING*)_mesh)->setCoordinates( _mesh->getSpaceDimension(),
                                             _mesh->getNumberOfNodes(),
                                             &coord_buf[0], "",
                                             MED_EN::MED_FULL_INTERLACE);
  coord_buf.clear();

  // set names

  vector< TCharSlice > char_data;
  if ( int badSlice = deserialize( _char_buf, char_data ))
    throw MED_EXCEPTION(MEDMEM::STRING("MeshSendReceive::recv(")<<irank<<"): "
                        << "char deserialization error on slice "<< badSlice);
  setNames( char_data, _mesh );
  _char_buf.clear();

  if ( toClearFamilies )
    ((TMESHING*) _mesh)->clearFamilies();

  return _mesh;
}

//=======================================================================
#else // #ifdef HAVE_MPI2

MeshSendReceive::MeshSendReceive() {}
MeshSendReceive::~MeshSendReceive() {}
void MeshSendReceive::send(int irank, int idomain, MEDMEM::MESH* mesh, const vector<int>& cell_glob_nums, const vector<int>& face_glob_nums, const vector<int>& node_glob_numbers) {}
MEDMEM::MESH* MeshSendReceive::recv(int irank,int idomain, vector<int>& cell_glob_nums, vector<int>& face_glob_nums, vector<int>& node_glob_numbers) { return 0; }
bool MeshSendReceive::isSent() { return 0; }
void MeshSendReceive::clear() {}

#endif
//=======================================================================
