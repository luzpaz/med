# ifndef MED_HXX
# define MED_HXX

// STL
# include <string>
# include <map>
# include <vector>
# include <deque>

// LOCAL
# include "MEDMEM_define.hxx"
# include "MEDMEM_MedMedDriver.hxx"
# include "MEDMEM_Exception.hxx"
//using namespace MED_EN;


class MESH;
class FIELD_;
class SUPPORT ;

typedef string MESH_NAME_;
typedef string FIELD_NAME_;
typedef struct { int dt; int it; } DT_IT_;
struct LT_DT_IT_
{
  bool operator() (const DT_IT_ &p1, const DT_IT_ &p2) const
  {
    if ( bool test = p1.dt == p2.dt)
      return p1.it < p2.it ;
    else
      return  p1.dt < p2.dt ;
  }
};
typedef map<DT_IT_, FIELD_*, LT_DT_IT_ > MAP_DT_IT_; 

// - IN THE FIRST CASE THE USER WANTS TO DISCOVER MESHES & FIELD_S 
//   CONTAINNED WITHIN A FILE <fileName> OF TYPE GIVEN BY THE  <driverType> PARAMETER
// - IN THE SECOND CASE THE USER BEGINS HIS WORK WITH A MESH OR A FIELD, 
//   ?? GET A MED POINTER THEN CAN ADD MESHes OR FIELDs ??

class MED
{
  friend class MED_MED_DRIVER;
  friend class MED_MED_RDONLY_DRIVER;

private:

  map<MESH_NAME_,MESH*>        _meshes;     // We can't have two MESHes with the same meshName.  
                                           // The string key is a meshName.

  map<FIELD_NAME_,MAP_DT_IT_>  _fields;     // We can't have two FIELDs with the same fieldName. 
 
  map<FIELD_ *, MESH_NAME_>    _meshName;   // Get the meshName associated with a FIELD * 
                                           // in order to get the MESH* from _meshes 

  map < MESH_NAME_, map <MED_FR::med_entite_maillage,SUPPORT * > > _support ;
  // For each MESH, we list support for each entity on all elements.

  vector<GENDRIVER *>  _drivers;          // Storage of the MED_MED drivers currently in use

public:

  MED();
  MED (driverTypes driverType, const string & fileName); // Analyse the file <fileName> by calling readFileStruct
  ~MED();
  
  // INUTILE : void addMesh  (const string & meshName  ); // Read the mesh <meshName> found in the file <_fileName>. <_fileName> must be set.
  // INUTILE : void addField (const string & fieldName ); // Pensez au cas ou on ajoute un Field/Mesh avec un driver déjà existant.
  
  void addField (const FIELD_  * const ptrField  ) throw (MEDEXCEPTION) ; // Add an existing FIELD object to the FIELD list, the meshName is given by the FIELD object.
  void addMesh  (const MESH   * ptrMesh   ) throw (MEDEXCEPTION) ; // Add an existing MESH object to the MESH list, the meshName is given by the MESH object.
 
  // ------  Drivers Management Part
protected:

  class INSTANCE {
  public:
    virtual GENDRIVER * run(const string & fileName, MED * const ptrMed) const = 0 ;
  } ;
  
  template <class T> class INSTANCE_DE : public INSTANCE {
  public :
    GENDRIVER * run(const string & fileName,  MED * const ptrMed) const { return new T(fileName,ptrMed) ; }
  } ;
  
  static INSTANCE_DE<MED_MED_DRIVER> inst_med ;
  //static INSTANCE_DE<VTK_DRIVER>   inst_vtk   ;
  static const INSTANCE * const instances[] ;

public:

  int  addDriver     (driverTypes driverType, const string & fileName);
  void rmDriver      (int index=0);
  void readFileStruct(int index=0);
  // void read          (int index=0);
  void writeFrom     (int index=0); 
  void write         (int index=0); 
  
  // ------ End Of Drivers Management Part

  int        getNumberOfMeshes ( void ) const;       
  int        getNumberOfFields ( void ) const;       
  void       getMeshNames      ( string * meshNames         ) const;
  deque<string>       getMeshNames      () const;
  MESH     * getMesh           ( const string & meshName    ) const;
  MESH     * getMesh           ( const  FIELD_ *  field     ) const;
  void       getFieldNames     ( string * fieldNames        ) const;
  deque<string>       getFieldNames     () const;
  // A FAIRE DES ROUTINES QUI DONNENT LES PDT ET ITE
  deque<DT_IT_>       getFieldIteration (const string & fieldName) const ;
  FIELD_   * getField          ( const string & fieldName,
                                 const int dt,  const int it) const;

  const map<MED_FR::med_entite_maillage,SUPPORT *> & getSupports(const string & meshName) const;
  SUPPORT *  getSupport (const string & meshName,MED_FR::med_entite_maillage entity) const ;

  void       updateSupport () ;

  // GERER LE CAS DE L'APPARITION DES MEMES NOMS DS DES FICHIERS <> !!!!!
 
  //friend ostream & operator<<(ostream &os,const MED & med);

};

#endif

//REM : AJOUTER DS LES DRIVERS des attributes INTERLACE, NO INTERLACE .........
//      AJOUTER L'OPERATEUR DE RECOPIE
