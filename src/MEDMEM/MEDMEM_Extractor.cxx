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

// File      : MEDMEM_Extractor.cxx
// Created   : Thu Dec 18 11:10:11 2008
// Author    : Edward AGAPOV (eap)
//
#include "MEDMEM_Extractor.hxx"

#include <MEDMEM_Field.hxx>
#include <MEDMEM_Mesh.hxx>
#include <MEDMEM_Meshing.hxx>
#include <MEDMEM_Support.hxx>

#include <list>

#include <math.h>

using namespace MED_EN;
using namespace std;
using namespace MEDMEM;

namespace { // local tools

  const int _POLYGON = -1; //!< map key to store connectivity of polygons

  const double _TOLER = 1e-12;

  //================================================================================
  /*!
   * \brief calculate cross product of two vectors
   */
  void crossProduct(const double* v1, const double* v2, double* res)
  {
    res[0] = v1[1] * v2[2] - v1[2] * v2[1];
    res[1] = v1[2] * v2[0] - v1[0] * v2[2];
    res[2] = v1[0] * v2[1] - v1[1] * v2[0];
  }
  //================================================================================
  /*!
   * \brief calculate dot product of two vectors
   */
  double dotProduct(const double* v1, const double* v2)
  {
    return v1[0]*v2[0] + v1[1]*v2[1] + v1[2]*v2[2];
  }
  //================================================================================
  /*!
   * \brief Accessor to some ids. Provides operator[] and more-next access methods
   */
  class TIter
  {
    const int *_cur, *_end;
  public:
    TIter(const int* start, const int* end):_cur(start), _end(end) {}
    TIter(const int* start, int nb):_cur(start), _end(start+nb) {}
    int size() const { return _end - _cur; }
    int operator[] (int i) const { return _cur[i]; }
    bool more() const { return _cur < _end; }
    int next() { return *_cur++; }
    const int* begin() const { return _cur; }
    const int* end() const { return _end; }
  };
  //================================================================================
  /*!
   * \brief Edge linking two nodes, used to find equal edges and store edges in std containers
   */
  struct TEdge: public pair<int,int>
  {
    TEdge(const int n1=0, const int n2=0): pair<int,int>(n1,n2)
    { if ( n2 < n1 ) first = n2, second = n1; }
    TEdge(const TIter& nodes, int i )
    { *this = TEdge( nodes[i], nodes[ (i+1)%nodes.size() ]); }
    int node1() const { return first; }
    int node2() const { return second; }
  };
  //================================================================================
  /*!
   * \brief Tool providing iteration on edges of the cell of given classical type
   */
  struct TEdgeIterator
  {
    vector<TEdge>* _edges;
    TEdgeIterator(const medGeometryElement type);
    int getNbEdges() const { return _edges->size(); }
    TEdge getEdge(int i, const int* cellConn ) const
    { return TEdge( cellConn[(*_edges)[i].node1()], cellConn[(*_edges)[i].node2()]); }
    TEdge getEdge(int i, const TIter& cellNodes ) const
    { return TEdge( cellNodes[(*_edges)[i].node1()], cellNodes[(*_edges)[i].node2()]); }
  };
  //================================================================================
  /*!
   * \brief Comparator used to sort nodes of polygon
   */
  struct TNodeCompare
  {
    const double* _coords; //!< coordinates of mesh nodes in full interlace
    const double* _bc; //!< polygon barycentre
    int _i1, _i2; //!< indices of two ordinates used to compare points

    TNodeCompare(const double* nodeCoords, int i1, int i2):
      _coords(nodeCoords),_i1(i1),_i2(i2) {}

    void setBaryCenter(const double* bc) { _bc = bc; }

    bool operator()(const int pt1, const int pt2) {
      const double* p1 = _coords + 3*(pt1-1);
      const double* p2 = _coords + 3*(pt2-1);
      // calculate angles of Op1 and Op2 with the i1 axis on plane (i1,i2)
      double ang1 = atan2(p1[_i1] - _bc[0], p1[_i2] - _bc[1]);
      double ang2 = atan2(p2[_i1] - _bc[0], p2[_i2] - _bc[1]);
      return ang1 > ang2;
    }
  };
  //================================================================================
  /*!
   * \brief Return tolerance to consider nodes coincident
   */
  double getTolerance(const MESH* mesh )
  {
    vector< vector<double> > bb = mesh->getBoundingBox();
    double diagonal = 0;
    for ( int j = 0; j < mesh->getSpaceDimension(); ++j ) {
      double dist = bb[0][j] - bb[1][j];
      diagonal += dist*dist;
    }
    return sqrt( diagonal ) * 1e-7;
  }
  //================================================================================
  /*!
   * \brief Line data and some methods
   */
  struct TLine
  {
    const double* _dir;
    const double* _coord;
    int           _maxDir; //!< index of maximal coordinate of _dir
    TLine( const double* dir, const double* coord): _dir(dir), _coord(coord) {
      _maxDir = 0;
      if ( fabs( _dir[_maxDir] ) < fabs( _dir[1] )) _maxDir = 1;
      if ( fabs( _dir[_maxDir] ) < fabs( _dir[2] )) _maxDir = 2;
    }
    //!< Check if intersection points coincide in _maxDir direction
    bool isSame( const double* p1, const double* p2 ) const {
      return fabs(p1[_maxDir] - p2[_maxDir]) < _TOLER;
    }
  };
  //================================================================================
  /*!
   * \brief Result of transfixing a face
   *
   * TIntersection's make up a chain via _prevSection field. Depending on whether
   * _prevSection is NULL or not, there are two types of TIntersection:
   * . _prevSection == NULL: TIntersection ending the chain. It is TIntersection from
   * which chain building starts.
   * . _prevSection != NULL: intermediate TIntersection, stores cells cut by a result segment.
   */
  struct TIntersection
  {
    double   _point[3];   //!< coordinates of itersection
    set<int> _cells;      //!< intersected cells
    int      _face;       //!< intersected face of a cell
    set<int> _startNodes; //!< nodes around which to look for next intersection
    TIntersection* _prevSection; //!< neighbor intersection

