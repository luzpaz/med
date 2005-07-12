# ifndef INTERPOLATION_HXX
# define INTERPOLATION_HXX

//template < class T> class FIELD;
//template < int > class Wrapper_Nuage;
//template < int > class Wrapper_Noeud;
//template <class ,class ,int ,int > class dTree;

#include <vector>
#include "utilities.h"
#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"

#include "MEDMEM_InterpolationHighLevelObjects.hxx"
#include "MEDMEM_Mesh.hxx"
#include "MEDMEM_Field.hxx"

namespace MEDMEM {
class MESH;

template <int DIMENSION=3> class INTERPOLATION
{
protected:

  FIELD<double> *		  _fromField;
  FIELD<double> *	          _toField;
  MESH   *                        _fromMesh;
  MESH   *                        _toMesh;
  
  Meta_Wrapper<DIMENSION>  *      _fromWrapper;
  Meta_Wrapper<DIMENSION>  *      _toWrapper;
  
  Meta_Mapping <DIMENSION> *      _mapping;

// only used when multi timestep are interpolated
// but always coherent
  int				  _iType;
  int				  _isConvexFromMesh;
  
public :

  void init();

  // Initialize INTERPOLATION in order to get : 
  // 1- the node number in the MESH  <fromMesh> which
  //     is the nearest from a given one ( use method : getNearestNode( double * node ) ); 
  // 2- the cell number (if exists) in the MESH <fromMesh> which
  //     contains a specified node ( use method : getContainingCell ( double * node) )
  INTERPOLATION(const MESH & fromMesh ); 
  // Initialize INTERPOLATION in order to get :
  // 1- the complete mapping ( use methode : getMapping() )
  // 2- the functionalities above 
  INTERPOLATION(const MESH & fromMesh,const MESH & toMesh ); 
  // Initialize INTERPOLATION in order to get the interpolation of <field> on <toMesh>
  // Moreover, all the others functionalities are so available
  INTERPOLATION(const FIELD<double> & fromField, const MESH & toMesh);

  ~INTERPOLATION( ); 

  //  Get the node number in the MESH  <fromMesh> which is the nearest from a given one
  int getNearestNode    ( double * node );
  //  Get the cell number (if exists) in the MESH <fromMesh> which contains a specified node 
  int getContainingCell ( double * node , int beginingCell=0, int flagIsConvexMesh=0 );
  // Get the complete mapping, defaultly, fromMesh is supposed to be non-convex, if it is false, set flagIsConvexMesh to 1
  vector<int> getMapping ( int flagIsConvexMesh=0 );
  // Get the interpolated field toField
  FIELD<double> * interpolate( /*med_interpolation_type*/ int itype,int flagIsConvexFromMesh=0);
  // reset the <from> parameters in order not to redo the mapping (if the mesh are identical)
  // and then get the interpoated field toField
  // this method is specifictly used on multi-timestep (or order number) fields
  // it has only to be used after the first step, the interpolation paramaters are the same for every step
  FIELD<double> * interpolateNextStep(const FIELD <double> &nextFromField ,int & flagNewMapping);

};

template <int DIMENSION> void INTERPOLATION<DIMENSION>::init() {

  const char * LOC = "INTERPOLATION::init(): ";

  BEGIN_OF(LOC);
  _fromField   = ( FIELD<double> * )           NULL;
  _toField     = ( FIELD<double> * )           NULL;
  _fromMesh    = ( MESH * )                    NULL;
  _toMesh      = ( MESH * )                    NULL;
  _fromWrapper = ( Meta_Wrapper<DIMENSION> * ) NULL;
  _toWrapper   = ( Meta_Wrapper<DIMENSION> * ) NULL;
  _mapping     = ( Meta_Mapping<DIMENSION> * ) NULL;
  _iType            = UNDEFINED ;
  _isConvexFromMesh = UNDEFINED ;
  END_OF(LOC);
}


template <int DIMENSION> INTERPOLATION<DIMENSION>::INTERPOLATION(const MESH & fromMesh ) {

  const char * LOC = "INTERPOLATION::INTERPOLATION(MESH * fromMesh ) : ";
  BEGIN_OF(LOC);
  
  init();	
  
  _fromMesh=const_cast<MESH * > (&fromMesh);
  
  if (! _fromMesh ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"fromMesh is a NULL pointer  !")) ;
  
