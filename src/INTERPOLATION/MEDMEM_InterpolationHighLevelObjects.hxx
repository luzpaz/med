#ifndef MEDMEM_INTERPOLATION_HIGHLEVEL_OBJECTS_HXX
#define MEDMEM_INTERPOLATION_HIGHLEVEL_OBJECTS_HXX

#include "MEDMEM_Connectivity.hxx"
#include "MEDMEM_WrapperConnectivity.hxx"
#include "MEDMEM_dTree.hxx"
#include "MEDMEM_WrapperNodes.hxx"
#include "MEDMEM_WrapperMesh.hxx"
#include "MEDMEM_WrapperCells.hxx"
#include "MEDMEM_Mapping.hxx"
#include "MEDMEM_WrapperField.hxx"
#include "MEDMEM_InterpolationTools.hxx"

//////////////////////////////////////////////////////////////////
///                                                            ///
///                        DECLARATIONS                        ///
///                                                            ///
//////////////////////////////////////////////////////////////////

template <int DIMENSION> class Meta_Wrapper;

/*********************************************************/
/*                                                       */
/*                    Meta_dTree                         */
/*                                                       */
/*********************************************************/

template <int DIMENSION> class Meta_dTree : public dTree<Wrapper_Noeud<DIMENSION>,Wrapper_Nuage_Noeud<DIMENSION>,DIMENSION>
{
protected : 
	// PATCH
	Wrapper_Nuage_Noeud<DIMENSION> * noeuds;
	// FIN PATCH
public :
	// PATCH
	Meta_dTree():noeuds(NULL) {}
	~Meta_dTree() {if (noeuds) delete noeuds;}
	Meta_dTree(int nbr_noeuds,double *coord):dTree<Wrapper_Noeud<DIMENSION>,Wrapper_Nuage_Noeud<DIMENSION>,DIMENSION>(noeuds=new Wrapper_Nuage_Noeud<DIMENSION>(nbr_noeuds,coord)) {}
	inline int trouve_plus_proche_point_bourrin(double *node);
	// FIN PATCH
	inline int trouve_plus_proche_point(double * node);
};

/*********************************************************/
/*                                                       */
/*                 Meta_Nuage_Maille                     */
/*                                                       */
/*********************************************************/


class Meta_Nuage_Maille : public Wrapper_Nuage_Maille<Wrapper_Med_Connectivity>
{
protected :
	Wrapper_Med_Connectivity * connectivite_med;
public :
	Meta_Nuage_Maille(CONNECTIVITY * connmed); 
	Meta_Nuage_Maille():connectivite_med(NULL) {}
	~Meta_Nuage_Maille() {if (connectivite_med) delete connectivite_med;}
};

/*********************************************************/
/*                                                       */
/*                    Meta_Maillage                      */
/*                                                       */
/*********************************************************/


typedef Wrapper_Maillage<Meta_Nuage_Maille> Meta_Maillage;

/*********************************************************/
/*                                                       */
/*                    Meta_Mapping                       */
/*                                                       */
/*********************************************************/

template <int DIMENSION> class Meta_Mapping : public Mapping<Meta_Maillage,Meta_Nuage_Maille,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,DIMENSION>
{
public :
	Meta_Mapping(Meta_Wrapper<DIMENSION> * MW):Mapping<Meta_Maillage,Meta_Nuage_Maille,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,DIMENSION>(MW->Get_Maillage(),MW->Get_Nuage_Noeuds(),NULL) {}
	Meta_Mapping(Meta_Wrapper<DIMENSION> * MW,Meta_Wrapper<DIMENSION> * TWB):Mapping<Meta_Maillage,Meta_Nuage_Maille,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,DIMENSION>(MW->Get_Maillage(),MW->Get_Nuage_Noeuds(),TWB->Get_Nuage_Noeuds()) {}
	// PATCH
	inline void Cree_Mapping(Meta_Wrapper<DIMENSION> * MWB, int flag_convexe) {Mapping<Meta_Maillage,Meta_Nuage_Maille,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,DIMENSION>::Cree_Mapping(MWB->Get_Nuage_Noeuds(),flag_convexe);} 
	inline void Cree_Mapping(int flag_convexe) {Mapping<Meta_Maillage,Meta_Nuage_Maille,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,DIMENSION>::Cree_Mapping(flag_convexe);} 
	inline vector<int> & Get_Mapping() {return Mapping<Meta_Maillage,Meta_Nuage_Maille,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,DIMENSION>::Get_Mapping();} 
	//FIN PATCH	
	inline int Trouve_Maille_Contenant_Noeud(double * node,int num_maille, int flag_convexe=0);
};