    TIntersection(): _face(-1), _prevSection(NULL)
    {}
    ~TIntersection() {
      if ( _prevSection ) delete _prevSection; _prevSection=0;
    }
    void getRange( double& min, double& max, const int j ) const {
      if ( _point[j] < min ) min = _point[j];
      if ( _point[j] > max ) max = _point[j];
      if ( _prevSection ) _prevSection->getRange( min, max, j );
    }
    void reverse() {
      if ( _prevSection ) {
        _prevSection->reverse();
        _prevSection->_cells = _cells;
        _prevSection->_prevSection = this;
        _prevSection = NULL;
      }
    }
    int size() const { return 1 + ( _prevSection ? _prevSection->size() : 0 ); }
  };
  //================================================================================
  /*!
   * \brief Provider of comfortable access to connectivity data of MESH
   */
  struct TMeshData
  {
    int           _dim;
    double        _tolerance;
    const double* _coord;
    const int*    _cellConn;
    const int*    _cellConnIndex;
    const int*    _faceConn;
    const int*    _faceConnIndex;
    const int*    _face2Cell;
    const int*    _face2CellIndex;
    const int*    _cell2Face;
    const int*    _cell2FaceIndex;
    const int*    _node2Cell;
    const int*    _node2CellIndex;
    TMeshData(const MESH &mesh)
    {
      _tolerance      = getTolerance(&mesh);
      _dim            = mesh.getSpaceDimension();
      _coord          = mesh.getCoordinates(MED_FULL_INTERLACE);
      _cellConn       = mesh.getConnectivity( MED_NODAL, MED_CELL, MED_ALL_ELEMENTS);
      _cellConnIndex  = mesh.getConnectivityIndex(MED_NODAL, MED_CELL);
      _cell2Face      = mesh.getConnectivity( MED_DESCENDING, MED_CELL, MED_ALL_ELEMENTS);
      _cell2FaceIndex = mesh.getConnectivityIndex( MED_DESCENDING, MED_CELL );
      _face2Cell      = mesh.getReverseConnectivity( MED_DESCENDING );
      _face2CellIndex = mesh.getReverseConnectivityIndex( MED_DESCENDING );
      _faceConn       = mesh.getConnectivity( MED_NODAL, MED_FACE, MED_ALL_ELEMENTS);
      _faceConnIndex  = mesh.getConnectivityIndex(MED_NODAL, MED_FACE);
      _node2Cell      = mesh.getReverseConnectivity( MED_NODAL );
      _node2CellIndex = mesh.getReverseConnectivityIndex( MED_NODAL );
    }
    double tolerance() const {
      return _tolerance; }
    const double* getNodeCoord( int node ) const {
      return _coord + _dim*(node-1); }
    TIter getFaces( int cell ) const {
      return TIter( _cell2Face+_cell2FaceIndex[cell-1]-1, _cell2Face+_cell2FaceIndex[cell]-1 ); }
    TIter getCellNodes( int cell ) const {
      return TIter( _cellConn+_cellConnIndex[cell-1]-1, _cellConn+_cellConnIndex[cell]-1 ); }
    TIter getFaceNodes( int face ) const {
      face = abs(face);
      return TIter( _faceConn+_faceConnIndex[face-1]-1, _faceConn+_faceConnIndex[face]-1 ); }
    TIter getCellsByNode( int node ) const {
      return TIter( _node2Cell+_node2CellIndex[node-1]-1, _node2Cell+_node2CellIndex[node]-1 ); }
    TIter getCellsByFace( int face ) const {
      face = abs(face);
      return TIter( _face2Cell+_face2CellIndex[face-1]-1, _face2Cell+_face2CellIndex[face]-1 ); }
    int isFreeFace( int face ) const {
      TIter cells = getCellsByFace( face );
      return ( cells[1] == 0 ) ? cells[0] : 0; }
  };
  //================================================================================
  /*!
   * \brief Calculates face normal
   *  \retval bool - false if face has zero area
   */
  bool calcFaceNormal( const int        face,
                       const TMeshData& meshData,
                       double*          norm)
  {
    TIter nodes = meshData.getFaceNodes( face );
    bool zeroArea = false;
    int i = 0;
    do {
      const double* p1 = meshData.getNodeCoord( nodes[i] );
      const double* p2 = meshData.getNodeCoord( nodes[i+1] );
      const double* p3 = meshData.getNodeCoord( nodes[i+2] );
      double p2p1[3] = { p1[0]-p2[0], p1[1]-p2[1], p1[2]-p2[2] };
      double p2p3[3] = { p3[0]-p2[0], p3[1]-p2[1], p3[2]-p2[2] };
      crossProduct( p2p3, p2p1, norm );
      double normSize2 = dotProduct( norm, norm );
      zeroArea = (normSize2 < DBL_MIN);
    }
    while ( zeroArea && i < 2 );
    return zeroArea;
  }
  //================================================================================
  /*!
   * \brief Fill set of cells sharing edge
   */
  void getCellsSharingEdge( const TEdge& edge, const TMeshData& meshData, set<int> & theCells )
  {
    TIter cells = meshData.getCellsByNode( edge.node1() );
    while ( cells.more() ) {
      int cell = cells.next();
      TIter nodes = meshData.getCellNodes( cell );
      TEdgeIterator edgeIter( medGeometryElement( 300 + nodes.size() ));
      for ( int i = 0, nb = edgeIter.getNbEdges(); i < nb; ++i ) {
        TEdge e = edgeIter.getEdge( i , nodes );
        if ( edge == e ) {
          theCells.insert( cell );
          break;
        }
      }
    }
  }
  bool canIntersect(const int        cell,
                    const TMeshData& meshData,
                    const TLine&     line);

  TIntersection* intersect(const int        cell,
                           const TMeshData& meshData,
                           const TLine&     line,
                           set<int>&        checkedCells,
                           TIntersection*   prevInter=0);

} // noname namespace

namespace MEDMEM
{

