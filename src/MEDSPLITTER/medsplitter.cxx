//  MED medsplitter : tool to split n MED files into p separate 
//                    MED files with a partitioning specified 
//                    by an external tool
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : medsplitter.cxx
//  Author : Vincent BERGEAUD (CEA-DEN/DANS/DM2S/SFME/LGLS)
//  Module : MED

#include <boost/program_options.hpp>
namespace po=boost::program_options;

#include <string>
#include <fstream>

#include "MEDMEM_define.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Family.hxx"
#include "MEDSPLITTER_Graph.hxx"
#include "MEDSPLITTER_MESHCollection.hxx"
#include "MEDSPLITTER_Topology.hxx"

using namespace std;



int main(int argc, char** argv)
{
	
	// Defining options
	// by parsing the command line
	int ndomains;
	string library;
	po::options_description desc("Available options");
	desc.add_options()
  	("help","produces this help message")
  	("mesh-only","prevents the splitter from creating the fields contained in the original file(s)")
  	("distributed","specifies that the input file is distributed")
  	("input-file",po::value<string>(),"name of the input MED file")
  	("output-file",po::value<string>(),"name of the resulting file")
  	("meshname",po::value<string>(),"name of the input mesh")
  	("split-method",po::value<string>(&library)->default_value("metis"),"name of the splitting library (metis,scotch)")
  	("ndomains",po::value<int>(&ndomains)->default_value(1),"number of subdomains in the output file")
		("plain-master","creates a plain masterfile instead of an XML file")
    ("creates-boundary-faces","creates the necessary faces so that faces joints are created in the output files");
  	;
  	
  po::variables_map vm;
  po::store(po::parse_command_line(argc,argv,desc),vm);
  po::notify(vm);
  
  if (vm.count("help")) {
  	cout<<desc<<"\n";
  	return 1;
  }
  if (!vm.count("ndomains"))
  {
    cout << "ndomains must be specified !"<<endl;
    return 1;
  }
    
  
  ndomains = vm["ndomains"].as<int>();
  if (!vm.count("input-file") || !vm.count("output-file"))
  	{
  		cout << "input-file and output-file names must be specified"<<endl;
  		return 1;
  	}
  
  if (!vm.count("distributed") && !vm.count("meshname") )
  {
    cout << "MEDSPLITTER : for a serial MED file, mesh name must be selected with --meshname=..."<<endl;
    return 1;
  }
  string input = vm["input-file"].as<string>();
  string output = vm["output-file"].as<string>();
 
 
 //testing whether it is possible to write a file at the specified location
  string outputtest = output + ".testioms.";
  ofstream testfile(outputtest.c_str());
  if (testfile.fail())
  { 
    cout << "MEDSPLITTER : output-file directory does not exist or is in read-only access" << endl;
    return 1;
  };
  //deletes test file
  remove(outputtest.c_str());
    
  bool is_sequential = true;
 
  if (vm.count("distributed"))
	  is_sequential=false;

  bool mesh_only=false;
  if (vm.count("mesh-only"))
		mesh_only=true;

	bool xml_output_master=true;
	if (vm.count("plain-master"))
		xml_output_master=false;
    
  bool creates_boundary_faces=false;  
  if (vm.count("creates-boundary-faces"))
    creates_boundary_faces=true;
  
  MEDSPLITTER::MESHCollection* collection;
  
  
  // Beginning of the computation
  
  // Loading the mesh collection
	cout << "MEDSPLITTER - reading input files "<<endl;
  if (is_sequential)
    {
      string meshname = vm["meshname"].as<string>();
      collection=new MEDSPLITTER::MESHCollection(input,meshname);
    }
  else
		collection = new MEDSPLITTER::MESHCollection(input);
  	  
	cout << "MEDSPLITTER - computing partition "<<endl;

  // Creating the graph and partitioning it	  
  MEDSPLITTER::Topology* new_topo;
  if (library == "metis")
  	new_topo = collection->createPartition(ndomains,MEDSPLITTER::Graph::METIS);
  else
  	new_topo = collection->createPartition(ndomains,MEDSPLITTER::Graph::SCOTCH);
  
	cout << "MEDSPLITTER - creating new meshes"<<endl;

  // Creating a new mesh collection from the partitioning
  MEDSPLITTER::MESHCollection new_collection(*collection, new_topo);
	if (!xml_output_master)
		new_collection.setDriverType(MEDSPLITTER::MedAscii);
  
  new_collection.setSubdomainBoundaryCreates(creates_boundary_faces);
  
	cout << "MEDSPLITTER - writing output files "<<endl;
  new_collection.write(output);

  // Casting the fields on the new collection
  if (!mesh_only)
		new_collection.castAllFields(*collection);

	// Cleaning memory
  delete collection;
  delete new_topo;

  return 0;
}