/*********************************************************/
/*                                                       */
/*                    Meta_Wrapper                       */
/*                                                       */
/*********************************************************/


template <int DIMENSION> class Meta_Wrapper
{
protected :
	Wrapper_Nuage_Noeud<DIMENSION> * noeuds   ;
	Meta_Nuage_Maille              * mailles  ;
	Meta_Maillage                  * maillage ;
	Wrapper_MED_Field              * champ    ;
	
	void init( ){noeuds=NULL;mailles=NULL;maillage=NULL;champ=NULL;}
public :
	Meta_Wrapper():noeuds(NULL),mailles(NULL),maillage(NULL),champ(NULL){}
	~Meta_Wrapper();
	inline void Construit_Wrapper_Nuage_Noeud  (     int nn, double * nodes     );
	inline void Construit_Wrapper_Nuage_Maille (     CONNECTIVITY * connmed     );
	inline void Construit_Wrapper_Maillage     (            void                );
	inline void Construit_Wrapper_Champ        ( const FIELD<double> * medfield );
	Meta_Wrapper(int nn,double *nodes,CONNECTIVITY *connmed, int flag_maillage=1);
	Meta_Wrapper(int nn,double *nodes);
	// defaultly, the connectivity (neighbouhood and so like) is built, 
	// Set flag_mesh to 0 if you don't want these informations to be built
	Meta_Wrapper(int nn,double *nodes,CONNECTIVITY *connmed, const FIELD<double> * c,int flag_mesh=1);
	// fonctions d'acces sures
	inline Wrapper_Nuage_Noeud<DIMENSION> * Get_Nuage_Noeuds  ( void );
	inline Meta_Nuage_Maille              * Get_Nuage_Mailles ( void );
	inline Meta_Maillage                  * Get_Maillage      ( void );
	inline Wrapper_MED_Field              * Get_Champ         ( void );
	inline void Change_Champ           ( const FIELD<double> * medfield );
};

/*********************************************************/
/*                                                       */
/*           Meta_Calcul_Interpolation_Hybride           */
/*                                                       */
/*********************************************************/

template <int DIMENSION> class Meta_Calcul_Interpolation_Hybride : public Calcul_Hybride<Wrapper_MED_Field,Valeur<double>,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,Meta_Nuage_Maille>
{
public :
	Meta_Calcul_Interpolation_Hybride(Meta_Wrapper<DIMENSION> * MW):Calcul_Hybride<Wrapper_MED_Field,Valeur<double>,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,Meta_Nuage_Maille>(MW->Get_Nuage_Noeuds(),MW->Get_Nuage_Mailles(),MW->Get_Champ()) {}
	Valeur<double> operator() (Wrapper_Noeud<DIMENSION> & node, int num_maille){return Calcul_Hybride<Wrapper_MED_Field,Valeur<double>,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,Meta_Nuage_Maille>::operator()(node,num_maille);}
	Valeur<double> operator() (double * node, int num_maille) 
		{
		static Wrapper_Noeud<DIMENSION> tmp;
		tmp.positionne(node);
		return Calcul_Hybride<Wrapper_MED_Field,Valeur<double>,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,Meta_Nuage_Maille>(tmp,num_maille);
		}
};

/*********************************************************/
/*                                                       */
/*         Meta_Calcul_Interpolation_Hybride_P1          */
/*                                                       */
/*********************************************************/