  //================================================================================
  /*!
   * \brief Creates a tool
   *  \param inputField - input field
   *
   * The input field is supposed to comply with following conditions <ul>
   *<li>  it is constant by element (i.e. has 1 gauss point),</li>
   *<li>  it's support mesh does not contain poly elements,</li>
   *<li>  volumic elements have planar faces,</li>
   *<li>  surfasic elements have linear edges.</li></ul>
   */
  //================================================================================

Extractor::Extractor(const FIELD<double>& inputField) throw (MEDEXCEPTION)
: _myInputField( & inputField )    
{
  const char* LOC = "Extractor::Extractor(inputField) :";

  // Check if the input field complies with the conditions

  if ( !inputField.getSupport() )
    throw MEDEXCEPTION(STRING(LOC) << "InputField has NULL support");

  medEntityMesh entity = inputField.getSupport()->getEntity();
  if ( entity == MED_NODE || entity == MED_EDGE )
    throw MEDEXCEPTION(STRING(LOC) << "InputField has invalid supporting entity");

  if ( inputField.getSupport()->getNumberOfElements(MED_ALL_ELEMENTS) == 0 )
    throw MEDEXCEPTION(STRING(LOC) << "InputField has support of zero size");

  if ( inputField.getGaussPresence() && inputField.getNumberOfGaussPoints()[0] > 1 )
    throw MEDEXCEPTION(STRING(LOC) << "InputField is not constant be element");

  const GMESH* mesh = inputField.getSupport()->getMesh();
  if ( !mesh )
      throw MEDEXCEPTION(STRING(LOC) << "InputField has support with NULL mesh");

  if ( mesh->getSpaceDimension() < 2 )
      throw MEDEXCEPTION(STRING(LOC) << "InputField with 1D support not acceptable");

  if ( mesh->getNumberOfElements(MED_CELL, MED_POLYGON) > 0 ||
       mesh->getNumberOfElements(MED_CELL, MED_POLYHEDRA) > 0 )
      throw MEDEXCEPTION(STRING(LOC) << "InputField has supporting mesh with poly elements");

  if ( mesh->getMeshDimension() < 2 )
      throw MEDEXCEPTION(STRING(LOC) << "Invalid entity dimension of connectivity");

  _myInputField->addReference();
  _myInputMesh = mesh->convertInMESH();
}

Extractor::~Extractor()
{
  _myInputField->removeReference();
  _myInputMesh->removeReference();
}

//================================================================================
  /*!
   * \brief Creates a field by cutting inputField by a plane
   *  \param coords - give a point to pass through by the plane
   *  \param normal - gives the plane normal
   *  \retval FIELD<double>* - resulting field holding ownership of its support,
   *                           which in its turn holds ownership of its mesh
   *
   * If the plane does not intersect the field, NULL is returned.
   */
//================================================================================

FIELD<double>* Extractor::extractPlane(const double* coords, const double* normal)
  throw (MEDEXCEPTION)
{
  const char* LOC = "Extractor::extractPlane(const double* coords, const double* normal) :";

  // check agrument validity
  if ( !coords || !normal )
    throw MEDEXCEPTION(STRING(LOC) << "NULL argument");

  double normalSize = sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
  if ( normalSize <= DBL_MIN )
    throw MEDEXCEPTION(STRING(LOC) << "normal has zero size");

  if ( _myInputField->getSupport()->getMesh()->getSpaceDimension() < 3 )
    throw MEDEXCEPTION(STRING(LOC) << "Extraction by plane is possible in 3D space only ");

  double norm[3] = { normal[0] / normalSize, normal[1] / normalSize, normal[2] / normalSize };

  // cut mesh
  map<int,set<int> > new2oldCells;
  MESH* mesh = divideEdges( coords, norm, new2oldCells );
  if ( !mesh ) return 0;

  FIELD<double>* ret=makeField( new2oldCells, mesh );
  return ret;
}

//================================================================================
  /*!
   * \brief Creates a field by cutting inputField by a line
   *  \param coords - give a point to pass through by the line
   *  \param direction - gives a vector collinear to the line
   *  \retval FIELD<double>* - resulting field holding ownership of its support,
   *                           which in its turn holds ownership of its mesh
   *
   * If the line does not intersect the field, NULL is returned.
   */
//================================================================================

FIELD<double>* Extractor::extractLine(const double* coords, const double* direction)
  throw (MEDEXCEPTION)
{
  const char* LOC = "Extractor::extractLine(const double* coords, const double* direction) :";

  // check agrument validity
  if ( !coords || !direction )
    throw MEDEXCEPTION(STRING(LOC) << "NULL argument");

  double directionSize =
    sqrt(direction[0]*direction[0] + direction[1]*direction[1] + direction[2]*direction[2]);
  if ( directionSize <= DBL_MIN )
    throw MEDEXCEPTION(STRING(LOC) << "direction has zero size");

  const SUPPORT* support = _myInputField->getSupport();
  const medGeometryElement* inTypes = support->getTypes();
  const int meshDim = inTypes[ support->getNumberOfTypes()-1 ] / 100;

  if ( meshDim == 2 && support->getMesh()->getSpaceDimension() == 3 )
    throw MEDEXCEPTION(STRING(LOC) << "Extraction from 2D mesh not supported");

  map<int,set<int> > new2oldCells;
  MESH* mesh = 0;
  if ( meshDim == 2 )
  {
    double norm[2] = { direction[1] / directionSize,
                       direction[0] / directionSize,  };
    // cut mesh
    mesh = divideEdges( coords, norm, new2oldCells );
  }
  else
  {
    double dir[3] = { direction[0] / directionSize,
                      direction[1] / directionSize,
                      direction[2] / directionSize };
    mesh = transfixFaces( coords, dir, new2oldCells );
  }

  if ( !mesh ) return 0;
    
  FIELD<double>*ret=makeField( new2oldCells, mesh );
  return ret;
}

//================================================================================
/*!
 * \brief Creates a new field and fill it from the input one
 *  \param new2oldCells - mapping of new to old cells
 *  \retval FIELD<double>* - resulting field
 */
//================================================================================

FIELD<double>* Extractor::makeField( const map<int,set<int> >& new2oldCells,
                                     MESH*                     mesh ) const
{
  // make new field
  int nbComp = _myInputField->getNumberOfComponents();
  const SUPPORT *sup = mesh->getSupportOnAll( MED_CELL );
  FIELD<double> * outField = new FIELD<double>( sup, nbComp );

  outField->setComponentsNames       ( _myInputField->getComponentsNames() );
  outField->setName                  ( STRING("Extracted from ")<< _myInputField->getName() );
  outField->setDescription           ( STRING("Created by MEDMEM::Extractor"));
  outField->setComponentsDescriptions( _myInputField->getComponentsDescriptions() );
  outField->setMEDComponentsUnits    ( _myInputField->getMEDComponentsUnits() );

  sup->removeReference(); // to delete mesh as soon as outField dies

  // put values to the new field

  double* outValues = const_cast<double*>( outField->getValue() );

  map<int,set<int> >::const_iterator new_olds, noEnd = new2oldCells.end();
  for ( new_olds = new2oldCells.begin(); new_olds != noEnd; ++new_olds )
  {
    for ( int j = 0; j < nbComp; ++j )
    {
      int ind = ( new_olds->first - 1 ) * nbComp + j;
      outValues[ ind ] = 0.0;
      set<int>::const_iterator inCells = new_olds->second.begin();
      set<int>::const_iterator inEnd   = new_olds->second.end();    
      for ( ; inCells != inEnd; ++inCells )
        outValues[ ind ] += _myInputField->getValueIJ( *inCells, j+1 );

      outValues[ ind ] /= new_olds->second.size();
    }
  }
  return outField;
}

//================================================================================
/*!
 * \brief Makes a mesh by dividing edges of cells of the input mesh by plane
 *        in 3D or by line in 2D.
 *  \param coords - give a point to pass through by the plane or the line
 *  \param normal - gives the normal to plane or line
 *  \param new2oldCells - output map of new cells to cut input cell
 */
//================================================================================

MESH* Extractor::divideEdges(const double*       coords,
                             const double*       normal,
                             map<int,set<int> >& new2oldCells)
{
  const char* LOC = "Extractor::divideEdges()";

  const SUPPORT* support            = _myInputField->getSupport();
  medEntityMesh entity              = support->getEntity();
  const MESH* inMesh                = _myInputMesh;//support->getMesh();
  const medGeometryElement* inTypes = support->getTypes();

  const int* inConn      = inMesh->getConnectivity( MED_NODAL, entity, MED_ALL_ELEMENTS);
  const int* inConnIndex = inMesh->getConnectivityIndex(MED_NODAL, entity);
  const int spaceDim     = inMesh->getSpaceDimension();
  const int meshDim      = inTypes[ support->getNumberOfTypes()-1 ] / 100;


  // connectivity of new cells by nb of nodes per cell
  map< int, vector< int > > newConnByNbNodes;
  int nbInputCells = support->getNumberOfElements( MED_ALL_ELEMENTS );
  int minNbNodesPerCell = 2, maxNbNodesPerCell = 2;
  if ( meshDim == 3 ) {
    newConnByNbNodes[ 3 ].reserve( nbInputCells/2 );
    newConnByNbNodes[ 4 ].reserve( nbInputCells/2 );
    minNbNodesPerCell = 3;
    maxNbNodesPerCell = int (MED_POLYGON); // polygones allowed
  }
  else {
    newConnByNbNodes[ 2 ].reserve( nbInputCells/2 );
  }
  list<int> nbNodesPerPolygon; // to make connectivity index of polygons

  // new cells
  map< set<int>, int> oldNodes2newCell; //!< map connectivity of all old nodes to new cell id

  // new nodes
  map< TEdge, int > cutEdge2newNodeId; // ids of nodes located between extremities of old edge
  map< int, int >   oldNode2newNodeId; // ids of nodes coincident with old node

  // tolerance
  double tol = getTolerance( inMesh );


  // ----------------------------------------------
  // compute distances of nodes from plane or line
  // ----------------------------------------------

  computeDistanceOfNodes(coords, normal);


  // ----------
  // Cut edges
  // ----------

  int inCell = 0;
  for ( int iType = 0; iType < support->getNumberOfTypes(); ++iType) // loop on geom types
  {
    medGeometryElement type = inTypes[ iType ];
    TEdgeIterator edges( type );

    const int* inCells = 0;
    if ( !support->isOnAllElements() )
      inCells = support->getNumber( type );

    int nbInputCells = support->getNumberOfElements( type ); // loop on cells
    for ( int i = 0; i < nbInputCells; ++i )
    {
      int oldCell = inCells ? inCells[i] : ++inCell;
      const int* cellConn = inConn + (inConnIndex[ oldCell-1 ] - 1);

      // Nodes of new mesh are either coincide with input nodes or lay on
      // edges cut by plane. If at least one edge of a cell is cut in the middle
      // then there will be the new cell, if the plane pass only through nodes of
      // input cell then the new cell is not necessarily created.

      set<int> oldNodes, newNodes; //!< cut old nodes, new nodes on edges
      int nbEdgesOnPlane = 0;
      for ( int iEdge = 0; iEdge < edges.getNbEdges(); ++iEdge ) // loop on cell edges
      {
        // Analyse edge position in relation to the cutting plane or line
        const TEdge& edge = edges.getEdge( iEdge, cellConn );
        double dist1 = _myNodeDistance[ edge.node1()-1 ];
        double dist2 = _myNodeDistance[ edge.node2()-1 ];
        bool n1OnPlane = fabs( dist1 ) < tol;
        bool n2OnPlane = fabs( dist2 ) < tol;
        if ( n1OnPlane )
          oldNodes.insert( edge.node1() );
        if ( n2OnPlane )
          oldNodes.insert( edge.node2() );
        else if ( !n1OnPlane && dist1 * dist2 < 0 ) {
          // edge intersected
          int newNode = cutEdge2newNodeId.size() + oldNode2newNodeId.size() + 1;
          int node = cutEdge2newNodeId.insert( make_pair( edge, newNode )).first->second;
          newNodes.insert( node );
        }
        nbEdgesOnPlane += int( n1OnPlane && n2OnPlane );
      }
      int nbNodesInNewCell = oldNodes.size() + newNodes.size();
      if ( nbNodesInNewCell > maxNbNodesPerCell )
        throw MEDEXCEPTION(STRING(LOC) << "invalid input mesh");

      if ( nbNodesInNewCell >= minNbNodesPerCell )
      {
        // Associate new and old cells
        int newCell = new2oldCells.size() + 1;
        // detect equal new cells on boundaries of old cells
        if ( newNodes.empty() && (int)oldNodes.size() == nbEdgesOnPlane + int(meshDim==2)) {
          pair < map< set<int>, int>::iterator, bool > it_unique =
            oldNodes2newCell.insert( make_pair( oldNodes, newCell ));
          if ( !it_unique.second ) { // equal new faces
            int equalNewCell = it_unique.first->second;
            new2oldCells[ equalNewCell ].insert( oldCell );
            continue;
          }
        }
        set<int>& oldCells = // add a set of old cells to the end of new2oldCells
          new2oldCells.insert( new2oldCells.end(), make_pair(newCell, set<int>()))->second;
        oldCells.insert( oldCell );

        // Store nodes
        vector< int >& connectivity =
          nbNodesInNewCell>4 ? newConnByNbNodes[_POLYGON] : newConnByNbNodes[nbNodesInNewCell];
        // nodes at edge intersection
        set<int>::iterator n, nEnd;
        for ( n = newNodes.begin(), nEnd = newNodes.end(); n != nEnd; ++n )
        {
          connectivity.push_back( *n );
        }
        // nodes coincident with input nodes
        for ( n = oldNodes.begin(), nEnd = oldNodes.end(); n != nEnd; ++n )
        {
          int newNode = cutEdge2newNodeId.size() + oldNode2newNodeId.size() + 1;
          int node = oldNode2newNodeId.insert( make_pair( *n, newNode )).first->second;
          connectivity.push_back( node );
        }
        if ( nbNodesInNewCell>4 )
          nbNodesPerPolygon.push_back( nbNodesInNewCell );
      }
    } // loop on cells, cutting thier edges

  } // loop on geom types of input mesh


  // -----------------
  // Make coordinates
  // -----------------

  int nbNodes = cutEdge2newNodeId.size() + oldNode2newNodeId.size();
  vector< double > resCoords( nbNodes * spaceDim );

  const double* inCoords = inMesh->getCoordinates(MED_FULL_INTERLACE);

  // nodes in the middle of edges
  map< TEdge, int >::iterator edge_node, enEnd = cutEdge2newNodeId.end();
  for ( edge_node = cutEdge2newNodeId.begin(); edge_node != enEnd; ++edge_node )
  {
    int newNode = edge_node->second;
    const TEdge& edge = edge_node->first;

    double*     nodeCoords = & resCoords[ spaceDim * ( newNode-1 )];
    const double* n1Coords = inCoords +   spaceDim * ( edge.node1()-1 );
    const double* n2Coords = inCoords +   spaceDim * ( edge.node2()-1 );

    double dist1 = _myNodeDistance[ edge.node1()-1 ];
    double dist2 = _myNodeDistance[ edge.node2()-1 ];
    double r1 = dist1 / ( dist1 - dist2 );

    for ( int j = 0 ; j < spaceDim; ++j )
      nodeCoords[ j ] = ( 1.-r1 ) * n1Coords[ j ] + r1 * n2Coords[ j ];
  }

  // nodes coincident with input nodes
  map< int, int >::iterator old_newNode, onEnd = oldNode2newNodeId.end();
  const size_t size = size_t( sizeof(double)*spaceDim );
  for ( old_newNode = oldNode2newNodeId.begin(); old_newNode != onEnd; ++old_newNode )
  {
    double*       newCoords = & resCoords[ spaceDim * ( old_newNode->second - 1 )];
    const double* oldCoords = inCoords + spaceDim * ( old_newNode->first - 1 );
    memcpy( newCoords, oldCoords, size );
  }

  // --------------------
  // Sort nodes of cells
  // --------------------
  if ( nbNodes > 0 )
    sortNodes( newConnByNbNodes, &resCoords[0], coords, normal, nbNodesPerPolygon );

  // ----------
  // Make mesh
  // ----------

  // count types
  vector< medGeometryElement > types;
  vector< int > nbCellByType;
  map< int, vector< int > >::iterator nbNoConn, ncEnd =newConnByNbNodes.end();
  for ( nbNoConn = newConnByNbNodes.begin(); nbNoConn != ncEnd; ++nbNoConn )
  {
    int nbNodesPerCell = nbNoConn->first;
    int connSize = nbNoConn->second.size();
    if ( connSize == 0 ) continue;
    if ( nbNodesPerCell >= 2 && nbNodesPerCell <= 4 )
    {
      nbCellByType.push_back( connSize / nbNodesPerCell );
      types.push_back( medGeometryElement( (meshDim-1)*100 + nbNodesPerCell ));
    }
    else
    {
      nbCellByType.push_back( nbNodesPerPolygon.size() );
      types.push_back( MED_POLYGON );
    }
  }
  if ( types.empty() )
    return 0;

  MESHING* meshing = new MESHING();

  meshing->setName(STRING("Cut of ") << inMesh->getName());
  meshing->setNumberOfTypes( types.size(), MED_CELL );
  meshing->setCoordinates( spaceDim, nbNodes, & resCoords[0],
                           inMesh->getCoordinatesSystem(), MED_FULL_INTERLACE );
  meshing->setTypes( &types[0], MED_CELL );
  meshing->setNumberOfElements( &nbCellByType[0], MED_CELL);
  for ( unsigned i = 0; i < types.size(); ++i )
    if ( types[i] != MED_POLYGON )
    {
      meshing->setConnectivity( MED_CELL, types[i], & newConnByNbNodes[ types[i]%100 ].front());
    }
    else
    {
      // make index
      vector<int> index;
      index.reserve( nbNodesPerPolygon.size()+1 );
      index.push_back( 1 );
      list<int>::iterator nbNodes = nbNodesPerPolygon.begin(), nnEnd = nbNodesPerPolygon.end();
      for ( ; nbNodes != nnEnd; ++nbNodes )
        index.push_back( index.back() + *nbNodes );
    
      meshing->setConnectivity( MED_CELL, types[i], & newConnByNbNodes[ _POLYGON ].front(),
                                & index[0]);
    }

  return meshing;
}

//================================================================================
/*!
 * \brief computes distance of each node from the plane or the line given by point and normal
 */
//================================================================================

void Extractor::computeDistanceOfNodes(const double* point,
                                       const double* normal)
{
  const MESH* mesh     = _myInputMesh;
  const double * coord = mesh->getCoordinates(MED_FULL_INTERLACE);
  const int spaceDim   = mesh->getSpaceDimension();

  _myNodeDistance.resize(mesh->getNumberOfNodes());

  // compute dot product: normal * Vec(point,node)
  for ( int i = 0; i < mesh->getNumberOfNodes(); ++i )
  {
    _myNodeDistance[i] = 0.0;
    for ( int j = 0; j < spaceDim; ++j, ++coord )
    {
      _myNodeDistance[i] += normal[j] * (*coord - point[j]);
    }
  }
}

//================================================================================
/*!
 * \brief Orient elements correctly and sort nodes of polygons
 *  \param connByNbNodes - map of nb of nodes per cell to cell connectivity to rearrange
 *  \param nodeCoords - coordinates of nodes of a new mesh in full interlace
 *  \param point - point on plane or line
 *  \param normal - normal to plane or line
 *  \param nbNodesPerPolygon - index of connByNbNodes[_POLYGON] connectivity 
 */
//================================================================================

void Extractor::sortNodes( map< int, vector< int > >& connByNbNodes,
                           const double*              nodeCoords,
                           const double*              point,
                           const double*              normal,
                           const list<int> &          nbNodesPerPolygon)
{
  const int spaceDim = _myInputField->getSupport()->getMesh()->getSpaceDimension();

  if ( !connByNbNodes[2].empty() ) // 1D mesh
  {
    vector< int > & conn = connByNbNodes[2];
    if ( spaceDim == 2 )
    {
      // Orient edges along an coordinate axis,
      // select ordinate to check
      int ind = (fabs(normal[0]) < fabs(normal[1])) ? 1 : 0;
      // sorting
      for ( unsigned i = 0; i < conn.size(); i += 2) {
        const double* p1 = nodeCoords + spaceDim*(conn[i]-1);
        const double* p2 = nodeCoords + spaceDim*(conn[i+1]-1);
        if ( p1[ind] > p2[ind] )
          std::swap( conn[i], conn[i+1] );
      }
    }
    else
    {
      // Reverse if necessary adjacent edges if they have equal nodes
      if ( conn.size() > 2 ) {
        if ( conn[0] == conn[2] || conn[0] == conn[3] )
          std::swap( conn[0], conn[1] );
        int i;
        for ( i = 2; i < (int)conn.size()-2; i += 2) {
          if ( conn[i-1] == conn[i+1] )
            std::swap( conn[i], conn[i+1] );
          else if ( conn[i] == conn[i+2] || conn[i] == conn[i+3] )
            std::swap( conn[i], conn[i+1] );
        }
        if ( conn[i+1] == conn[i-1] )
          std::swap( conn[i], conn[i+1] );
      }
    }
    return;
  }

  // Fix order of nodes

  // select two ordinates for sorting
  int i1 = 0, i2 = 1, i3 = 2;
  if ( fabs( normal[i1] ) > fabs( normal[i3] )) swap( i1, i3 );
  if ( fabs( normal[i2] ) > fabs( normal[i3] )) swap( i2, i3 );

  // comparator of nodes
  TNodeCompare nodeCompare( nodeCoords, i1, i2 );

  map< int, vector< int > >::iterator nbN_conn = connByNbNodes.begin();
  for ( ; nbN_conn != connByNbNodes.end(); ++nbN_conn )
  {
    if ( nbN_conn->second.empty() ) continue;

    int * conn    = & nbN_conn->second[0];
    int * connEnd = conn + nbN_conn->second.size();

    int nbNodesPerFace = nbN_conn->first;
    list<int>::const_iterator nbPolyNodes, npEnd = nbNodesPerPolygon.end();

    for ( nbPolyNodes = nbNodesPerPolygon.begin(); conn != connEnd; conn += nbNodesPerFace )
    {
      if ( nbPolyNodes != npEnd )
        nbNodesPerFace = *nbPolyNodes++;

      // Sort nodes of polygons and quadrangles
      
      if ( nbNodesPerFace > 3 )
      {
        // get barycenter
        double bary[2] = { 0., 0. };
        for ( int i = 0; i < nbNodesPerFace; ++i ) {
          const double* coord = nodeCoords + spaceDim*(conn[i]-1);
          bary[0] += coord[i1]; bary[1] += coord[i2];
        }
        bary[0] /= nbNodesPerFace; bary[1] /= nbNodesPerFace;
        nodeCompare.setBaryCenter( bary );

        // sorting
        std::sort( conn, conn + nbNodesPerFace, nodeCompare);
      }

      // Fix orientation of faces, orient them to have thier normal collinear with plane normal

      // calculate cross product of two adjacent segments
      double dot = 0.;
      int i = 0;
      do {
        const double* p1 = nodeCoords + spaceDim*(conn[i+0]-1);
        const double* p2 = nodeCoords + spaceDim*(conn[i+1]-1);
        const double* p3 = nodeCoords + spaceDim*(conn[i+2]-1);
        double p2p1[2] = { p1[i1]-p2[i1] , p1[i2]-p2[i2] };
        double p2p3[2] = { p3[i1]-p2[i1] , p3[i2]-p2[i2] };
        dot = p2p3[1] * p2p1[0] - p2p3[0] * p2p1[1];
        ++i;
      }
      while ( dot == 0. && i+2 < nbNodesPerFace );

      if ( dot * normal[i3] < 0. )
        std::reverse( conn, conn + nbNodesPerFace );
    }
  }
}
//================================================================================
/*!
 * \brief Makes a 1D mesh by transfixing faces of 3D cells of input mesh by the line
 *  \param coords - give a point to pass through by the line
 *  \param direction - direction of the line
 *  \param new2oldCells - output map of new cells to cut input cell
 */
//================================================================================

MESH* Extractor::transfixFaces( const double*       coords,
                                const double*       direction,
                                map<int,set<int> >& new2oldCells)
{
  const MESH* inMesh = _myInputMesh;
  TMeshData inMeshData( *inMesh );
  TLine line( direction, coords );

  const int nbFaces = inMesh->getNumberOfElements( MED_FACE, MED_ALL_ELEMENTS );
  const int dim = 3;

  // Intersect 1st domain

  vector< TIntersection*> chains;
  vector< pair< double, double > > ranges;
  int nbSegments = 0; // in the future mesh

  set<int> checkedCells;
  checkedCells.insert(0); // 0 is returned by getCellsByFace() for boundary faces

  int face = 1;
  for ( ; face <= nbFaces; ++face ) {
    if ( int soleCell = inMeshData.isFreeFace( face ))
      if ( checkedCells.insert(soleCell).second  &&
           canIntersect( soleCell, inMeshData, line ))
        if ( TIntersection* chain = intersect( soleCell, inMeshData, line, checkedCells )) {
          chains.push_back( chain );
          double min = DBL_MAX, max = -DBL_MAX;
          chain->getRange( min, max, line._maxDir );
          ranges.push_back( make_pair( min, max ));
          nbSegments += chain->size() - 1;
          break;
        }
  }
  if ( chains.empty() )
    return 0;

  // Intersect the rest domains

  for ( ; face <= nbFaces; ++face ) {
    if ( int soleCell = inMeshData.isFreeFace( face ))
      if ( checkedCells.insert(soleCell).second)
      {
        // check if at least one node of face is out of ranges of found chains
        TIter nodes = inMeshData.getFaceNodes( face );
        bool isOut = false;
        while ( nodes.more() && !isOut ) {
          double coord = inMeshData.getNodeCoord( nodes.next() )[ line._maxDir ];
          bool isIn = false;
          for ( unsigned i = 0; i < ranges.size() && !isIn; ++i ) {
            const pair< double, double > & minMax = ranges[i];
            isIn = ( minMax.first < coord && coord < minMax.second );
          }
          isOut = !isIn;
        }
        // try to intersect
        if ( isOut && canIntersect( soleCell, inMeshData, line ))
          if ( TIntersection* chain = intersect( soleCell, inMeshData, line, checkedCells )) {
            chains.push_back( chain );
            double min = DBL_MAX, max = -DBL_MAX;
            chain->getRange( min, max, line._maxDir );
            ranges.push_back( make_pair( min, max ));
            nbSegments += chain->size() - 1;
          }
      }
  }

  // Fill mesh data

  int nbNodes = nbSegments + chains.size();
  vector< double > resCoords( nbNodes * dim );
  vector< int > resConn; resConn.reserve( nbSegments * 2 );

  int iNode = 1, iSeg = 1;
  double* coord = & resCoords[0];
  const size_t cooSize = size_t( sizeof(double)*dim );

  for ( unsigned i = 0; i < chains.size(); ++i ) {
    TIntersection* section = chains[i];
    while ( section ) {
      memcpy( coord, section->_point, cooSize );
      coord += dim;
      if ( section->_prevSection ) {
        resConn.push_back( iNode++ );
        resConn.push_back( iNode );
        new2oldCells[ iSeg++ ] = section->_cells;
      }
      section = section->_prevSection;
    }
    iNode++;
    delete chains[i];
  }

  // Create mesh

  MESHING* meshing = new MESHING();

  meshing->setName(STRING("Cut of ") << inMesh->getName());
  meshing->setNumberOfTypes( 1, MED_CELL );
  meshing->setCoordinates( dim, nbNodes, &resCoords[0],
                           inMesh->getCoordinatesSystem(), MED_FULL_INTERLACE );
  meshing->setTypes( &MED_SEG2, MED_CELL );
  meshing->setNumberOfElements( &nbSegments, MED_CELL);
  meshing->setConnectivity( MED_CELL, MED_SEG2, & resConn[0]);

  return meshing;
}

} // namespace MEDMEM


