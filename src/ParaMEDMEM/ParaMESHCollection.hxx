#ifndef PARAMESHCOLLECTION_HXX_
#define PARAMESHCOLLECTION_HXX_

#include <vector>

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_GenDriver.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_ConnectZone.hxx"
#include "ParaMEDMEM_ProcessorGroup.hxx"

class ParaMESHCollection
{
public:
	ParaMESHCollection();
	ParaMESHCollection(MEDMEM::driverTypes driver_type, const string& file_name, 
		const string& driver_name, const ProcessorGroup& proc_group,
		const int* distribution=NULL)
	throw (MEDMEM::MEDEXCEPTION);
	virtual ~ParaMESHCollection();
	
private :
	vector<ParaMESH*> _meshes;
};

#endif /*PARAMESHCOLLECTION_HXX_*/