template <int DIMENSION> class Meta_Calcul_Interpolation_Hybride_P1 : public Calcul_Hybride<Wrapper_MED_Field,Valeur<double>,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,Meta_Nuage_Maille>
{
public :
	Meta_Calcul_Interpolation_Hybride_P1(Meta_Wrapper<DIMENSION> * MW)
		{
		
		Wrapper_Nuage_Noeud<DIMENSION> * nn = MW->Get_Nuage_Noeuds();
		Meta_Nuage_Maille *              nm = MW->Get_Nuage_Mailles();
		Wrapper_MED_Field *              c  = MW->Get_Champ();
		
		mailles=nm;
		
		fonctions[MED_TRIA3  ]=new Calcul_Interpolation_Tria3  <Wrapper_MED_Field,Valeur<double>,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,Meta_Nuage_Maille>(nn,nm,c);
		fonctions[MED_QUAD4  ]=new Calcul_Interpolation_Quad4  <Wrapper_MED_Field,Valeur<double>,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,Meta_Nuage_Maille>(nn,nm,c);
		fonctions[MED_TETRA4 ]=new Calcul_Interpolation_Tetra4 <Wrapper_MED_Field,Valeur<double>,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,Meta_Nuage_Maille>(nn,nm,c);
		fonctions[MED_HEXA8  ]=new Calcul_Interpolation_Hexa8  <Wrapper_MED_Field,Valeur<double>,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,Meta_Nuage_Maille>(nn,nm,c);
		fonctions[MED_PENTA6 ]=new Calcul_Interpolation_Penta6 <Wrapper_MED_Field,Valeur<double>,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,Meta_Nuage_Maille>(nn,nm,c);
		fonctions[MED_PYRA5  ]=new Calcul_Interpolation_Pyra5  <Wrapper_MED_Field,Valeur<double>,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,Meta_Nuage_Maille>(nn,nm,c);
		fonctions[MED_TRIA6  ]=fonctions[MED_TRIA3  ];
		fonctions[MED_QUAD8  ]=fonctions[MED_QUAD4  ];
		fonctions[MED_TETRA10]=fonctions[MED_TETRA4 ];
		fonctions[MED_HEXA20 ]=fonctions[MED_HEXA8  ];
		fonctions[MED_PENTA15]=fonctions[MED_PENTA6 ];
		fonctions[MED_PYRA13 ]=fonctions[MED_PYRA5  ];
		}
	Valeur<double> operator() (Wrapper_Noeud<DIMENSION> & node, int num_maille){return Calcul_Hybride<Wrapper_MED_Field,Valeur<double>,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,Meta_Nuage_Maille>::operator()(node,num_maille);}
	Valeur<double> operator() (double * node, int num_maille) 
		{
		static Wrapper_Noeud<DIMENSION> tmp;
		tmp.positionne(node);
		return Calcul_Hybride<Wrapper_MED_Field,Valeur<double>,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,Meta_Nuage_Maille>(tmp,num_maille);
		}
};

/*********************************************************/
/*                                                       */
/*              Meta_Calcul_Interpolation_P0             */
/*                                                       */
/*********************************************************/

template <int DIMENSION> class Meta_Calcul_Interpolation_P0 : public Calcul_Interpolation_P0<Wrapper_MED_Field,Valeur<double>,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,Meta_Nuage_Maille>
{
public : 
	Meta_Calcul_Interpolation_P0(Meta_Wrapper<DIMENSION> * MW):Calcul_Interpolation_P0<Wrapper_MED_Field,Valeur<double>,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,Meta_Nuage_Maille>(MW->Get_Nuage_Noeuds(),MW->Get_Nuage_Mailles(),MW->Get_Champ()) {}
	Valeur<double> operator() (Wrapper_Noeud<DIMENSION> & node, int num_maille){return Calcul_Interpolation_P0<Wrapper_MED_Field,Valeur<double>,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,Meta_Nuage_Maille>::operator()(node,num_maille);}
	Valeur<double> operator() (double * node, int num_maille) 
		{
		static Wrapper_Noeud<DIMENSION> tmp;
		tmp.positionne(node);
		return Calcul_Interpolation_P0<Wrapper_MED_Field,Valeur<double>,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,Meta_Nuage_Maille>(tmp,num_maille);
		}
};

/*********************************************************/
/*                                                       */
/*                   Meta_Interpolateur                  */
/*                                                       */
/*********************************************************/

template <class FONCTEUR, int DIMENSION> class Meta_Interpolateur
{
protected :
	FONCTEUR * fct;
	Meta_Mapping<DIMENSION> * mapping;
	Meta_Wrapper<DIMENSION> * fromWrapper;
public : 
	Meta_Interpolateur():fct(NULL),mapping(NULL),fromWrapper(NULL) {}
	Meta_Interpolateur(Meta_Mapping<DIMENSION> * map, Meta_Wrapper<DIMENSION> * mw):mapping(map),fromWrapper(mw),fct(new FONCTEUR(mw)){}
	~Meta_Interpolateur() {if (fct) delete fct;}
	Wrapper_MED_Field Perform_Interpolation(Wrapper_Nuage_Noeud<DIMENSION> * toNodes)
		{
		int i;
		
		int ni=0;
		int ne=0;
		
  		int nbr_composantes = fromWrapper->Get_Champ()->Get_Nbr_Composantes();
  		int nbr_valeurs     = toNodes->SIZE();
  
  		double * valeurs=new double[nbr_valeurs*nbr_composantes];
		
  		Wrapper_MED_Field resultat(nbr_valeurs,nbr_composantes,valeurs);
		
		int nlpp,nmc;
		
		for (i=0;i<nbr_valeurs;i++) 
			{
			//cout<<"Interpolation du noeud "<<i<<flush;
			nmc = (*mapping)[i];
			//cout<<" | mappé dans la maille "<<nmc<<flush;
			//cout<<" | coordonnées = "<<flush<<(*toNodes)[i]<<flush;
			if (nmc>=0) 
				{
				//cout<<" | valeurs qui va etre assignée = "<<flush<<(*fct)((*toNodes)[i],nmc)<<flush;
				resultat[i]=(*fct)((*toNodes)[i],nmc);
				ni++;
				}
			else 
				{
				nlpp = mapping->Get_Noeud_Le_Plus_Proche(i);
				//cout<<" | et dont le point le plus proche a pour numéro : "<<nlpp<<flush;
				//cout<<" | valeurs qui va etre assignée = "<<(*fromWrapper->Get_Champ())[nlpp]<<flush;
				if (nlpp!=UNDEFINED) 
					{
					resultat[i]=(*fromWrapper->Get_Champ())[nlpp];
					ne++;
					}
				else
					{
					cerr<<"Meta_Interpolateur : Le noeud "<<i+1<<" n'a ni maille contenante, ni point le plus proche"<<flush;
					exit(-1);
					}
				}
			//cout<<" | => OK ! "<<endl;
			}
		
		cout<<"Résultat de l'interpolation : "<<endl;
		cout<<"Nombre de noeuds intérieurs = "<<ni<<endl;
		cout<<"Nombre de noeuds extérieurs = "<<ne<<endl;
			
		return resultat;
		
		}
};



