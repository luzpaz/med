#include <mpi.h>
#include "CommInterface.hxx"
#include "Topology.hxx"
#include "BlockTopology.hxx"
#include "ComponentTopology.hxx"
#include "ParaFIELD.hxx"
#include "MPIProcessorGroup.hxx"
#include "DEC.hxx"
#include "NonCoincidentDEC.hxx"

extern "C" {
#include <fvm_parall.h>
#include <fvm_nodal.h>
#include <fvm_nodal_append.h>
#include <fvm_locator.h>
}

namespace ParaMEDMEM
{
  fvm_nodal_t*  medmemMeshToFVMMesh(const MEDMEM::MESH* mesh)
  {
   // create an FVM structure from the paramesh structure
      fvm_nodal_t * fvm_nodal = fvm_nodal_create(mesh->getName().c_str());
      
      //loop on cell types
      int nbtypes = mesh->getNumberOfTypes(MED_EN::MED_CELL);
      const MED_EN::medGeometryElement* types = mesh->getTypes(MED_EN::MED_CELL);
      for (int itype=0; itype<nbtypes; itype++)
	      {
	       fvm_element_t fvm_type;
	       switch (types[itype]) 
         {
	       case MED_EN::MED_TRIA3 :
	        fvm_type=FVM_FACE_TRIA;
      	    break;
      	 case MED_EN::MED_QUAD4 :
      	    fvm_type=FVM_FACE_QUAD;
      	    break;
      	 case MED_EN::MED_TETRA4 :
      	    fvm_type=FVM_CELL_TETRA;
      	    break;
      	 case MED_EN::MED_HEXA8 :
      	    fvm_type=FVM_CELL_HEXA;
      	    break;
      	 default:
      	    throw MEDEXCEPTION(" MED type  conversion to fvm is not handled yet.");
      	    break;

	       }
         fvm_lnum_t nbelems = mesh->getNumberOfElements(MED_EN::MED_CELL, types[itype]);
         fvm_lnum_t* conn = const_cast<fvm_lnum_t*> (mesh->getConnectivity(MED_EN::MED_FULL_INTERLACE,MED_EN::MED_NODAL, MED_EN::MED_CELL, types[itype]));
       
         fvm_nodal_append_by_transfer(fvm_nodal, nbelems, fvm_type,0,0,0,conn,0);

       }
     return fvm_nodal;
  }
  
