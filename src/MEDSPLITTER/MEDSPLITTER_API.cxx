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

using namespace std;
/*!
 * MEDSPLITTER high-level API
 * 
 * \param inputfilename name of the input file
 * \param mesh name of the input mesh (only used for the sequential input file case)
 * \param outputfilename name out the master output file
 * \param is_distributed sequential input (0) or distributed input (1)
 * \param nprocs number of subdomains
 * \param method METIS(0) or SCOTCH(1)
 * \param meshonly projects mesh and fields (0) or only the mesh (1) 
 */
 
int medsplitter(const char* inputfilename, 
			    const char* mesh,
			    const char* outputfilename,  
			    int is_distributed,
			    int nprocs,
			    int method,
			    int meshonly)
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
  	new_topo = collection->createPartition(nprocs,MEDSPLITTER::Graph::METIS);
  else
  	new_topo = collection->createPartition(nprocs,MEDSPLITTER::Graph::SCOTCH);
  
  // Creating a new mesh collection from the partitioning 
  MEDSPLITTER::MESHCollection new_collection(*collection, new_topo);
 
 //Writing the output files (master + MED files)
 string output(outputfilename);
  new_collection.write(output);
  
  // Casting the fields on the new collection
  if (meshonly!=0)
  new_collection.castAllFields(*collection);
  delete collection;
 
  return 0;
}