  int spaceDimension = _fromMesh->getSpaceDimension();
  if (spaceDimension != DIMENSION ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The spaceDimension of mesh |" << _fromMesh->getName() << "| is |" << spaceDimension << "| and should be |" << DIMENSION << "|" << endl)) ;

  _fromWrapper = new Meta_Wrapper<DIMENSION>(_fromMesh->getNumberOfNodes(),
                                             const_cast<double *> (_fromMesh->getCoordinates(MED_EN::MED_FULL_INTERLACE)),
					     const_cast<CONNECTIVITY *> (_fromMesh->getConnectivityptr())
					     );

  _mapping     = new  Meta_Mapping<DIMENSION> (_fromWrapper);
					        
  END_OF(LOC);
}; 

template <int DIMENSION> INTERPOLATION<DIMENSION>::INTERPOLATION(const MESH & fromMesh,const MESH & toMesh ) {

  const char * LOC = "INTERPOLATION::INTERPOLATION(MESH * fromMesh,,const MESH & toMesh) : ";
  BEGIN_OF(LOC);
  
  init();	
  
  _fromMesh = const_cast<MESH * > ( &fromMesh );
  _toMesh   = const_cast<MESH * > ( &toMesh   );
  
  if (! _fromMesh ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"fromMesh is a NULL pointer  !")) ;
  if (! _toMesh   ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"toMesh   is a NULL pointer  !")) ;
  
  int fromSpaceDimension = _fromMesh->getSpaceDimension();
  int toSpaceDimension   =   _toMesh->getSpaceDimension();
  
  if (fromSpaceDimension != DIMENSION ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The spaceDimension of mesh |" << _fromMesh->getName() << "| is |" << INTERPOLATION<DIMENSION>::spaceDimension << "| and should be |" << DIMENSION << "|" << endl)) ;
  if (  toSpaceDimension != DIMENSION ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"The spaceDimension of mesh |" <<   _toMesh->getName() << "| is |" << INTERPOLATION<DIMENSION>::spaceDimension << "| and should be |" << DIMENSION << "|" << endl)) ;
 
  _fromWrapper = new Meta_Wrapper<DIMENSION>(_fromMesh->getNumberOfNodes(),
                                             const_cast<double *> (_fromMesh->getCoordinates(MED_EN::MED_FULL_INTERLACE)),
					     const_cast<CONNECTIVITY *> (_fromMesh->getConnectivityptr())
					     );

  _toWrapper   = new Meta_Wrapper<DIMENSION>(_toMesh->getNumberOfNodes(),
                                             const_cast<double *> (_toMesh->getCoordinates(MED_EN::MED_FULL_INTERLACE))
					     );

  _mapping     = new  Meta_Mapping<DIMENSION> (_fromWrapper);
					        
  END_OF(LOC);
};

template <int DIMENSION> INTERPOLATION<DIMENSION>::INTERPOLATION(const FIELD<double> & fromField,const MESH & toMesh) {

  const char * LOC = "INTERPOLATION(const FIELD<double> & field,const MESH & toMesh) : ";
  BEGIN_OF(LOC);
  
  init();

  _toMesh    = const_cast<MESH *>(&toMesh);
  _fromField = const_cast<FIELD<double> *>(&fromField);

  if ( ! _toMesh    )  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"toMesh is a NULL pointer  !")) ;
  if ( ! _fromField )  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"field is a NULL pointer  !")) ;

  _fromMesh = _fromField->getSupport()->getMesh();
  
  if ( ! _fromMesh    )  throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"fromMesh is a NULL pointer  !")) ;

  _fromWrapper = new Meta_Wrapper<DIMENSION>(_fromMesh->getNumberOfNodes(),
                                             const_cast<double *> (_fromMesh->getCoordinates(MED_EN::MED_FULL_INTERLACE)),
					     const_cast<MEDMEM::CONNECTIVITY *> (_fromMesh->getConnectivityptr()),
					     const_cast<MEDMEM::FIELD<double> *>(_fromField)
					     );
					     
					     
  _toWrapper   = new Meta_Wrapper<DIMENSION>(_toMesh->getNumberOfNodes(),
                                             const_cast<double *> (_toMesh->getCoordinates(MED_EN::MED_FULL_INTERLACE))
					     );  

  
  _mapping     = new  Meta_Mapping<DIMENSION> (_fromWrapper);
  
					  
  END_OF(LOC);
};

