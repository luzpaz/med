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

/*!
 * API for the MEDSPLITTER tool 
 * 
 * API has a C binding so that it can be used
 * in a C/C++ program and it can easily be wrapped 
 * in a Fortran program
 */
 
#include "MEDSPLITTER_API.hxx"

#include <string>
#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDSPLITTER_Graph.hxx"
#include "MEDSPLITTER_MESHCollection.hxx"
#include "MEDSPLITTER_Topology.hxx"
#include "MEDSPLITTER_ParaDomainSelector.hxx"

using namespace std;
/*!
 * MEDSPLITTER high-level API
 * 
 * \param inputfilename name of the input file
 * \param mesh name of the input mesh (only used for the sequential input file case)
 * \param outputfilename name out the master output file
 * \param is_distributed sequential input (0) or distributed input (1)
 * \param nbdomains number of subdomains
 * \param method METIS(0) or SCOTCH(1)
 * \param meshonly projects mesh and fields (0) or only the mesh (1) 
 * \param plainmaster creates a plain masterfile (1) instead of an XML file (0),
 * \param createboundaryfaces creates the necessary faces so that face joints are created
 * \param familysplitting preserves the family names (1) instead of focusing on the groups (0)
 * \param emptygroups creates empty groups in zones that do not contain a group from the original domain
*/
 
int medsplitter(const char* inputfilename, 
                const char* mesh,
                const char* outputfilename,  
                int is_distributed,
                int nbdomains,
                int method,
                int meshonly,
                int plainmaster,
                int createboundaryfaces,
                int familysplitting,
                int emptygroups)
{                        

  //Pointer to the initial collection
  MEDSPLITTER::MESHCollection* collection; 

  // Loading the mesh collection
  string input(inputfilename);

  if (is_distributed ==0)
  {
    string meshname (mesh);
    collection=new MEDSPLITTER::MESHCollection(input,meshname);
  }
  else
    collection = new MEDSPLITTER::MESHCollection(input);

  // Creating the graph and partitioning it
  MEDSPLITTER::Topology* new_topo;
  if (method==0)
    new_topo = collection->createPartition(nbdomains,MEDSPLITTER::Graph::METIS);
  else
    new_topo = collection->createPartition(nbdomains,MEDSPLITTER::Graph::SCOTCH);
  
  // Creating a new mesh collection from the partitioning 
  MEDSPLITTER::MESHCollection new_collection(*collection, new_topo, familysplitting, emptygroups);
 
  //Writing the output files (master + MED files)
  if (plainmaster)
    new_collection.setDriverType(MEDSPLITTER::MedAscii);
  new_collection.setSubdomainBoundaryCreates(createboundaryfaces);
  string output(outputfilename);
  new_collection.write(output);

  // Casting the fields on the new collection
  if (meshonly!=0)
    new_collection.castAllFields(*collection);
  delete collection;

  return 0;
}


/*!
 * Parallel MEDSPLITTER high-level API
 * 
 * \param inputfilename name of the input distributed MED file
 * \param outputfilename name out the master output file
 * \param nprocs number of subdomains
 * \param method METIS(0) or SCOTCH(1)
 * \param create_boundary_faces creates the necessary faces so that faces joints are created in the output files
 * \param family_splitting preserves the family names instead of focusing on the groups
 */

int medsplitter_para(const char* inputfilename, 
                     const char* outputfilename,  
                     const int   nprocs,
                     const int   method,
                     const       bool create_boundary_faces,
                     const       bool family_splitting)
{
  // Parallelizer
  MEDSPLITTER::ParaDomainSelector parallelizer;

  // Loading the mesh collection
  MEDSPLITTER::MESHCollection collection (inputfilename, parallelizer);

  // Creating the graph and partitioning it   
  auto_ptr<MEDSPLITTER::Topology> new_topo;
  if (method==0)
    new_topo.reset( collection.createPartition(nprocs,MEDSPLITTER::Graph::METIS));
  else
    new_topo.reset( collection.createPartition(nprocs,MEDSPLITTER::Graph::SCOTCH));

  // Creating a new mesh collection from the partitioning 
  MEDSPLITTER::MESHCollection new_collection(collection, new_topo.get(),family_splitting);
  new_collection.setSubdomainBoundaryCreates(create_boundary_faces);

  //Writing the output files (master + MED files)
  string output(outputfilename);
  new_collection.write(output);

  return 0;
}