  fvm_nodal_t*  medmemSupportToFVMMesh(const MEDMEM::SUPPORT* support)
  {

   // create an FVM structure from the paramesh structure
      fvm_nodal_t * fvm_nodal = fvm_nodal_create(support->getName().c_str());
      
      const MEDMEM::MESH* mesh= support->getMesh();
      
      //loop on cell types
      MED_EN::medEntityMesh entity = support->getEntity();
      
      int nbtypes = support->getNumberOfTypes();
      const MED_EN::medGeometryElement* types = support->getTypes();
      int ioffset=0;
      const int* type_offset = support->getNumberIndex();
      
      //browsing through all types
      for (int itype=0; itype<nbtypes; itype++)
        {
         fvm_element_t fvm_type;
         switch (types[itype]) 
         {
         case MED_EN::MED_TRIA3 :
          fvm_type=FVM_FACE_TRIA;
            break;
         case MED_EN::MED_QUAD4 :
            fvm_type=FVM_FACE_QUAD;
            break;
         case MED_EN::MED_TETRA4 :
            fvm_type=FVM_CELL_TETRA;
            break;
         case MED_EN::MED_HEXA8 :
            fvm_type=FVM_CELL_HEXA;
            break;
         default:
            throw MEDEXCEPTION(" MED type  conversion to fvm is not handled yet.");
            break;

         }
         fvm_lnum_t nbelems = support->getNumberOfElements(types[itype]);
         
         //for a partial support, defining the element numbers that are taken into
         //account in the support
         fvm_lnum_t* elem_numbers=0;
         if (!support->isOnAllElements())
         {
           elem_numbers = const_cast<fvm_lnum_t*> (support->getNumber(types[itype]));
           
           //creating work arrays to store list of elems for partial suports
           if (itype>0)
           {
             fvm_lnum_t* temp = new int[nbelems];
             for (int i=0; i< nbelems; i++)
              temp[i] = elem_numbers [i]-ioffset;
             ioffset+=type_offset[itype];
             elem_numbers = temp;
           }
         }
         //retrieving original mesh connectivity
         fvm_lnum_t* conn = const_cast<fvm_lnum_t*> (mesh->getConnectivity(MED_EN::MED_FULL_INTERLACE,MED_EN::MED_NODAL,entity, types[itype]));
       
        // adding the elements to the FVM structure 
         fvm_nodal_append_by_transfer(fvm_nodal, nbelems, fvm_type,0,0,0,conn,elem_numbers);
         
         //cleaning work arrays (for partial supports)
         if (!support->isOnAllElements() && itype>0)
            delete[] elem_numbers;
      
       }
     return fvm_nodal;
  }
  
NonCoincidentDEC::NonCoincidentDEC()
{	
}

NonCoincidentDEC::~NonCoincidentDEC()
{
}

/*! Synchronization process for exchanging topologies
 */
void NonCoincidentDEC::synchronize()
{
  
  //initializing FVM parallel environment
  fvm_parall_set_mpi_comm(MPI_COMM_WORLD);
  
  
  //setting up the communication DEC on both sides
  
  if (_source_field!=0)
    {
      MEDMEM::MESH* mesh = _source_field->getField()->getSupport()->getMesh();
      fvm_nodal_t* source_nodal = ParaMEDMEM::medmemMeshToFVMMesh(mesh);
      
      const ProcessorGroup* proc_group = _source_field->getTopology()->getProcGroup();
      int target_size = proc_group->getCommInterface().worldSize() -
	       proc_group->size()	;
      int start_rank=	proc_group->size();
      const MPI_Comm* comm = (dynamic_cast<const MPIProcessorGroup*> (proc_group))->getComm();
      fvm_locator_t* locator = 
	    fvm_locator_create(1e-6,
			   *comm,
			   target_size,
			   start_rank);
      
      fvm_locator_set_nodal(locator,
			    source_nodal,
			    2,
			    0,
			    0,
			    0);

      
      _nb_distant_points = fvm_locator_get_n_dist_points(locator);
      _distant_coords = fvm_locator_get_dist_coords(locator);
      _distant_locations = fvm_locator_get_dist_locations(locator);
           
  }
  if (_target_field!=0)
    {
      MEDMEM::MESH* mesh = _target_field->getField()->getSupport()->getMesh();
      
      fvm_nodal_t* target_nodal = ParaMEDMEM::medmemMeshToFVMMesh(mesh);
      const ProcessorGroup* proc_group = _target_field->getTopology()->getProcGroup();
      int source_size = proc_group->getCommInterface().worldSize() -proc_group->size()	;
      int start_rank=	0 ;
      const MPI_Comm* comm = (dynamic_cast<const MPIProcessorGroup*> (proc_group))->getComm();
      fvm_locator_t* locator = 
	fvm_locator_create(1e-6,
			   *comm,
			   source_size,
			   start_rank);
      int nbcells = mesh->getNumberOfElements(MED_EN::MED_CELL,MED_EN::MED_ALL_ELEMENTS);
      SUPPORT support(mesh, MED_EN::MED_CELL);
      const double* coords = (mesh->getBarycenter(&support))->getValue();
      fvm_locator_set_nodal(locator,
			    target_nodal,
			    2,
			    nbcells,
			    0,
			    coords);	
    }
}


void NonCoincidentDEC::recvData()
{
  int nbelems = _target_field->getField()->getSupport()->getMesh()->getNumberOfElements(MED_EN::MED_CELL, MED_EN::MED_ALL_ELEMENTS);
  int nbcomp =  _target_field->getField()->getNumberOfComponents();
  double* values = new double [nbelems*nbcomp];
  fvm_locator_exchange_point_var(_locator,
                                 0,
                                 values,
                                 sizeof(double),
                                 nbcomp);
 _target_field->getField()->setValue(values);
}

void NonCoincidentDEC::sendData()
{
  const double* values=_source_field->getField()->getValue();
  int nbcomp = _source_field->getField()->getNumberOfComponents();
  double* distant_values = new double [_nb_distant_points*nbcomp];
  for (int i=0; i<_nb_distant_points; i++)
    for (int j=0; j <nbcomp; j++)
      distant_values[i*nbcomp+j]=values[_distant_locations[i]*nbcomp+j];
	
  fvm_locator_exchange_point_var(_locator,
                                 distant_values,
                                 0,
                                 sizeof(double),
                                 nbcomp);
}
	
}