template <int DIMENSION> INTERPOLATION<DIMENSION>::~INTERPOLATION()
{
  if ( _fromWrapper  ) delete _fromWrapper ;    
  if ( _toWrapper    ) delete _toWrapper   ;    
  if ( _mapping      ) delete _mapping     ;
};

template <int DIMENSION> int INTERPOLATION<DIMENSION>::getNearestNode(  double * node ) {
  
  const char * LOC = "INTERPOLATION::getNearestNode( double * node ) ";

  BEGIN_OF(LOC);
  
  if ( ! _mapping ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"mapping is a NULL pointer  !")) ;
  
  return _mapping->Donne_dTree()->trouve_plus_proche_point(Wrapper_Noeud<DIMENSION > (node) );
  
  END_OF(LOC);

};

template <int DIMENSION> int INTERPOLATION<DIMENSION>::getContainingCell ( double * node , int beginingCell, int flagIsConvexMesh ) {
  
  const char * LOC = "INTERPOLATION::getContainingCell( double * node ) ";

  BEGIN_OF(LOC);
  
  if ( ! _mapping ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"mapping is a NULL pointer  !")) ;

  _isConvexFromMesh=flagIsConvexMesh;

  _mapping->Cree_Mapping(_toWrapper,_isConvexFromMesh);

  return _mapping->Trouve_Maille_Contenant_Noeud(node,beginingCell,flagIsConvexMesh);
  
  END_OF(LOC);

};

template <int DIMENSION> vector<int> INTERPOLATION<DIMENSION>::getMapping ( int flagIsConvexMesh ) {
  
  const char * LOC = "INTERPOLATION::getMapping( ) ";

  BEGIN_OF(LOC);

  if ( ! _mapping   ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"mapping is a NULL pointer  !")) ;
  if ( ! _toWrapper ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"toWrapper  is a NULL pointer  !")) ;

  _isConvexFromMesh=flagIsConvexMesh;

  _mapping->Cree_Mapping(_toWrapper,_isConvexFromMesh);

  return _mapping->Get_Mapping();
  
  END_OF(LOC);

};

template <int DIMENSION> FIELD<double> * INTERPOLATION<DIMENSION>::interpolate(int itype,int flagIsConvexFromMesh) {
  
  const char * LOC = "INTERPOLATION::interpolate(int itype,int flagIsConvexFromMesh) ";

  BEGIN_OF(LOC);

  if ( ! _mapping      ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"mapping is a NULL pointer  !")) ;
  if ( ! _toWrapper    ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"toWrapper  is a NULL pointer  !")) ;
  if ( ! _fromField    ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"fromField  is a NULL pointer  !")) ;

  _iType=itype;

  _isConvexFromMesh=flagIsConvexFromMesh;

  _mapping->Cree_Mapping(_toWrapper,_isConvexFromMesh);
  
  Wrapper_Nuage_Noeud<DIMENSION> * toNodes = _toWrapper->Get_Nuage_Noeuds();
  
  Wrapper_MED_Field resultat;
  
  /*
  cout<<"Mapping"<<endl;
  _mapping->affiche();
  cout<<"Mailles"<<endl;
  _fromWrapper->Get_Maillage()->DONNE_POINTEUR_NUAGEMAILLE()->affiche();
  cout<<"Noeuds"<<endl;
  _fromWrapper->Get_Nuage_Noeuds()->affiche();
  */
  
  switch (_iType)
    {
    case 0 : // INTERPOLATION P0
      cout<<"Avant ="<<endl;
      resultat=Meta_Interpolateur< Meta_Calcul_Interpolation_P0<DIMENSION>,DIMENSION >(_mapping,_fromWrapper).Perform_Interpolation(toNodes);
      break;
    case 1 : // INTERPOLATION P-Hybride (Interpole avec la fonction d'interpolation naturelle de la maille contenant le point)
      resultat=Meta_Interpolateur< Meta_Calcul_Interpolation_Hybride<DIMENSION>,DIMENSION >(_mapping,_fromWrapper).Perform_Interpolation(toNodes);
      break;
    case 2 : // INTERPOLATION (P/Q) 1 forcée (Interpole avec la fonction élément fini de la maille de degré 1 -meme si la maille est de degré supérieur-)
      resultat=Meta_Interpolateur< Meta_Calcul_Interpolation_Hybride_P1<DIMENSION>, DIMENSION >(_mapping,_fromWrapper).Perform_Interpolation(toNodes);
      break;
    default : 
      throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"Interpolation type "<<itype<<" not yet implemented !")) ;
    }

  cout<<"On a fini l'interpolation"<<endl;	

  _toField = new FIELD<double>;
  
  _toField->setName		      ( _fromField->getName()			);
  _toField->setDescription	      ( _fromField->getDescription()		);
  _toField->setNumberOfComponents     ( _fromField->getNumberOfComponents()	);
  _toField->setNumberOfValues	      ( _toMesh   ->getNumberOfNodes()  	);
  _toField->setComponentsNames        ( _fromField->getComponentsNames()        );
  _toField->setComponentsDescriptions ( _fromField->getComponentsDescriptions() );
  _toField->setMEDComponentsUnits     ( _fromField->getMEDComponentsUnits()	);
  _toField->setIterationNumber        ( _fromField->getIterationNumber()        );
  _toField->setTime		      ( _fromField->getTime()		        );
  _toField->setOrderNumber	      ( _fromField->getOrderNumber()		);
  _toField->setValueType	      ( MED_EN::MED_REEL64			);

  SUPPORT * mySupport(new SUPPORT(_toMesh,"support",MED_EN::MED_NODE));
  _toField->setSupport(mySupport);  
  
  _toField->allocValue(_toField->getNumberOfComponents(),_toField->getNumberOfValues());
    
  _toField->setValue( MED_EN::MED_FULL_INTERLACE,resultat.Get_Valeurs());
 
  _toWrapper->Construit_Wrapper_Champ(_toField);

  return _toField;
  
  END_OF(LOC);

};

