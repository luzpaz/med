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

%module libMEDSPLITTER_Swig

%{
#include "MEDMEM_Mesh.hxx"
#include<MEDSPLITTER_Topology.hxx>
#include<MEDSPLITTER_MESHCollection.hxx>

using namespace MEDSPLITTER;

typedef enum {METIS,SCOTCH} splitter_type;
%}

typedef enum {METIS,SCOTCH} splitter_type;
typedef enum{MedAscii, MedXML, Undefined} DriverType; 

%include "my_typemap.i"

%typecheck(SWIG_TYPECHECK_POINTER) const int* {
   $1 = PyList_Check($input) ? 1 : 0;
}

namespace MEDSPLITTER {
class Graph {
public:
  typedef enum {METIS,SCOTCH} splitter_type;
  %extend {
    Graph() {return 0;}
  }
};
}

class MESHCollection {
public:
  MESHCollection(const MESHCollection&, MEDSPLITTER::Topology*,
                 bool family_splitting=false, bool create_empty_groups=false);
  MESHCollection(const char* filename);
  MESHCollection(const char* filename, char* meshname);

  MEDSPLITTER::Topology* createPartition(int nbdomain,
                                         MEDSPLITTER::Graph::splitter_type splitter_type = METIS, 
                                         const char* theStr=0);
  MEDSPLITTER::Topology* createPartition(const int* partition);

  void setDriverType(::DriverType type);
  void setSubdomainBoundaryCreates(bool flag);

  void write(const char* filename);
  void castAllFields(const MESHCollection& old_collection);
  void castFamilies(const MESHCollection& old_collection);
  void castField(const MESHCollection& old_collection, const char* fieldname, 
		 int itnumber, int ordernumber);

  void setIndivisibleGroup(const std::string& a);
};
