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
#ifndef MESHCOLLECTIONDRIVER_HXX_
#define MESHCOLLECTIONDRIVER_HXX_

#include "MEDSPLITTER.hxx"

namespace MEDSPLITTER
{
  class MESHCollection;
  class ParaDomainSelector;

  class MEDSPLITTER_EXPORT MESHCollectionDriver
  {
  public:

    MESHCollectionDriver(MESHCollection*);
    virtual ~MESHCollectionDriver(){}

    virtual int read(char*, ParaDomainSelector* sel=0)=0;
    int readSeq(char*,char*);

    virtual void write(char*, ParaDomainSelector* sel=0)=0;
    virtual void readFields (vector <MEDMEM::FIELD<int> *>& filenames, char* fieldname,
                             int itnumber, int ordernumber) =0;
    virtual void readFields (vector <MEDMEM::FIELD<double> *>& filenames, char* fieldname,
                             int itnumber, int ordernumber) =0;
    virtual void writeFields(vector <MEDMEM::FIELD<int> *>& filenames, char* fieldname)=0;
    virtual void writeFields(vector <MEDMEM::FIELD<double> *>& filenames, char* fieldname)=0;

    void readFileStruct(vector <string>&  field_names,vector<int>& iternumber,vector <int>&  ordernumber,vector <int> & types);

    int getFieldType(const std::string& fieldname);
    //  void exportFamily(vector<int*>,MED_EN::medEntityMesh, const string& name);

    void readLoc2GlobCellConnect(int idomain, const set<int>& loc_domains, ParaDomainSelector* ds,
                                 vector<int> & loc2glob_corr);

    int readMeshDimension() const;

  protected:

    void readSubdomain(vector<int*>& cellglobal,
                       vector<int*>& faceglobal,
                       vector<int*>& nodeglobal, int idomain);
    void writeSubdomain(int idomain,int nbdomain, char*filename,
                        ParaDomainSelector* domain_selector);

    void writeElementJoint(medEntityMesh entity ,
                           int icz, 
                           int idomain, 
                           int idistant, 
                           char* mesh_name, 
                           char* joint_name,  
                           med_2_3::med_idt fid );
    void jointSort(int* elems, int nbelems, bool is_first);



    MESHCollection* _collection;

    std::vector <std::string> _filename;
    std::vector <std::string> _meshname;

  };

}


#endif /*MESHCOLLECTIONDRIVER_HXX_*/