template <int DIMENSION> FIELD<double> * INTERPOLATION<DIMENSION>::interpolateNextStep(const FIELD <double> & nextFromField, int & flagNewMapping) {
  
  const char * LOC = "INTERPOLATION::interpolateNextStep(int itype,int flagIsConvexFromMesh) ";

  BEGIN_OF(LOC);

  if ( ! _mapping      ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"mapping is a NULL pointer  !"        ));
  if ( ! _toWrapper    ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"toWrapper  is a NULL pointer  !"     )) ;
  if ( ! _fromWrapper  ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"fromWrapper  is a NULL pointer  !"   )) ;  
  if ( ! _fromField    ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"fromField  is a NULL pointer  !"     )) ;
  
  
  if ( ! _toField                   ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"toField  is a NULL pointer, wrong use of interpolateNextStep"       )) ;
  if ( _iType==UNDEFINED            ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"_iType is not defined, wrong use of interpolateNextStep"            )) ;
  if ( _isConvexFromMesh==UNDEFINED ) throw MEDEXCEPTION(LOCALIZED(STRING(LOC)<<"_isConvexFromMesh is not defined, wrong use of interpolateNextStep" )) ;  

  // delete _toField; ????????????????????????????

  // if the mesh are identical, the mapping is the same, if not, the mapping has to be re-calculated
  if (nextFromField.getSupport()->getMesh()->getName()!=_fromMesh->getName())
  	{
	
	flagNewMapping=1;
	
	delete _mapping;
	delete _fromWrapper;

        _fromField = const_cast<FIELD<double> *>(&nextFromField);

        _fromMesh = _fromField->getSupport()->getMesh();

	_fromWrapper = new Meta_Wrapper<DIMENSION>(_fromMesh->getNumberOfNodes(),
						   const_cast<double *> (_fromMesh->getCoordinates(MED_EN::MED_FULL_INTERLACE)),
						   const_cast<MEDMEM::CONNECTIVITY *> (_fromMesh->getConnectivityptr()),
						   const_cast<MEDMEM::FIELD<double> *>(_fromField)
						   );
  	_mapping     = new  Meta_Mapping<DIMENSION> (_fromWrapper);
	
	_mapping->Cree_Mapping(_toWrapper,_isConvexFromMesh);
	
	}
  else
  	{
	
	flagNewMapping=0;
	
	_fromField = const_cast<MEDMEM::FIELD<double> *>(&nextFromField);
	_fromWrapper->Change_Champ(const_cast<MEDMEM::FIELD<double> *>(_fromField));		
	}
  
  return interpolate(_iType,_isConvexFromMesh);

};

};

#endif


