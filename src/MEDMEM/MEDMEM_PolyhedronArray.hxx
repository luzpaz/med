# ifndef __POLYHEDRONARRAY_H__
# define __POLYHEDRONARRAY_H__

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_PointerOf.hxx"
#include "MEDMEM_define.hxx"
using MED_EN::med_int;

namespace MEDMEM {
class POLYHEDRONARRAY
{
private :
  med_int   _numberOfPolyhedron;
  med_int   _numberOfFaces;
  med_int   _numberOfNodes;
  PointerOf <med_int>   _polyhedronIndex;
  PointerOf <med_int>   _facesIndex;
  PointerOf <med_int>   _nodes;

public :
  POLYHEDRONARRAY();
  POLYHEDRONARRAY(med_int numberofpolyhedron,med_int numberoffaces,med_int numberofnodes);

  inline  med_int   getNumberOfPolyhedron()     const;
  inline  med_int   getNumberOfFaces()          const;
  inline  med_int   getNumberOfNodes()          const;
  inline  const med_int*   getPolyhedronIndex()    const;
  inline  const med_int*   getFacesIndex()         const;
  inline  const med_int*   getNodes()              const;

  inline  void      setPolyhedronIndex(const med_int* polyhedronindex);
  inline  void      setFacesIndex(const med_int* facesindex);
  inline  void      setNodes(const med_int* nodes);
};
}

using namespace MEDMEM;
// ----------------------------------------------------------
//                       Methodes Inline
// ----------------------------------------------------------
inline med_int POLYHEDRONARRAY::getNumberOfPolyhedron() const
{
  return _numberOfPolyhedron;
};
inline med_int POLYHEDRONARRAY::getNumberOfFaces() const
{
  return _numberOfFaces;
};
inline med_int POLYHEDRONARRAY::getNumberOfNodes() const
{
  return _numberOfNodes;
};
inline const med_int* POLYHEDRONARRAY::getPolyhedronIndex() const
{
  return (const med_int*) _polyhedronIndex;
};
inline const med_int* POLYHEDRONARRAY::getFacesIndex() const
{
  return (const med_int*) _facesIndex;
};
inline const med_int* POLYHEDRONARRAY::getNodes() const
{
  return (const med_int*) _nodes;
};


inline void POLYHEDRONARRAY::setPolyhedronIndex(const med_int* polyhedronindex)
{
  _polyhedronIndex.set(_numberOfPolyhedron, polyhedronindex);
}
inline void POLYHEDRONARRAY::setFacesIndex(const med_int* facesindex)
{
  _facesIndex.set(_numberOfFaces, facesindex);
}
inline void POLYHEDRONARRAY::setNodes(const med_int* nodes)
{
  _nodes.set(_numberOfNodes, nodes);
}


# endif
