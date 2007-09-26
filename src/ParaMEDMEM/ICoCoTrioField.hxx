#ifndef _ICOCOTRIOFIELD_HXX_
#define _ICOCOTRIOFIELD_HXX_

#include <string>
namespace ICoCo
{
	/*!
		\brief structure for coupling Trio codes via the ICoCo interface

		This structure contains all the necessary information 
		for constructing a ParaMEDMEM::ParaFIELD (with the addition of the MPI
		communicator). The ICoCo API specifies two kinds of calls for
		the ICoCo::Field : either with the mesh only or with the entire information (mesh and field).
		This structure can therefore be left without _time, _nb_field_components, _field
		information, which are related to the field values.
	 */
  class TrioField:public Field
    {
    public:
      
      TrioField();

    public:
			int _mesh_dim;
			int _space_dim;
			int _nbnodes;
			int _nodes_per_elem;
			int _nb_elems;
			int _itnumber;
			std::string _name;
			int* _connectivity;
			double* _coords;

			double _time;
			int _nb_field_components;
			double* _field;
    };
};

#endif
