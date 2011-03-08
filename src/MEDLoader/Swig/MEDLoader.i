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

%module MEDLoader

#define MEDCOUPLING_EXPORT
#define MEDLOADER_EXPORT

%include "MEDCoupling.i"

%{
#include "MEDLoader.hxx"
#include "MEDFileMesh.hxx"
#include "MEDFileField.hxx"
#include "MEDLoaderTypemaps.i"

using namespace ParaMEDMEM;
%}

#if SWIG_VERSION >= 0x010329
%template()  std::vector<std::string>;
#endif

%typemap(out) ParaMEDMEM::MEDFileMesh*
{
  $result=convertMEDFileMesh($1,$owner);
}

%newobject MEDLoader::ReadUMeshFromFamilies;
%newobject MEDLoader::ReadUMeshFromGroups;
%newobject MEDLoader::ReadUMeshFromFile;
%newobject MEDLoader::ReadField;
%newobject MEDLoader::ReadFieldCell;
%newobject MEDLoader::ReadFieldNode;
%newobject MEDLoader::ReadFieldGauss;
%newobject MEDLoader::ReadFieldGaussNE;
%newobject ParaMEDMEM::MEDFileMesh::New;
%newobject ParaMEDMEM::MEDFileMesh::getGenMeshAtLevel;
%newobject ParaMEDMEM::MEDFileMesh::getGroupArr;
%newobject ParaMEDMEM::MEDFileMesh::getGroupsArr;
%newobject ParaMEDMEM::MEDFileMesh::getFamilyArr;
%newobject ParaMEDMEM::MEDFileMesh::getFamiliesArr;
%newobject ParaMEDMEM::MEDFileMesh::getNodeGroupArr;
%newobject ParaMEDMEM::MEDFileMesh::getNodeGroupsArr;
%newobject ParaMEDMEM::MEDFileMesh::getNodeFamilyArr;
%newobject ParaMEDMEM::MEDFileMesh::getNodeFamiliesArr;
%newobject ParaMEDMEM::MEDFileUMesh::New;
%newobject ParaMEDMEM::MEDFileUMesh::getCoords;
%newobject ParaMEDMEM::MEDFileUMesh::getGroup;
%newobject ParaMEDMEM::MEDFileUMesh::getGroups;
%newobject ParaMEDMEM::MEDFileUMesh::getFamily;
%newobject ParaMEDMEM::MEDFileUMesh::getFamilies;
%newobject ParaMEDMEM::MEDFileUMesh::getMeshAtLevel;
%newobject ParaMEDMEM::MEDFileUMesh::getLevel0Mesh;
%newobject ParaMEDMEM::MEDFileUMesh::getLevelM1Mesh;
%newobject ParaMEDMEM::MEDFileUMesh::getLevelM2Mesh;
%newobject ParaMEDMEM::MEDFileUMesh::getLevelM3Mesh;
%newobject ParaMEDMEM::MEDFileCMesh::New;

%newobject ParaMEDMEM::MEDFileFields::New;
%newobject ParaMEDMEM::MEDFileFieldMultiTS::New;
%newobject ParaMEDMEM::MEDFileField1TS::New;

