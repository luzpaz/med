#include "ICoCoMEDField.hxx"
#include "ICoCoTrioField.hxx"
#include "ProcessorGroup.hxx"
#include "ParaMESH.hxx"
#include "ParaSUPPORT.hxx"
#include "ParaFIELD.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Meshing.hxx"
#include "MEDMEM_Support.hxx"
#include "MEDMEM_Field.hxx"
namespace ICoCo
{
	MEDField::MEDField(const TrioField& triofield, const ParaMEDMEM::ProcessorGroup& group)
	{
		MEDMEM::MESHING* mesh = new MEDMEM::MESHING();
		string system="CARTESIAN";
		mesh->setCoordinates(triofield._space_dim,
												triofield._nbnodes,
												triofield._coords,
												system,
												MED_EN::MED_FULL_INTERLACE);
		mesh->setNumberOfTypes(1, MED_EN::MED_CELL);
		MED_EN::medGeometryElement elemtype;
		switch (triofield._mesh_dim)
			{
			case 2:
				switch (triofield._nodes_per_elem)
					{
					case 3:
						elemtype=MED_TRIA3;
						break;
					case 4 : 
						elemtype=MED_QUAD4;
						break;
					default:
						throw MEDMEM::MEDEXCEPTION("incompatible Trio field - wrong nb of nodes per elem");
					}
			case 3:
				switch (triofield._nodes_per_elem)
					{
					case 4:
						elemtype=MED_TETRA4;
						break;
					case 8 : 
						elemtype=MED_HEXA8;
						break;
					default:
						throw MEDMEM::MEDEXCEPTION("incompatible Trio field - wrong nb of nodes per elem");
					}
			default:
				throw MEDMEM::MEDEXCEPTION("incompatible Trio field - wrong mesh dimension");
			}
		mesh->setTypes (&elemtype, MED_EN::MED_CELL);
		mesh->setNumberOfElements(&triofield._nb_elems, MED_EN::MED_CELL);
		mesh->setConnectivity(triofield._connectivity, MED_EN::MED_CELL, elemtype);

		_mesh=new ParaMEDMEM::ParaMESH(*mesh, group, "support for trio field");
		ParaMEDMEM::ParaSUPPORT* parasupport=new ParaMEDMEM::ParaSUPPORT(_mesh);
		ParaMEDMEM::ComponentTopology* comptopo=new ParaMEDMEM::ComponentTopology(triofield._nb_field_components);
		_field = new ParaMEDMEM::ParaFIELD(parasupport, *comptopo );
		_field->getField()->setName(triofield._name);
		_field->getField()->setTime(triofield._time);
		_field->getField()->setIterationNumber(triofield._itnumber);
		_field->getField()->setOrderNumber(0);
		for (int i =0; i<triofield._nb_elems; i++)
			for (int j=0; j<triofield._nb_field_components; j++)
			{
				_field->getField()->setValueIJ(i+1,j+1,triofield._field[i*triofield._nb_field_components+j]);
			}
	}
};