//////////////////////////////////////////////////////////////////
///                                                            ///
///                            CODE                            ///
///                                                            ///
//////////////////////////////////////////////////////////////////

/*********************************************************/
/*                                                       */
/*                    Meta_dTree                         */
/*                                                       */
/*********************************************************/

template <int DIMENSION> inline int Meta_dTree<DIMENSION>::trouve_plus_proche_point(double *node)
	{
	static Wrapper_Noeud<DIMENSION> nodetmp;
	nodetmp.positionne(node);
	return dTree<Wrapper_Noeud<DIMENSION>,Wrapper_Nuage_Noeud<DIMENSION>,DIMENSION>::trouve_plus_proche_point(Wrapper_Noeud<DIMENSION>(nodetmp));
	}
// PATCH
template <int DIMENSION> inline int Meta_dTree<DIMENSION>::trouve_plus_proche_point_bourrin(double *node)
	{
	static Wrapper_Noeud<DIMENSION> nodetmp;
	nodetmp.positionne(node);
	return dTree<Wrapper_Noeud<DIMENSION>,Wrapper_Nuage_Noeud<DIMENSION>,DIMENSION>::trouve_plus_proche_point_bourrin(Wrapper_Noeud<DIMENSION>(nodetmp));
	}
/*********************************************************/
/*                                                       */
/*                 Meta_Nuage_Maille                     */
/*                                                       */
/*********************************************************/

inline Meta_Nuage_Maille::Meta_Nuage_Maille(CONNECTIVITY * conmed):Wrapper_Nuage_Maille<Wrapper_Med_Connectivity>(connectivite_med=new Wrapper_Med_Connectivity(conmed))
	{
	}

/*********************************************************/
/*                                                       */
/*                    Meta_Mapping                       */
/*                                                       */
/*********************************************************/

template <int DIMENSION> inline int Meta_Mapping<DIMENSION>::Trouve_Maille_Contenant_Noeud(double * node,int num_maille,int flag_convexe)
	{
	int interdit=num_maille;
	int max_loop=100;
	int nme=0;
	static Wrapper_Noeud<DIMENSION> nodetmp;
	nodetmp.positionne(node);
	return Mapping<Meta_Maillage,Meta_Nuage_Maille,Wrapper_Nuage_Noeud<DIMENSION>,Wrapper_Noeud<DIMENSION>,DIMENSION>::Trouve_Maille_Contenant_Point_Mth_Co(nodetmp,num_maille,interdit,max_loop,nme,flag_convexe);
	}

/*********************************************************/
/*                                                       */
/*                    Meta_Wrapper                       */
/*                                                       */
/*********************************************************/

template <int DIMENSION> 	Meta_Wrapper<DIMENSION>::~Meta_Wrapper()
	{
	if ( noeuds   ) delete  noeuds   ;
	if ( mailles  ) delete  mailles  ;
	if ( maillage ) delete  maillage ;
	if ( champ    ) delete  champ    ;
	}
template <int DIMENSION> 	inline void Meta_Wrapper<DIMENSION>::Construit_Wrapper_Nuage_Noeud  ( int nn, double * nodes )  
	{
	if (nodes) noeuds=new Wrapper_Nuage_Noeud<DIMENSION>(nn,nodes); 
	else
		{
		cerr<<"Meta_Wrapper : Nuage MED_FULL_INTERLACE vide passé en argument au constructeur"<<endl;
		exit(-1);
		}
	}