class MapGeoEdge : public map< medGeometryElement, vector<TEdge>* >
{
public:
  MapGeoEdge();
  ~MapGeoEdge();
};

MapGeoEdge::MapGeoEdge()
{
  std::vector<TEdge> *edges=(*this)[MED_TRIA3]=(*this)[MED_TRIA6]=new vector<TEdge>();
  edges->reserve( 3 );
  edges->push_back( TEdge( 0, 1 ));
  edges->push_back( TEdge( 1, 2 ));
  edges->push_back( TEdge( 2, 0 ));
  edges=(*this)[MED_QUAD4]=(*this)[MED_QUAD8]=new vector<TEdge>();
  edges->reserve( 4 );
  edges->push_back( TEdge( 0, 1 ));
  edges->push_back( TEdge( 1, 2 ));
  edges->push_back( TEdge( 2, 3 ));
  edges->push_back( TEdge( 3, 0 ));
  edges=(*this)[MED_TETRA4]=(*this)[MED_TETRA10]=new vector<TEdge>();
  edges->reserve( 6 );
  edges->push_back( TEdge( 0, 1 ));
  edges->push_back( TEdge( 1, 2 ));
  edges->push_back( TEdge( 2, 0 ));
  edges->push_back( TEdge( 0, 3 ));
  edges->push_back( TEdge( 1, 3 ));
  edges->push_back( TEdge( 2, 3 ));
  edges=(*this)[MED_HEXA8]=(*this)[MED_HEXA20]=new vector<TEdge>();
  edges->reserve( 12 );
  edges->push_back( TEdge( 0, 1 ));
  edges->push_back( TEdge( 1, 2 ));
  edges->push_back( TEdge( 2, 3 ));
  edges->push_back( TEdge( 3, 0 ));
  edges->push_back( TEdge( 4, 5 ));
  edges->push_back( TEdge( 5, 6 ));
  edges->push_back( TEdge( 6, 7 ));
  edges->push_back( TEdge( 7, 4 ));
  edges->push_back( TEdge( 0, 4 ));
  edges->push_back( TEdge( 1, 5 ));
  edges->push_back( TEdge( 2, 6 ));
  edges->push_back( TEdge( 3, 7 ));
  edges=(*this)[MED_PYRA5]=(*this)[MED_PYRA13]=new vector<TEdge>();
  edges->reserve( 8 );
  edges->push_back( TEdge( 0, 1 ));
  edges->push_back( TEdge( 1, 2 ));
  edges->push_back( TEdge( 2, 3 ));
  edges->push_back( TEdge( 3, 0 ));
  edges->push_back( TEdge( 0, 4 ));
  edges->push_back( TEdge( 1, 4 ));
  edges->push_back( TEdge( 2, 4 ));
  edges->push_back( TEdge( 3, 4 ));
  edges=(*this)[MED_PENTA6]=(*this)[MED_PENTA15]=new vector<TEdge>();
  edges->reserve( 9 );
  edges->push_back( TEdge( 0, 1 ));
  edges->push_back( TEdge( 1, 2 ));
  edges->push_back( TEdge( 2, 0 ));
  edges->push_back( TEdge( 3, 4 ));
  edges->push_back( TEdge( 4, 5 ));
  edges->push_back( TEdge( 5, 3 ));
  edges->push_back( TEdge( 0, 4 ));
  edges->push_back( TEdge( 1, 5 ));
  edges->push_back( TEdge( 2, 3 ));
  (*this)[MED_NONE]         = 0;
  (*this)[MED_POINT1]       = 0;
  (*this)[MED_SEG2]         = 0;
  (*this)[MED_SEG3]         = 0;
  (*this)[MED_POLYGON]      = 0;
  (*this)[MED_POLYHEDRA]    = 0;
  (*this)[MED_ALL_ELEMENTS] = 0;
}

