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

#ifndef MEDSPLITTER_FACEMODEL_HXX_
#define MEDSPLITTER_FACEMODEL_HXX_

namespace MEDMEM
{
  class CELLMODEL;
}

namespace MEDSPLITTER
{

  class MEDSPLITTER_FaceModel
  {
  public:
    MEDSPLITTER_FaceModel():_conn(0){};
    virtual ~MEDSPLITTER_FaceModel() {if (_conn!=0) delete[] _conn;}
    MED_EN::medGeometryElement getType() const {return _type;}
    void setType(MED_EN::medGeometryElement type) {_type=type;}
    void setNbNodes(int nbnodes){_conn=new int[nbnodes];}
    int& operator[](int i){return *(_conn+i);}
    int getGlobal(){return _global;}
    void setGlobal(int i){_global=i;}

    // Makes face common for two given cells (implemented in MEDSPLITTER_MESHCollection.cxx)
    static MEDSPLITTER_FaceModel* getCommonFace(const int*               nodes1,
                                                const int*               nodes1_local,
                                                const MEDMEM::CELLMODEL& celltype1,
                                                const int*               nodes2,
                                                int                      nb_nodes2,
                                                int                      global_id);

  private:
    int _nbnodes;
    int* _conn;
    MED_EN::medGeometryElement _type;
    int _global;
  };

}

#endif /*MEDSPLITTER_FACEMODEL_HXX_*/
