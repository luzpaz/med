#include <mpi.h>
#include "CommInterface.hxx"
#include "Topology.hxx"
#include "BlockTopology.hxx"
#include "ComponentTopology.hxx"
#include "ParaFIELD.hxx"
#include "MPIProcessorGroup.hxx"
#include "ParaMESH.hxx"
#include "ParaSUPPORT.hxx"
#include "DEC.hxx"
#include "InterpolationMatrix.hxx"
#include "IntersectionDEC.hxx"
#include "ElementLocator.hxx"


namespace ParaMEDMEM
{
    
IntersectionDEC::IntersectionDEC()
{	
}

IntersectionDEC::IntersectionDEC(ProcessorGroup& local_group, ProcessorGroup& distant_group):
  DEC(local_group, distant_group),_interpolation_matrix(0)
{
}

IntersectionDEC::~IntersectionDEC()
{
  if (_interpolation_matrix !=0)
    delete _interpolation_matrix;
 
}

/*! Synchronization process for exchanging topologies
 */
void IntersectionDEC::synchronize()
{
	const ParaMEDMEM::ParaMESH* para_mesh = _local_field->getSupport()->getMesh();
	cout <<"size of Interpolation Matrix"<<sizeof(InterpolationMatrix)<<endl;
	_interpolation_matrix = new InterpolationMatrix (*para_mesh, *_source_group,*_target_group,"P0"); 
	
  //setting up the communication DEC on both sides  
  if (_source_group->containsMyRank())
    {
      //locate the distant meshes
      ElementLocator locator(*para_mesh, *_target_group);
			
      MESH* distant_mesh=0; 
      int* distant_ids=0;
      for (int i=0; i<_target_group->size(); i++)
				{
					//	int idistant_proc = (i+_source_group->myRank())%_target_group->size();
					int	idistant_proc=i;
					
					//gathers pieces of the target meshes that can intersect the local mesh
					locator.exchangeMesh(idistant_proc,distant_mesh,distant_ids);
					
					if (distant_mesh !=0)
						{
							//adds the contribution of the distant mesh on the local one
							int idistant_proc_in_union=_union_group->translateRank(_target_group,idistant_proc);
							cout <<"add contribution from proc "<<idistant_proc_in_union<<" to proc "<<_union_group->myRank()<<endl;
							_interpolation_matrix->addContribution(*distant_mesh,idistant_proc_in_union,distant_ids);
							
							delete distant_mesh;
							delete[] distant_ids;
							distant_mesh=0;
							distant_ids=0;
						}
				}  
			
		}
	
  if (_target_group->containsMyRank())
    {
      ElementLocator locator(*para_mesh, *_source_group);
      MESH* distant_mesh=0;
      int* distant_ids=0;
      for (int i=0; i<_source_group->size(); i++)
				{
					//	int idistant_proc = (i+_target_group->myRank())%_source_group->size();
					int  idistant_proc=i;
					//gathers pieces of the target meshes that can intersect the local mesh
					locator.exchangeMesh(idistant_proc,distant_mesh,distant_ids);
					cout << " Data sent from "<<_union_group->myRank()<<" to source proc "<< idistant_proc<<endl;
					if (distant_mesh!=0)
						{
							delete distant_mesh;
							delete[] distant_ids;
							distant_mesh=0;
							distant_ids=0;
						}
				}      
		}
  _interpolation_matrix->prepare();
	
}


void IntersectionDEC::recvData()
{
	if (_source_group->containsMyRank())
		_interpolation_matrix->transposeMultiply(*_local_field->getField());
	else if (_target_group->containsMyRank())
		{
			_interpolation_matrix->multiply(*_local_field->getField());
			if (_forced_renormalization_flag)
				for (int icomp=0; icomp<_local_field->getField()->getNumberOfComponents(); icomp++)
					{
						double total_norm = _local_field->getVolumeIntegral(icomp+1);
						double source_norm=total_norm;
						_comm_interface->broadcast(&source_norm, 1, MPI_DOUBLE, 0,* dynamic_cast<MPIProcessorGroup*>(_union_group)->getComm());
						
						if (_target_group->containsMyRank() && abs(total_norm)>1e-100)
							_local_field->getField()->applyLin(source_norm/total_norm,0.0,icomp+1);
					}
		}
	
	
}

void IntersectionDEC::sendData()
{
	if (_source_group->containsMyRank())
		{
			_interpolation_matrix->multiply(*_local_field->getField());
			if (_forced_renormalization_flag)
				for (int icomp=0; icomp<_local_field->getField()->getNumberOfComponents(); icomp++)
					{
						double total_norm = _local_field->getVolumeIntegral(icomp+1);
						double source_norm = total_norm;
						_comm_interface->broadcast(&source_norm, 1, MPI_DOUBLE, 0,* dynamic_cast<MPIProcessorGroup*>(_union_group)->getComm());
						
						if (_target_group->containsMyRank() && abs(total_norm)>1e-100)
							_local_field->getField()->applyLin(source_norm/total_norm,0.0,icomp+1);
					}
		}
	else if (_target_group->containsMyRank())
		_interpolation_matrix->transposeMultiply(*_local_field->getField());
}


	
}



