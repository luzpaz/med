//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D
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

#ifndef __MEDFILEMESHELT_HXX__
#define __MEDFILEMESHELT_HXX__

#include "MEDCouplingMemArray.hxx"
#include "MEDCouplingAutoRefCountObjectPtr.hxx"

#include "NormalizedUnstructuredMesh.hxx"

extern "C"
{
#include "med.h"
}

namespace ParaMEDMEM
{
  class MEDCouplingUMesh;

  class MEDFileUMeshPerType : public RefCountObject
  {
  public:
    static MEDFileUMeshPerType *New(med_idt fid, const char *mName, int mdim, med_geometrie_element geoElt, INTERP_KERNEL::NormalizedCellType geoElt2);
    static bool isExisting(med_idt fid, const char *mName, med_geometrie_element geoElt, med_entite_maillage& whichEntity);
    int getDim() const;
    const DataArrayInt *getNodal() const { return _conn; }
    const DataArrayInt *getNodalIndex() const { return _conn_index; }
    const DataArrayInt *getFam() const { return _fam; }
    const DataArrayInt *getNum() const { return _num; }
    static void write(med_idt fid, const char *mname, int mdim, const MEDCouplingUMesh *m, const DataArrayInt *fam, const DataArrayInt *num);
  private:
    MEDFileUMeshPerType(med_idt fid, const char *mName, int mdim, med_geometrie_element geoElt, INTERP_KERNEL::NormalizedCellType type,
                        med_entite_maillage entity);
    void loadFromStaticType(med_idt fid, const char *mName, int mdim, int curNbOfElem, med_geometrie_element geoElt, INTERP_KERNEL::NormalizedCellType type,
                            med_entite_maillage entity);
    void loadPolyg(med_idt fid, const char *mName, int mdim, int curNbOfElem, med_geometrie_element geoElt,
                   med_entite_maillage entity);
    void loadPolyh(med_idt fid, const char *mName, int mdim, int curNbOfElem, med_geometrie_element geoElt,
                   med_entite_maillage entity);
  private:
    MEDCouplingAutoRefCountObjectPtr<DataArrayInt> _conn;
    MEDCouplingAutoRefCountObjectPtr<DataArrayInt> _conn_index;
    MEDCouplingAutoRefCountObjectPtr<DataArrayInt> _num;
    MEDCouplingAutoRefCountObjectPtr<DataArrayInt> _fam;
    INTERP_KERNEL::NormalizedCellType _type;
    med_entite_maillage _entity;
  };
}

#endif