class MEDLoader
{
public:
  static void setEpsilonForNodeComp(double val) throw(INTERP_KERNEL::Exception);
  static void setCompPolicyForCell(int val) throw(INTERP_KERNEL::Exception);
  static void setTooLongStrPolicy(int val) throw(INTERP_KERNEL::Exception);
  static void CheckFileForRead(const char *fileName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetMeshNames(const char *fileName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetMeshNamesOnField(const char *fileName, const char *fieldName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetMeshGroupsNames(const char *fileName, const char *meshName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetMeshFamiliesNames(const char *fileName, const char *meshName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetMeshFamiliesNamesOnGroup(const char *fileName, const char *meshName, const char *grpName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetMeshGroupsNamesOnFamily(const char *fileName, const char *meshName, const char *famName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetAllFieldNamesOnMesh(const char *fileName, const char *meshName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetAllFieldNames(const char *fileName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetFieldNamesOnMesh(ParaMEDMEM::TypeOfField type, const char *fileName, const char *meshName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetCellFieldNamesOnMesh(const char *fileName, const char *meshName) throw(INTERP_KERNEL::Exception);
  static std::vector<std::string> GetNodeFieldNamesOnMesh(const char *fileName, const char *meshName) throw(INTERP_KERNEL::Exception);
  static double GetTimeAttachedOnFieldIteration(const char *fileName, const char *fieldName, int iteration, int order) throw(INTERP_KERNEL::Exception);
  %extend
     {
       static PyObject *GetFieldIterations(ParaMEDMEM::TypeOfField type, const char *fileName, const char *meshName, const char *fieldName) throw(INTERP_KERNEL::Exception)
       {
         std::vector< std::pair<int,int> > res=MEDLoader::GetFieldIterations(type,fileName,meshName,fieldName);
         PyObject *ret=PyList_New(res.size());
         int rk=0;
         for(std::vector< std::pair<int,int> >::const_iterator iter=res.begin();iter!=res.end();iter++,rk++)
           {
             PyObject *elt=PyTuple_New(2);
             PyTuple_SetItem(elt,0,SWIG_From_int((*iter).first));
             PyTuple_SetItem(elt,1,SWIG_From_int((*iter).second));
             PyList_SetItem(ret,rk,elt);
           }
         return ret;
       }

       static PyObject *GetAllFieldIterations(const char *fileName, const char *meshName, const char *fieldName) throw(INTERP_KERNEL::Exception)
       {
         std::vector< std::pair< std::pair<int,int>, double> > res=MEDLoader::GetAllFieldIterations(fileName,meshName,fieldName);
         PyObject *ret=PyList_New(res.size());
         int rk=0;
         for(std::vector< std::pair< std::pair<int,int>, double> >::const_iterator iter=res.begin();iter!=res.end();iter++,rk++)
           {
             PyObject *elt=PyTuple_New(3);
             PyTuple_SetItem(elt,0,SWIG_From_int((*iter).first.first));
             PyTuple_SetItem(elt,1,SWIG_From_int((*iter).first.second));
             PyTuple_SetItem(elt,2,SWIG_From_double((*iter).second));
             PyList_SetItem(ret,rk,elt);
           }
         return ret;
       }

       static PyObject *GetCellFieldIterations(const char *fileName, const char *meshName, const char *fieldName) throw(INTERP_KERNEL::Exception)
       {
         std::vector< std::pair<int,int> > res=MEDLoader::GetCellFieldIterations(fileName,meshName,fieldName);
         PyObject *ret=PyList_New(res.size());
         int rk=0;
         for(std::vector< std::pair<int,int> >::const_iterator iter=res.begin();iter!=res.end();iter++,rk++)
           {
             PyObject *elt=PyTuple_New(2);
             PyTuple_SetItem(elt,0,SWIG_From_int((*iter).first));
             PyTuple_SetItem(elt,1,SWIG_From_int((*iter).second));
             PyList_SetItem(ret,rk,elt);
           }
         return ret;
       }
       static PyObject *GetNodeFieldIterations(const char *fileName, const char *meshName, const char *fieldName) throw(INTERP_KERNEL::Exception)
       {
         std::vector< std::pair<int,int> > res=MEDLoader::GetNodeFieldIterations(fileName,meshName,fieldName);
         PyObject *ret=PyList_New(res.size());
         int rk=0;
         for(std::vector< std::pair<int,int> >::const_iterator iter=res.begin();iter!=res.end();iter++,rk++)
           {
             PyObject *elt=PyTuple_New(2);
             PyTuple_SetItem(elt,0,SWIG_From_int((*iter).first));
             PyTuple_SetItem(elt,1,SWIG_From_int((*iter).second));
             PyList_SetItem(ret,rk,elt);
           }
         return ret;
       }
       static PyObject *ReadFieldsOnSameMesh(ParaMEDMEM::TypeOfField type, const char *fileName, const char *meshName, int meshDimRelToMax,
                                             const char *fieldName, PyObject *liIts) throw(INTERP_KERNEL::Exception)
       {
         std::vector<std::pair<int,int> > its=convertTimePairIdsFromPy(liIts);
         std::vector<ParaMEDMEM::MEDCouplingFieldDouble *> res=MEDLoader::ReadFieldsOnSameMesh(type,fileName,meshName,meshDimRelToMax,fieldName,its);
         return convertFieldDoubleVecToPy(res);
       }
       static void WriteUMeshesPartition(const char *fileName, const char *meshName, PyObject *li, bool writeFromScratch) throw(INTERP_KERNEL::Exception)
       {
         std::vector<const ParaMEDMEM::MEDCouplingUMesh *> v=convertUMeshVecFromPy(li);
         MEDLoader::WriteUMeshesPartition(fileName,meshName,v,writeFromScratch);
       }
       static void WriteUMeshesPartitionDep(const char *fileName, const char *meshName, PyObject *li, bool writeFromScratch) throw(INTERP_KERNEL::Exception)
       {
         std::vector<const ParaMEDMEM::MEDCouplingUMesh *> v=convertUMeshVecFromPy(li);
         MEDLoader::WriteUMeshesPartitionDep(fileName,meshName,v,writeFromScratch);
       }
       static void WriteUMeshes(const char *fileName, PyObject *li, bool writeFromScratch) throw(INTERP_KERNEL::Exception)
       {
         std::vector<const ParaMEDMEM::MEDCouplingUMesh *> v=convertUMeshVecFromPy(li);
         MEDLoader::WriteUMeshes(fileName,v,writeFromScratch);
       }
       static PyObject *GetTypesOfField(const char *fileName, const char *fieldName, const char *meshName) throw(INTERP_KERNEL::Exception)
       {
         std::vector< ParaMEDMEM::TypeOfField > v=MEDLoader::GetTypesOfField(fileName,fieldName,meshName);
         int size=v.size();
         PyObject *ret=PyList_New(size);
         for(int i=0;i<size;i++)
           PyList_SetItem(ret,i,PyInt_FromLong((int)v[i]));
         return ret;
       }
       static ParaMEDMEM::MEDCouplingUMesh *ReadUMeshFromGroups(const char *fileName, const char *meshName, int meshDimRelToMax, PyObject *li) throw(INTERP_KERNEL::Exception)
       {
         std::vector<std::string> grps;
         converPyListToVecString(li,grps);
         return MEDLoader::ReadUMeshFromGroups(fileName,meshName,meshDimRelToMax,grps);
       }
       static ParaMEDMEM::MEDCouplingUMesh *ReadUMeshFromFamilies(const char *fileName, const char *meshName, int meshDimRelToMax, PyObject *li) throw(INTERP_KERNEL::Exception)
       {
         std::vector<std::string> fams;
         converPyListToVecString(li,fams);
         return MEDLoader::ReadUMeshFromFamilies(fileName,meshName,meshDimRelToMax,fams);
       }
     }
  static ParaMEDMEM::MEDCouplingUMesh *ReadUMeshFromFile(const char *fileName, const char *meshName, int meshDimRelToMax=0) throw(INTERP_KERNEL::Exception);
  static ParaMEDMEM::MEDCouplingUMesh *ReadUMeshFromFile(const char *fileName, int meshDimRelToMax=0) throw(INTERP_KERNEL::Exception);
  static ParaMEDMEM::MEDCouplingFieldDouble *ReadField(ParaMEDMEM::TypeOfField type, const char *fileName, const char *meshName, int meshDimRelToMax, const char *fieldName, int iteration, int order) throw(INTERP_KERNEL::Exception);
  static ParaMEDMEM::MEDCouplingFieldDouble *ReadFieldCell(const char *fileName, const char *meshName, int meshDimRelToMax, const char *fieldName, int iteration, int order) throw(INTERP_KERNEL::Exception);
  static ParaMEDMEM::MEDCouplingFieldDouble *ReadFieldNode(const char *fileName, const char *meshName, int meshDimRelToMax, const char *fieldName, int iteration, int order) throw(INTERP_KERNEL::Exception);
  static ParaMEDMEM::MEDCouplingFieldDouble *ReadFieldGauss(const char *fileName, const char *meshName, int meshDimRelToMax, const char *fieldName, int iteration, int order) throw(INTERP_KERNEL::Exception);
  static ParaMEDMEM::MEDCouplingFieldDouble *ReadFieldGaussNE(const char *fileName, const char *meshName, int meshDimRelToMax, const char *fieldName, int iteration, int order) throw(INTERP_KERNEL::Exception);
  static void WriteUMesh(const char *fileName, const ParaMEDMEM::MEDCouplingUMesh *mesh, bool writeFromScratch) throw(INTERP_KERNEL::Exception);
  static void WriteUMeshDep(const char *fileName, const ParaMEDMEM::MEDCouplingUMesh *mesh, bool writeFromScratch) throw(INTERP_KERNEL::Exception);
  static void WriteField(const char *fileName, const ParaMEDMEM::MEDCouplingFieldDouble *f, bool writeFromScratch) throw(INTERP_KERNEL::Exception);
  static void WriteFieldDep(const char *fileName, const ParaMEDMEM::MEDCouplingFieldDouble *f, bool writeFromScratch) throw(INTERP_KERNEL::Exception);
  static void WriteFieldUsingAlreadyWrittenMesh(const char *fileName, const ParaMEDMEM::MEDCouplingFieldDouble *f) throw(INTERP_KERNEL::Exception);
};

namespace ParaMEDMEM
{
  class MEDFileMesh : public RefCountObject
  {
  public:
    static MEDFileMesh *New(const char *fileName) throw(INTERP_KERNEL::Exception);
    static MEDFileMesh *New(const char *fileName, const char *mName, int dt=-1, int it=-1);
    virtual void clearNonDiscrAttributes() const;
    void setName(const char *name);
    const char *getName();
    void setUnivName(const char *name);
    const char *getUnivName() const;
    void setDescription(const char *name);
    const char *getDescription() const;
    void setOrder(int order);
    int getOrder() const;
    void setIteration(int it);
    int getIteration();
    void setTimeValue(double time);
    void setTime(double time, int dt, int it);
    double getTimeValue() const;
    void setTimeUnit(const char *unit);
    const char *getTimeUnit() const;
    void write(const char *fileName, int mode) const throw(INTERP_KERNEL::Exception);
    int getSizeAtLevel(int meshDimRelToMaxExt) const throw(INTERP_KERNEL::Exception);
    //
    bool existsFamily(int famId) const;
    bool existsFamily(const char *familyName) const;
    void setFamilyId(const char *familyName, int id);
    void addFamily(const char *familyName, int id) throw(INTERP_KERNEL::Exception);
    void addGrpOnFamily(const char *grpName, const char *famName) throw(INTERP_KERNEL::Exception);
    void copyFamGrpMapsFrom(const MEDFileMesh& other);
    const std::map<std::string,int>& getFamilyInfo() const;
    const std::map<std::string, std::vector<std::string> >& getGroupInfo() const;
    std::vector<std::string> getFamiliesOnGroup(const char *name) const throw(INTERP_KERNEL::Exception);
    std::vector<std::string> getGroupsOnFamily(const char *name) const throw(INTERP_KERNEL::Exception);
    std::vector<std::string> getGroupsNames() const;
    std::vector<std::string> getFamiliesNames() const;
    void removeGroup(const char *name) throw(INTERP_KERNEL::Exception);
    void removeFamily(const char *name) throw(INTERP_KERNEL::Exception);
    void changeGroupName(const char *oldName, const char *newName) throw(INTERP_KERNEL::Exception);
    void changeFamilyName(const char *oldName, const char *newName) throw(INTERP_KERNEL::Exception);
    void setFamilyInfo(const std::map<std::string,int>& info);
    void setGroupInfo(const std::map<std::string, std::vector<std::string> >&info);
    int getFamilyId(const char *name) const throw(INTERP_KERNEL::Exception);
    int getMaxFamilyId() const throw(INTERP_KERNEL::Exception);
    std::vector<int> getFamiliesIds(const std::vector<std::string>& famNames) const throw(INTERP_KERNEL::Exception);
    std::string getFamilyNameGivenId(int id) const throw(INTERP_KERNEL::Exception);
    //
    virtual MEDCouplingMesh *getGenMeshAtLevel(int meshDimRelToMax) const throw(INTERP_KERNEL::Exception);
    virtual void setFamilyFieldArr(int meshDimRelToMaxExt, DataArrayInt *famArr) throw(INTERP_KERNEL::Exception);
    virtual void setRenumFieldArr(int meshDimRelToMaxExt, DataArrayInt *renumArr) throw(INTERP_KERNEL::Exception);
    virtual DataArrayInt *getFamiliesArr(int meshDimRelToMaxExt, const std::vector<std::string>& fams, bool renum=false) const throw(INTERP_KERNEL::Exception);
    virtual DataArrayInt *getGroupsArr(int meshDimRelToMaxExt, const std::vector<std::string>& grps, bool renum=false) const throw(INTERP_KERNEL::Exception);
    virtual DataArrayInt *getGroupArr(int meshDimRelToMaxExt, const char *grp, bool renum=false) const throw(INTERP_KERNEL::Exception);
    virtual DataArrayInt *getFamilyArr(int meshDimRelToMaxExt, const char *fam, bool renum=false) const throw(INTERP_KERNEL::Exception);
    virtual DataArrayInt *getNodeGroupArr(const char *grp, bool renum=false) const throw(INTERP_KERNEL::Exception);
    virtual DataArrayInt *getNodeGroupsArr(const std::vector<std::string>& grps, bool renum=false) const throw(INTERP_KERNEL::Exception);
    virtual DataArrayInt *getNodeFamilyArr(const char *fam, bool renum=false) const throw(INTERP_KERNEL::Exception);
    virtual DataArrayInt *getNodeFamiliesArr(const std::vector<std::string>& fams, bool renum=false) const throw(INTERP_KERNEL::Exception);
    %extend
       {
         PyObject *getTime()
         {
           int tmp1,tmp2;
           double tmp0=self->getTime(tmp1,tmp2);
           PyObject *res = PyList_New(3);
           PyList_SetItem(res,0,SWIG_From_double(tmp0));
           PyList_SetItem(res,1,SWIG_From_int(tmp1));
           PyList_SetItem(res,2,SWIG_From_int(tmp2));
           return res;
         }

         virtual PyObject *isEqual(const MEDFileMesh *other, double eps) const
         {
           std::string what;
           bool ret0=self->isEqual(other,eps,what);
           PyObject *res=PyList_New(2);
           PyObject *ret0Py=ret0?Py_True:Py_False;
           Py_XINCREF(ret0Py);
           PyList_SetItem(res,0,ret0Py);
           PyList_SetItem(res,1,PyString_FromString(what.c_str()));
           return res;
         }
         
         PyObject *areFamsEqual(const MEDFileMesh *other) const
         {
           std::string what;
           bool ret0=self->areFamsEqual(other,what);
           PyObject *res=PyList_New(2);
           PyObject *ret0Py=ret0?Py_True:Py_False;
           Py_XINCREF(ret0Py);
           PyList_SetItem(res,0,ret0Py);
           PyList_SetItem(res,1,PyString_FromString(what.c_str()));
           return res;
         }

         PyObject *areGrpsEqual(const MEDFileMesh *other) const
         {
           std::string what;
           bool ret0=self->areGrpsEqual(other,what);
           PyObject *res=PyList_New(2);
           PyObject *ret0Py=ret0?Py_True:Py_False;
           Py_XINCREF(ret0Py);
           PyList_SetItem(res,0,ret0Py);
           PyList_SetItem(res,1,PyString_FromString(what.c_str()));
           return res;
         }

         PyObject *getFamilyFieldAtLevel(int meshDimRelToMaxExt) const throw(INTERP_KERNEL::Exception)
         {
           const DataArrayInt *tmp=self->getFamilyFieldAtLevel(meshDimRelToMaxExt);
           if(tmp)
             tmp->incrRef();
           return SWIG_NewPointerObj(SWIG_as_voidptr(tmp),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
         }

         PyObject *getNumberFieldAtLevel(int meshDimRelToMaxExt) const throw(INTERP_KERNEL::Exception)
         {
           const DataArrayInt *tmp=self->getNumberFieldAtLevel(meshDimRelToMaxExt);
           if(tmp)
             tmp->incrRef();
           return SWIG_NewPointerObj(SWIG_as_voidptr(tmp),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
         }
       }
  };

  class MEDFileUMesh : public MEDFileMesh
  {
  public:
    static MEDFileUMesh *New(const char *fileName, const char *mName, int dt=-1, int it=-1) throw(INTERP_KERNEL::Exception);
    static MEDFileUMesh *New(const char *fileName) throw(INTERP_KERNEL::Exception);
    static MEDFileUMesh *New();
    ~MEDFileUMesh();
    //
    int getMeshDimension() const;
    std::vector<int> getNonEmptyLevels() const;
    std::vector<int> getNonEmptyLevelsExt() const;
    DataArrayDouble *getCoords() const;
    MEDCouplingUMesh *getGroup(int meshDimRelToMaxExt, const char *grp, bool renum=false) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *getGroupArr(int meshDimRelToMaxExt, const char *grp, bool renum=false) const throw(INTERP_KERNEL::Exception);
    MEDCouplingUMesh *getGroups(int meshDimRelToMaxExt, const std::vector<std::string>& grps, bool renum=false) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *getGroupsArr(int meshDimRelToMaxExt, const std::vector<std::string>& grps, bool renum=false) const throw(INTERP_KERNEL::Exception);
    MEDCouplingUMesh *getFamily(int meshDimRelToMaxExt, const char *fam, bool renum=false) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *getFamilyArr(int meshDimRelToMaxExt, const char *fam, bool renum=false) const throw(INTERP_KERNEL::Exception);
    MEDCouplingUMesh *getFamilies(int meshDimRelToMaxExt, const std::vector<std::string>& fams, bool renum=false) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *getFamiliesArr(int meshDimRelToMaxExt, const std::vector<std::string>& fams, bool renum=false) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *getNodeGroupArr(const char *grp, bool renum=false) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *getNodeGroupsArr(const std::vector<std::string>& grps, bool renum=false) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *getNodeFamilyArr(const char *fam, bool renum=false) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *getNodeFamiliesArr(const std::vector<std::string>& fams, bool renum=false) const throw(INTERP_KERNEL::Exception);
    MEDCouplingUMesh *getMeshAtLevel(int meshDimRelToMaxExt, bool renum=false) const throw(INTERP_KERNEL::Exception);
    MEDCouplingUMesh *getLevel0Mesh(bool renum=false) const throw(INTERP_KERNEL::Exception);
    MEDCouplingUMesh *getLevelM1Mesh(bool renum=false) const throw(INTERP_KERNEL::Exception);
    MEDCouplingUMesh *getLevelM2Mesh(bool renum=false) const throw(INTERP_KERNEL::Exception);
    MEDCouplingUMesh *getLevelM3Mesh(bool renum=false) const throw(INTERP_KERNEL::Exception);
    //
    void setFamilyNameAttachedOnId(int id, const std::string& newFamName) throw(INTERP_KERNEL::Exception);
    void setCoords(DataArrayDouble *coords) throw(INTERP_KERNEL::Exception);
    void eraseGroupsAtLevel(int meshDimRelToMaxExt) throw(INTERP_KERNEL::Exception);
    void setFamilyField(DataArrayInt *arr, const std::vector< std::vector< int > > &userfids, const std::vector<std::string>& grpNames, bool renum=false) throw(INTERP_KERNEL::Exception);
    void addNodeGroup(const std::string& name, const std::vector<int>& ids) throw(INTERP_KERNEL::Exception);
    void setMeshAtLevel(int meshDimRelToMax, MEDCouplingUMesh *m) throw(INTERP_KERNEL::Exception);
    void setMeshAtLevelOld(int meshDimRelToMax, MEDCouplingUMesh *m) throw(INTERP_KERNEL::Exception);
    void setMeshAtLevelGen(int meshDimRelToMax, MEDCouplingUMesh *m, bool newOrOld) throw(INTERP_KERNEL::Exception);
    void setGroupsFromScratch(int meshDimRelToMax, const std::vector<const MEDCouplingUMesh *>& ms) throw(INTERP_KERNEL::Exception);
    void setGroupsOnSetMesh(int meshDimRelToMax, const std::vector<const MEDCouplingUMesh *>& ms, bool renum) throw(INTERP_KERNEL::Exception);
    void optimizeFamilies() throw(INTERP_KERNEL::Exception);
    %extend
       { 
         PyObject *getRevNumberFieldAtLevel(int meshDimRelToMaxExt) const throw(INTERP_KERNEL::Exception)
         {
           const DataArrayInt *tmp=self->getRevNumberFieldAtLevel(meshDimRelToMaxExt);
           if(tmp)
             tmp->incrRef();
           return SWIG_NewPointerObj(SWIG_as_voidptr(tmp),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
         }
         
         void setGroupsAtLevel(int meshDimRelToMaxExt, PyObject *li, bool renum=false) throw(INTERP_KERNEL::Exception)
         {
           std::vector<const DataArrayInt *> grps;
           convertPyObjToVecDataArrayIntCst(li,grps);
           self->setGroupsAtLevel(meshDimRelToMaxExt,grps,renum);
         }
       }
  };

  class MEDFileCMesh : public MEDFileMesh
  {
  public:
    static MEDFileCMesh *New();
    static MEDFileCMesh *New(const char *fileName) throw(INTERP_KERNEL::Exception);
    static MEDFileCMesh *New(const char *fileName, const char *mName, int dt=-1, int it=-1) throw(INTERP_KERNEL::Exception);
    void setMesh(MEDCouplingCMesh *m);
    %extend
       {
         PyObject *getMesh() const
         {
           const MEDCouplingCMesh *tmp=self->getMesh();
           if(tmp)
             tmp->incrRef();
           return SWIG_NewPointerObj(SWIG_as_voidptr(tmp),SWIGTYPE_p_ParaMEDMEM__MEDCouplingCMesh, SWIG_POINTER_OWN | 0 );
         }
       }
  };

  class MEDFieldFieldGlobs
  {
  public:
    std::vector<std::string> getPfls() const;
    std::vector<std::string> getLocs() const;
  };

  class MEDFileField1TSWithoutDAS : public RefCountObject
  {
  public:
    int getIteration() const;
    int getOrder() const;
    std::string getName();
    std::string getMeshName();
    int getNumberOfComponents() const;
    const std::vector<std::string>& getInfos() const;
    std::vector<std::string> getPflsReallyUsed() const;
    std::vector<std::string> getLocsReallyUsed() const;
  };

  class MEDFileField1TS : public MEDFileField1TSWithoutDAS, public MEDFieldFieldGlobs
  {
  public:
    static MEDFileField1TS *New(const char *fileName, const char *fieldName, int iteration, int order) throw(INTERP_KERNEL::Exception);
  };

  class MEDFileFieldMultiTSWithoutDAS
  {
  public:
    int getNumberOfTS() const;
  };

  class MEDFileFieldMultiTS : public MEDFileFieldMultiTSWithoutDAS, public MEDFieldFieldGlobs
  {
  public:
    static MEDFileFieldMultiTS *New(const char *fileName, const char *fieldName) throw(INTERP_KERNEL::Exception);
  };

  class MEDFileFields : public RefCountObject, public MEDFieldFieldGlobs
  {
  public:
    static MEDFileFields *New(const char *fileName) throw(INTERP_KERNEL::Exception);
    int getNumberOfFields() const;
    std::vector<std::string> getPfls() const;
    std::vector<std::string> getLocs() const;
  };
}