MapGeoEdge::~MapGeoEdge()
{
  delete (*this)[MED_TRIA6];
  delete (*this)[MED_QUAD8];
  delete (*this)[MED_TETRA10];
  delete (*this)[MED_HEXA20];
  delete (*this)[MED_PYRA13];
  delete (*this)[MED_PENTA15];
}

//================================================================================
/*!
 * \brief Constructs TEdgeIterator on given classical cell type
 */
//================================================================================

TEdgeIterator::TEdgeIterator(const medGeometryElement type)
{
  static MapGeoEdge _edgesByType;
  _edges = _edgesByType[type];
}

namespace {

//================================================================================
/*!
 * \brief Transfixes a cell with a line. Returns a head of chain of intersections
 *  \param cell - cell id to transfixe
 *  \param meshData - input 3D mesh
 *  \param line - cutting line
 *  \param checkedCells - set of cell already tried
 *  \param prevInter - previosly found intersection. Used to build a chain of
 *                     intersection via recursive call
 */
//================================================================================

TIntersection* intersect(const int        cell,
                         const TMeshData& meshData,
                         const TLine&     line,
                         set<int>&        checkedCells,
                         TIntersection*   prevInter)
{
  TIntersection* newSection = 0; // section to find
  TIntersection* auxSection = 0; // second section used when !prevInter
  list< TIntersection > bndSections;// list of intersection on edges

  int avoidFace = prevInter ? prevInter->_face : -1;

  TIter faces = meshData.getFaces( cell );
  while ( faces.more() ) /////////  loop on faces of the cell
  {
    int face = abs( faces.next() );
    if ( face == avoidFace )
      continue;
    TIter nodes = meshData.getFaceNodes( face );

    // Get face normal
    // ----------------

    double faceNormal[3];
    bool zeroArea = calcFaceNormal( face, meshData, faceNormal );
    if ( zeroArea )
      continue; // next face

    // Is face parallel to line
    // -------------------------

    double dirDotNorm = dotProduct( line._dir, faceNormal );
    const double* pFace0 = meshData.getNodeCoord( nodes[0] );
    if ( fabs( dirDotNorm ) < _TOLER )
    {
      // line || face, check if the line lays on the face

      double lf[3] = { line._coord[0] - pFace0[0],
                       line._coord[1] - pFace0[1],
                       line._coord[2] - pFace0[2] };
      double lfDotNorm = dotProduct( lf, faceNormal );
      if ( fabs( lfDotNorm ) < _TOLER )
      {
        // =========================================================
        // Line lays on face. Intersect line with edges of the face
        // =========================================================

        // Calculate distance of nodes from the line
        // ------------------------------------------
        double lineNormal[3];
        crossProduct( faceNormal, line._dir, lineNormal );
        vector<double> dist( nodes.size(), 0.0 );
        for ( int n = 0; n < nodes.size(); ++n )
        {
          const double* p = meshData.getNodeCoord( nodes[n] );
          for ( int j = 0; j < 3; ++j )
            dist[n] += lineNormal[j] * ( p[j] - line._coord[j] );
        }
        // Find intersections
        // -------------------
        vector<double> pCoords;  // intersection coordinates
        set<int>       pAtNodes; // intersected nodes of the face
        list<TEdge>    cutEdges; // intersected edges
        int nbPoints = 0;        // nb intersections
        pCoords.reserve(6);
        for ( int n = 0; n < nodes.size() && nbPoints < 2; ++n )
        {
          int n2 = (n+1) % nodes.size();
          double dist1 = dist[ n ];
          double dist2 = dist[ n2 ];
          bool n1OnLine = fabs( dist1 ) < meshData.tolerance();
          bool n2OnLine = fabs( dist2 ) < meshData.tolerance();
          if ( n1OnLine )
            pAtNodes.insert( nodes[n] );
          if ( n2OnLine )
            pAtNodes.insert( nodes[n2] );
          else if ( !n1OnLine && dist1 * dist2 < 0 ) {
            const double* p1 = meshData.getNodeCoord( nodes[n] );
            const double* p2 = meshData.getNodeCoord( nodes[n2] );
            double r1 = dist1 / ( dist1 - dist2 );
            for ( int j = 0 ; j < 3; ++j )
              pCoords.push_back(( 1.-r1 ) * p1[ j ] + r1 * p2[ j ]);
            cutEdges.push_back( TEdge( nodes[n], nodes[n2] ));
          }
          nbPoints = cutEdges.size() + pAtNodes.size();
        }
        // coords of intersection are stored in pCoords in order:
        // first go points on edges, then, points on nodes
        if ( nbPoints == 2 && !pAtNodes.empty() ) {
          set<int>::iterator n = pAtNodes.begin();
          while ( pCoords.size() != 6 ) { // there must be coords of two points
            const double* p = meshData.getNodeCoord( *n++ );
            for ( int j = 0 ; j < 3; ++j )
              pCoords.push_back( p[j] );
          }
        }
        // Store intersections
        // --------------------
        if ( nbPoints == 2 )
        {
          vector< TIntersection* > sections(nbPoints);
          const double* intCoord = & pCoords[0];

          for ( int i = 0; i < nbPoints; ++i, intCoord += 3 )
          {
            // Set coords of intersection point
            sections[i] = new TIntersection;
            sections[i]->_point[0] = intCoord[0];
            sections[i]->_point[1] = intCoord[1];
            sections[i]->_point[2] = intCoord[2];

            // Set intersected cells
            if ( cutEdges.empty() ) {
              // line can pass by edge shared by several cells
              TEdge cutEdge( *pAtNodes.begin(), *pAtNodes.rbegin() );
              getCellsSharingEdge( cutEdge, meshData, sections[i]->_cells );
            }
            if ( !cutEdges.empty() || sections[i]->_cells.empty() ) {
              // line pass by face between two cells
              TIter cells = meshData.getCellsByFace( face );
              while ( cells.more() )
                if ( int elem = cells.next() )
                  sections[i]->_cells.insert( elem );
            }
            // Not to check the face at next search
            sections[i]->_face = face;

            // Set nodes to start search of next intersection from
            if ( !cutEdges.empty() ) {
              sections[i]->_startNodes.insert( cutEdges.front().node1() );
              sections[i]->_startNodes.insert( cutEdges.front().node2() );
              cutEdges.pop_front();
            }
            else if ( pAtNodes.size() > 1 ) {
              set<int>::iterator p = pAtNodes.begin();
              if ( !line.isSame( intCoord, meshData.getNodeCoord( *p )))
                ++p;
              sections[i]->_startNodes.insert( *p );
              pAtNodes.erase( p );
            }
            else {
              sections[i]->_startNodes.insert( *pAtNodes.begin() );
            }
          }
          if ( prevInter ) {
            // Only one point is needed, exclude already found intersection
            if ( line.isSame( prevInter->_point, sections[0]->_point ))
              std::swap( sections[0], sections[1] );
            delete sections[1];
            sections[1] = 0;
          }
          newSection = sections[0];
          auxSection = sections[1];
          if ( auxSection )
            auxSection->_cells = newSection->_cells;

          bndSections.clear(); // remove already found intersections

        } // if ( nbPoints == 2 )

        break; // from loop on faces of cell

      } // line lays on face
    }
    else
    {
      // ==================================
      // Line intersects the plane of face
      // ==================================

      // Find distance of intersection point from line origin
      // t = faceNormal * ( pFace0 - line._coord ) / ( faceNormal * line._dir )

      double pf0Coord[] = { pFace0[0] - line._coord[0],
                            pFace0[1] - line._coord[1],
                            pFace0[2] - line._coord[2] };
      double t = dotProduct( faceNormal, pf0Coord ) / dotProduct( faceNormal, line._dir );

      // facePlane-line intersection point
      double ip[] = { line._coord[0] + line._dir[0] * t,
                      line._coord[1] + line._dir[1] * t,
                      line._coord[2] + line._dir[2] * t};

      if ( prevInter && line.isSame( ip, prevInter->_point ))
        continue;
      if ( !bndSections.empty() && line.isSame( ip, bndSections.back()._point ))
        continue;

      // Check if intersection point (ip) is inside the face.
      // ----------------------------------------------------

      // do it in 2d, on the cartesian plane most normal to the face;
      // find indices on that plane: i1, i2
      int i1 = 0, i2 = 1, i3 = 2;
      if ( fabs( faceNormal[i1] ) > fabs( faceNormal[i3] )) swap( i1, i3 );
      if ( fabs( faceNormal[i2] ) > fabs( faceNormal[i3] )) swap( i2, i3 );
      if ( i2-i1 != 1 && i2 != 0 ) swap ( i1, i2 );

      int inside = true, nbOnBoundary = 0;
      TEdge cutEdge;
      for ( int n = 0; n < nodes.size() && inside; ++n )
      {
        const double* p0 = meshData.getNodeCoord( nodes[n] );
        const double* p1 = meshData.getNodeCoord( nodes[ (n+1) % nodes.size() ] );
        double sign =
          faceNormal[i3]*((ip[i2] - p0[i2])*(p1[i1] - p0[i1]) - (ip[i1] - p0[i1])*(p1[i2] - p0[i2]));
        if ( sign < -DBL_MIN )
          inside = false;
        else if ( sign < DBL_MIN ) {
          nbOnBoundary++;
          cutEdge = TEdge( nodes, n );
        }
      }

      // Store intersection point
      // -------------------------
      if ( inside )
      {
        TIntersection* section;
        if ( !nbOnBoundary )
          section = new TIntersection;
        else {
          if ( bndSections.size() >= 2 )
            continue; // go to next face
          bndSections.push_back( TIntersection() );
          section = & bndSections.back();
          // set nodes to start next searching from
          if ( nbOnBoundary == 1 ) {
            // edge is cut
            section->_startNodes.insert( cutEdge.node1() );
            section->_startNodes.insert( cutEdge.node2() );
          }
          else { // node is cut
            const double* p1 = meshData.getNodeCoord( cutEdge.node1() );
            if ( fabs( ip[i1]-p1[i1] ) < _TOLER && fabs( ip[i2]-p1[i2] ) < _TOLER  )
              section->_startNodes.insert( cutEdge.node1() );
            else
              section->_startNodes.insert( cutEdge.node2() );
          }
        }
        section->_point[0] = ip[0];
        section->_point[1] = ip[1];
        section->_point[2] = ip[2];
        section->_face     = face;
        section->_cells.insert( cell );

        if ( !nbOnBoundary )
        {
          if ( !newSection )
            newSection = section;
          else
            auxSection = section;
          if ( prevInter || auxSection ) {
            bndSections.clear();
            break; // from loop on faces
          }
        }
      }
    }
  } // loop on faces of cell

  // Care of intersections passing through edges
  // --------------------------------------------

  if ( !bndSections.empty() )
  {
    if ( prevInter ) { // boundary section not equal to previous section
      if ( !newSection )
        newSection = new TIntersection( bndSections.front() );
    }
    else {
      if ( !newSection ) {
        newSection = new TIntersection( bndSections.front() );
        bndSections.pop_front();
      }
      if ( !auxSection && !bndSections.empty() ) {
        auxSection = new TIntersection( bndSections.front() );
      }
    }
  }

  // Find the rest of chain starting from the found sections
  // --------------------------------------------------------

  if ( newSection && ( prevInter || auxSection ))
  {
    TIntersection* chain[] = { newSection, auxSection };
    int chainLength[] = {0,0};
    for ( int i = 0; i < 2; ++i )
    {
      TIntersection* section = chain[i];
      if ( !section ) continue;
      // get cells to try to intersect next
      set<int> cellsToCheck;
      if ( !section->_startNodes.empty() ) {
        if ( section->_startNodes.size() == 1 ) {
          TIter cells = meshData.getCellsByNode( *section->_startNodes.begin() );
          cellsToCheck.insert( cells.begin(), cells.end() );
        }
        else {
          TEdge cutEdge( *section->_startNodes.begin(), *section->_startNodes.rbegin() );
          getCellsSharingEdge( cutEdge, meshData, cellsToCheck );
        }
      }
      else {
        TIter cells = meshData.getCellsByFace( section->_face );
        cellsToCheck.insert( cells.begin(), cells.end() );
      }
      // find the rest intersections
      chain[i] = 0;
      set<int>::iterator elem = cellsToCheck.begin(), elemEnd = cellsToCheck.end();
      for ( ; elem != elemEnd && !chain[i]; ++elem ) {
        if ( checkedCells.insert( *elem ).second &&
             section->_cells.find( *elem ) == section->_cells.end() )
        {
          chain[i] = intersect( *elem, meshData, line, checkedCells, section );
        }
      }
      if ( chain[i] )
        chainLength[i] = chain[i]->size();
    }

    // Connect just found sections into a chain
    if ( prevInter ) {
      newSection->_prevSection = prevInter;
    }
    else {
      if ( chainLength[0] < chainLength[1] ) {
        std::swap( chain[0], chain[1] );
        std::swap( newSection, auxSection );
      }
      if ( chain[1] )
        chain[1]->reverse();
      newSection->_prevSection = auxSection;
    }

    if ( chain[0] )
      return chain[0];
    return newSection;
  }
  else {
    delete newSection;
  }

  return 0;
}

//================================================================================
/*!
 * \brief Evaluate if the line can intersect a cell
 */
//================================================================================

bool canIntersect(const int        cell,
                  const TMeshData& meshData,
                  const TLine&     line)
{
  // calculate bnd box of the cell
  double min[] = { DBL_MAX,DBL_MAX,DBL_MAX }, max[] = { -DBL_MAX,-DBL_MAX,-DBL_MAX };

  TIter cellNodes = meshData.getCellNodes( cell );
  for ( int n = 0; n < cellNodes.size(); ++n ) {
    const double* node = meshData.getNodeCoord( cellNodes[n] );
    for ( int j = 0; j < 3; ++j ) {
      if ( node[j] < min[j] ) min[j] = node[j];
      if ( node[j] > max[j] ) max[j] = node[j];
    }
  }
  double xmin = 0, xmax = 0, ymin = 0, ymax = 0, zmin, zmax;
  double parmin, parmax, par1, par2;
  bool xToSet, yToSet;
  const double infinite = 1e100;

  if (fabs(line._dir[0])>0.) {
    par1=(min[0]-line._coord[0])/line._dir[0];
    par2=(max[0]-line._coord[0])/line._dir[0];
    parmin=std::min(par1, par2);
    parmax=std::max(par1, par2);
    xToSet=true;
  }
  else {
    if (line._coord[0]<min[0] || max[0]<line._coord[0]) {
      return false;
    }
    xmin=line._coord[0];
    xmax=line._coord[0];
    parmin=-infinite;
    parmax=infinite;
    xToSet=false;
  }

  if (fabs(line._dir[1])>0.) {
    par1=(min[1]-line._coord[1])/line._dir[1];
    par2=(max[1]-line._coord[1])/line._dir[1];
    if(parmax < std::min(par1,par2) || parmin > std::max(par1,par2))
      return false;
    parmin=std::max(parmin, std::min(par1,par2));
    parmax=std::min(parmax, std::max(par1,par2));
    yToSet=true;
  }
  else {
    if (line._coord[1]<min[1] || max[1]<line._coord[1]) {
      return false;
    }
    ymin=line._coord[1];
    ymax=line._coord[1];
    yToSet=false;
  }

  if (fabs(line._dir[2])>0.) {
    par1=(min[2]-line._coord[2])/line._dir[2];
    par2=(max[2]-line._coord[2])/line._dir[2];
    if(parmax < std::min(par1,par2) || parmin > std::max(par1,par2))
      return false;
    parmin=std::max(parmin, std::min(par1,par2));
    parmax=std::min(parmax, std::max(par1,par2));
    par1=line._coord[2]+parmin*line._dir[2];
    par2=line._coord[2]+parmax*line._dir[2];
    zmin=std::min(par1, par2);
    zmax=std::max(par1, par2);
  }
  else {
    if (line._coord[2]<min[2] || max[2]<line._coord[2])
      return false;
    zmin=line._coord[2];
    zmax=line._coord[2];
  }
  if (zmax<min[2] || max[2]<zmin) return false;

  if (xToSet) {
    par1=line._coord[0]+parmin*line._dir[0];
    par2=line._coord[0]+parmax*line._dir[0];
    xmin=std::min(par1, par2);
    xmax=std::max(par1, par2);
  }
  if (xmax<min[0] || max[0]<xmin) return false;

  if (yToSet) {
    par1=line._coord[1]+parmin*line._dir[1];
    par2=line._coord[1]+parmax*line._dir[1];
    ymin=std::min(par1, par2);
    ymax=std::max(par1, par2);
  }
  if (ymax<min[1] || max[1]<ymin) return false;

  return true;
}
} // unnamed namespace

