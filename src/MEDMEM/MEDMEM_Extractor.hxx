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

// File      : MEDMEM_Extractor.hxx
// Created   : Thu Dec 18 10:52:01 2008
// Author    : Edward AGAPOV (eap)
//
#ifndef __MEDMEM_EXTRACTOR_HXX__
#define __MEDMEM_EXTRACTOR_HXX__

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Field.hxx"

#include <map>
#include <vector>
#include <set>

namespace MEDMEM
{
  /*!
   * \brief Tool taking an input field and making a field of lower dimension
   * by cutting the given field by a plane or a line.
   *
   * The following extractions are possible: <ul>
   *<li>  3D -> 2D (3D mesh cut by plane)</li>
   *<li>  3D -> 1D (3D mesh cut by line)</li>
   *<li>  2D -> 1D (2D mesh cut by line in 2D space or by plane in 3D space)</li></ul>
   */
  class MEDMEM_EXPORT Extractor
  {
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
  public:
    Extractor(const FIELD<double>& inputField) throw (MEDEXCEPTION);
    ~Extractor();
    /*!
     * \brief Creates a field by cutting inputField by a plane
     *  \param coords - give a point to pass through by the plane
     *  \param normal - gives the plane normal
     *  \retval FIELD<double>* - resulting field holding ownership of its support,
     *                           which in its turn holds ownership of its mesh
     */
    FIELD<double>* extractPlane(const double* coords, const double* normal) throw (MEDEXCEPTION);

    /*!
     * \brief Creates a field by cutting inputField by a line
     *  \param coords - give a point to pass through by the line
     *  \param direction - gives a vector collinear to the line
     *  \retval FIELD<double>* - resulting field holding ownership of its support,
     *                           which in its turn holds ownership of its mesh
     */
    FIELD<double>* extractLine(const double* coords, const double* direction) throw (MEDEXCEPTION);

  private:

    MESH* divideEdges(const double*       coords,
                      const double*       normal,
                      std::map<int,std::set<int> >& new2oldCells);
                     
    MESH* transfixFaces( const double*       coords,
                         const double*       direction,
                         std::map<int,std::set<int> >& new2oldCells);

    FIELD<double>* makeField( const std::map<int,std::set<int> >& new2oldCells,
                              MESH*                               mesh) const;

    void computeDistanceOfNodes(const double* point, const double* normal);

    void sortNodes( std::map< int, std::vector< int > >& connByNbNodes,
                    const double* nodeCoords,
                    const double* point,
                    const double* normal,
                    const std::list<int> & nbNodesPerPolygon);


    const FIELD<double>* _myInputField; //!<  input field
    const MESH*          _myInputMesh;  //!<  unstructured field support mesh
    std::vector<double>  _myNodeDistance; //!< distance of input nodes to cutting tool
  };
}

#endif