template <int DIMENSION> 	inline void Meta_Wrapper<DIMENSION>::Construit_Wrapper_Nuage_Maille ( CONNECTIVITY * connmed )  
	{ 
	if (connmed) mailles=new Meta_Nuage_Maille(connmed);              
	else
		{
		cerr<<"Meta_Wrapper : CONNECTIVITY vide passée en argument au constructeur"<<endl;
		exit(-1);
		}
	}
template <int DIMENSION> 	inline void Meta_Wrapper<DIMENSION>::Construit_Wrapper_Maillage     (         void           )  
	{ 
	if (mailles==NULL) 	
		{
		cerr<<"Meta_Wrapper : Le nuage de maille n'a pas été initialisé !"<<endl;
		exit(-1);
		}
	if (noeuds==NULL) 	
		{
		cerr<<"Meta_Wrapper : Le nuage de noeuds n'a pas été initialisé !"<<endl;
		exit(-1);
		}
	maillage=new Meta_Maillage(mailles,noeuds->SIZE());
	}
template <int DIMENSION> 	inline void Meta_Wrapper<DIMENSION>::Construit_Wrapper_Champ        ( const FIELD<double> * medfield )
	{
	if (medfield) champ=new Wrapper_MED_Field(medfield);
	else
		{
		cerr<<"Meta_Wrapper : FIELD MED vide passé en argument au constructeur"<<endl;
		exit(-1);
		}
	}
template <int DIMENSION> 	inline void Meta_Wrapper<DIMENSION>::Change_Champ           ( const FIELD<double> * medfield )
	{
	if (medfield) 
		{
		if (champ) delete champ;
		champ=new Wrapper_MED_Field(medfield);
		}
	else
		{
		cerr<<"Meta_Wrapper : FIELD MED vide passé en argument Change_Champ"<<endl;
		exit(-1);
		}
	}
template <int DIMENSION> 	Meta_Wrapper<DIMENSION>::Meta_Wrapper(int nn,double *nodes,CONNECTIVITY *connmed, int flag_maillage)
	{
	init();
	Construit_Wrapper_Nuage_Noeud(nn,nodes);
	Construit_Wrapper_Nuage_Maille(connmed);
	if (flag_maillage) Construit_Wrapper_Maillage();
	}
template <int DIMENSION> 	Meta_Wrapper<DIMENSION>::Meta_Wrapper(int nn,double *nodes,CONNECTIVITY *connmed, const FIELD<double> * c,int flag_maillage)
	{
	init();
	Construit_Wrapper_Nuage_Noeud(nn,nodes);
	Construit_Wrapper_Nuage_Maille(connmed);
	if (flag_maillage) Construit_Wrapper_Maillage();
	Construit_Wrapper_Champ(c);
	}
template <int DIMENSION> 	Meta_Wrapper<DIMENSION>::Meta_Wrapper(int nn,double *nodes)
	{
	init();
	Construit_Wrapper_Nuage_Noeud(nn,nodes);
	}			
template <int DIMENSION> 	inline Wrapper_Nuage_Noeud<DIMENSION> * Meta_Wrapper<DIMENSION>::Get_Nuage_Noeuds  ( void ) 
	{
	if (noeuds) return noeuds; 
	else
		{
		cerr<<"Meta_Wrapper : Nuage noeuds demandé alors qu'il n'est pas construit !"<<endl;
		exit(-1);
		}
	}
template <int DIMENSION> 	inline Meta_Nuage_Maille              * Meta_Wrapper<DIMENSION>::Get_Nuage_Mailles ( void ) 
	{ 
	if (mailles) return mailles  ; 
	else
		{
		cerr<<"Meta_Wrapper : Nuage mailles demandé alors qu'il n'est pas construit !"<<endl;
		exit(-1);
		}
	}
template <int DIMENSION> 	inline Meta_Maillage                  * Meta_Wrapper<DIMENSION>::Get_Maillage      ( void ) 
	{ 
	if (maillage) return maillage ; 
	else
		{
		cerr<<"Meta_Wrapper : Connectivitée maillage demandée alors qu'elle n'est pas construite !"<<endl;
		exit(-1);
		}
	}

template <int DIMENSION>	inline Wrapper_MED_Field              * Meta_Wrapper<DIMENSION>::Get_Champ         ( void )
	{
	if (champ) return champ;
	else
		{
		cerr<<"Meta_Wrapper : Champ demandé alors qu'il n'est pas construit !"<<endl;
		exit(-1);
		}
	}

#endif 
