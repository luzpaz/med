#include "IntersectorHexa.hxx"

#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"

#include "MeshUtils.hxx"

#include "IntersectorTetra.hxx"

using namespace MED_EN;
using namespace MEDMEM;

namespace INTERP_UTILS
{
  
  IntersectorHexa::IntersectorHexa(const MEDMEM::MESH& srcMesh, const MEDMEM::MESH& targetMesh, int targetCell, SplittingPolicy policy)
  {

    // check type
    const medGeometryElement targetType = targetMesh.getElementType(MED_CELL, targetCell);
    assert(targetType == MED_HEXA8);

    const int numTetra = static_cast<int>(policy);

    // pre-calculate nodes
    calculateSubNodes(targetMesh, targetCell, policy);

    _tetra.reserve(numTetra);
    _nodes.reserve(30); // we never have more than this

    switch(policy)
      {
      case PLANAR_FACE_5:
	{
	  const int subZone[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	  fiveSplit(srcMesh, subZone);
	}
	break;

      case PLANAR_FACE_6:
	{
	  const int subZone[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	  sixSplit(srcMesh, subZone);
	}
	break;

      case GENERAL_24:
	{
	  calculateGeneral24Tetra(srcMesh);
	}
	break;

      case GENERAL_48:
	{
	  calculateGeneral48Tetra(srcMesh);
	}
	break;
      default:
	assert(false);
      }
  }
    
  IntersectorHexa::~IntersectorHexa()
  {
    for(vector<IntersectorTetra*>::iterator iter = _tetra.begin(); iter != _tetra.end(); ++iter)
      {
	delete *iter;
      }
    
    // free potential sub-mesh nodes that have been allocated
    vector<const double*>::iterator iter = _nodes.begin() + 8;
    while(iter != _nodes.end())
      {
	delete[] *iter;
	++iter;
      }
  }

  void IntersectorHexa::fiveSplit(const MEDMEM::MESH& srcMesh, const int* const subZone)
  {
    static const int SPLIT_NODES_5[20] = 
      {
	0, 1, 5, 2,
	0, 4, 5, 7,
	0, 3, 7, 2,
	5, 6, 7, 2,
	0, 2, 5, 7
      };
    
    for(int i = 0; i < 5; ++i)
      {
	const double* nodes[4];
	for(int j = 0; j < 4; ++j)
	  {
	    nodes[j] = getCoordsOfSubNode(subZone[ SPLIT_NODES_5[4*i+j] ]);
	  }
	IntersectorTetra* t = new IntersectorTetra(srcMesh, nodes);
	_tetra.push_back(t);
      }
  }

  void IntersectorHexa::sixSplit(const MEDMEM::MESH& srcMesh, const int* const subZone)
  {
    static const int SPLIT_NODES_6[24] = 
      {
	0, 1, 5, 6,
	0, 2, 1, 6,
	0, 5, 4, 6,
	0, 4, 7, 6,
	0, 3, 2, 6,
	0, 7, 3, 6
      };

    for(int i = 0; i < 6; ++i)
      {
	const double* nodes[4];
	for(int j = 0; j < 4; ++j)
	  {
	    nodes[j] = getCoordsOfSubNode(subZone[ SPLIT_NODES_6[4*i+j] ]);
	  }
	IntersectorTetra* t = new IntersectorTetra(srcMesh, nodes);
	_tetra.push_back(t);
      }
  }

  void IntersectorHexa::calculateGeneral24Tetra(const MEDMEM::MESH& srcMesh)
  {
    // the two mesh nodes used in each tetrahedron
    // the tetrahedra all have nodes (cellCenter, faceCenter, edgeNode1, edgeNode2)
    static const int TETRA_EDGES[48] = 
      {
	// face with center 9
	1, 2,
	2, 6,
	6, 5,
	5, 1,
	// face with center 10
	1, 2,
	2, 3,
	3, 4, 
	4, 1,
	// face with center 11
	1, 5,
	5, 8,
	8, 4,
	4, 1,
	// face with center 12
	2, 6,
	6, 7,
	7, 3,
	3, 2,
	// face with center 13
	6, 7,
	7, 8,
	8, 5,
	5, 6,
	// face with center 14
	3, 7,
	7, 8, 
	8, 4,
	4, 3
      };
    
    const double* nodes[4];
    
    // get the cell center
    nodes[0] = getCoordsOfSubNode(15);
    
    for(int faceCenterNode = 9; faceCenterNode <= 14; ++faceCenterNode)
      {
	// get the face center
	nodes[1] = getCoordsOfSubNode(faceCenterNode);
	for(int j = 0; j < 4; ++j)
	  {
	    const int row = 4*(faceCenterNode - 9) + j;
	    nodes[2] = getCoordsOfSubNode(TETRA_EDGES[2*row]);
	    nodes[3] = getCoordsOfSubNode(TETRA_EDGES[2*row + 1]);
	    
	    IntersectorTetra* t = new IntersectorTetra(srcMesh, nodes);
	    _tetra.push_back(t);
	  }
      }
  }

  void IntersectorHexa::calculateGeneral48Tetra(const MEDMEM::MESH& srcMesh)
  {
    // define 8 hexahedral subzones as in Grandy, p449
    // the values correspond to the nodes that correspond to nodes 1,2,3,4,5,6,7,8 in the subcell
    // these nodes have correspondance 1 -> 0, 2 -> a, 3 -> e, 4 -> d, 5 -> b, 6 -> c, 7 -> g, 8 -> f
    // with the Fig. 4.c in Grandy
    static const int subZones[64] = 
      {
	1, 9, 22, 13, 10, 21, 27, 23,
	9, 2, 14, 22, 21, 11, 24, 27,
	13, 22, 17, 4, 23, 27, 26, 18,
	22, 14, 3, 17, 27, 24, 19, 26,
	10, 21, 27, 23, 5, 12, 25, 15,
	21, 11, 24, 27, 12, 6, 16, 25,
	23, 27, 26, 18, 15, 25, 20, 8,
	27, 24, 19, 26, 25, 16, 7, 20
      };
    
    for(int i = 0; i < 8; ++i)
      {
	sixSplit(srcMesh, &subZones[8*i]);
      }
  }
  
  void IntersectorHexa::calculateSubNodes(const MEDMEM::MESH& targetMesh, int targetCell, SplittingPolicy policy)
  {
    // retrieve real mesh nodes
    for(int node = 1; node <= 8 ; ++node)
      {
	// calculate only normal nodes
	_nodes.push_back(getCoordsOfNode(node, targetCell, targetMesh));
      }

    // create sub-mesh nodes if needed
    switch(policy)
      {
      case GENERAL_24:
	{
	  static const int GENERAL_24_SUB_NODES[28] = 
	    {
	      1, 2, 5, 6, // sub-node 9 (face)
	      1, 2, 3, 4, // sub-node 10 (face)
	      1, 4, 5, 8, // sub-node 11 (face)
	      2, 3, 6, 7, // sub-node 12 (face)
	      5, 6, 7, 8, // sub-node 13 (face)
	      3, 4, 7, 8, // sub-node 14 (face)
	      10, 11, 12, 13 // sub-node 15 (cell)
	    };
	  
	  for(int i = 0; i < 7; ++i)
	    {
	      double* barycenter = new double[3];
	      calcBarycenter<4>(barycenter, &GENERAL_24_SUB_NODES[4*i]);
	      _nodes.push_back(barycenter);
	    }
	}
	break;
	
      case GENERAL_48:
	{
	  static const int GENERAL_48_SUB_NODES[38] = 
	    {
	      1, 2,  // sub-node 9 (edge)
	      1, 5,  // sub-node 10 (edge)
	      2, 6,  // sub-node 11 (edge)
	      5, 6,  // sub-node 12 (edge)
	      1, 4,  // sub-node 13 (edge)
	      2, 3,  // sub-node 14 (edge)
	      5, 8,  // sub-node 15 (edge)
	      6, 7,  // sub-node 16 (edge)
	      3, 4,  // sub-node 17 (edge)
	      4, 8,  // sub-node 18 (edge)
	      3, 7,  // sub-node 19 (edge)
	      7, 8,  // sub-node 20 (edge)
	      9, 12, // sub-node 21 (face)
	      13, 14,// sub-node 22 (face)
	      10, 18,// sub-node 23 (face)
	      11, 19,// sub-node 24 (face)
	      15, 16,// sub-node 25 (face)
	      17, 20,// sub-node 26 (face)
	      21, 26 // sub-node 27 (cell)
	    };

	  for(int i = 0; i < 19; ++i)
	    {
	      double* barycenter = new double[3];
	      calcBarycenter<2>(barycenter, &GENERAL_48_SUB_NODES[2*i]);
	      _nodes.push_back(barycenter);
	    }
	}
	break;
       
      default:
	break;
      }
  }	

  double IntersectorHexa::intersectSourceCell(int srcCell)
  {
    double volume = 0.0;
    for(vector<IntersectorTetra*>::iterator iter = _tetra.begin(); iter != _tetra.end(); ++iter)
      {
	volume += (*iter)->intersectSourceCell(srcCell);
      }
    return volume;
  }


};
