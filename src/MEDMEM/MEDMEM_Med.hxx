# ifndef MED_HXX
# define MED_HXX

// STL
# include <string>
# include <map>
# include <vector>
# include <deque>

// LOCAL
# include "MEDMEM_define.hxx"

// Add your own driver header (step 2)
# include "MEDMEM_MedMedDriver.hxx"
# include "MEDMEM_VtkMedDriver.hxx"

# include "MEDMEM_Exception.hxx"
using namespace MED_EN;


namespace MEDMEM {
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
//

/*!

  This class is use to group together some MESH, SUPPORT and FIELD
  objects.

*/

class MED
{
  // Add your personnal driver line (step 2)
  friend class MED_MED_RDONLY_DRIVER;
  friend class MED_MED_WRONLY_DRIVER;
  friend class MED_MED_RDWR_DRIVER;

private:

  map<MESH_NAME_,MESH*>        _meshes;     // We can't have two MESHes with the same meshName.  
                                            // The string key is a meshName.

  map<FIELD_NAME_,MAP_DT_IT_>  _fields;     // We can't have two FIELD_s with the same fieldName. 
 
  map<FIELD_ *, MESH_NAME_>    _meshName;   // Get the meshName associated with a FIELD_ * 
                                            // in order to get the MESH* from _meshes 

  // POURQUOI MED_FR::med_entite_maillage ? devrait être MED_EN !
  map < MESH_NAME_, map <MED_FR::med_entite_maillage,SUPPORT * > > _support ;
  // For each MESH, we list support for each entity on all elements.

  vector<GENDRIVER *>  _drivers;          // Storage of the MED_MED drivers currently in use

public:

  MED();
  MED (driverTypes driverType, const string & fileName); // Analyse the file <fileName> by calling readFileStruct
  ~MED();
  
  // INUTILE : void addMesh  (const string & meshName  ); // Read the mesh <meshName> found in the file <_fileName>. <_fileName> must be set.
  // INUTILE : void addField (const string & fieldName ); // Pensez au cas ou on ajoute un Field/Mesh avec un driver déjà existant.
  
  void addField ( FIELD_  * const ptrField  ) throw (MED_EXCEPTION) ;
  void addMesh  ( MESH    * const ptrMesh   ) throw (MED_EXCEPTION) ;

  // ------  Drivers Management Part
protected:

  class INSTANCE {
  public:
    virtual GENDRIVER * run(const string & fileName, MED * const ptrMed) const = 0 ;
  } ;
  
  template <class T> class INSTANCE_DE : public INSTANCE {
  public :
    GENDRIVER * run(const string & fileName,  MED * const ptrMed) const 
    { 
      MESSAGE("GENDRIVER * run") ;
      return new T(fileName,ptrMed) ; 
    }
  } ;
  
  // Add your new driver instance here (step 3)
  static INSTANCE_DE<MED_MED_RDWR_DRIVER> inst_med ;
  static INSTANCE_DE<VTK_MED_DRIVER> inst_vtk ;
  static const INSTANCE * const instances[] ;

public:

  int  addDriver     (driverTypes driverType, const string & fileName);
  int  addDriver     (GENDRIVER & driver);
  void rmDriver      (int index=0) throw (MEDEXCEPTION) ;

  void readFileStruct(int index=0) throw (MEDEXCEPTION) ;
  void read          (int index=0) throw (MEDEXCEPTION) ;
  void writeFrom     (int index=0) throw (MEDEXCEPTION) ; 
  void write         (int index=0) throw (MEDEXCEPTION) ; 
  
  // ------ End Of Drivers Management Part

  int        getNumberOfMeshes ( void ) const;       
  int        getNumberOfFields ( void ) const;       
  void       getMeshNames      ( string * meshNames ) const throw (MEDEXCEPTION) ;
  deque<string> getMeshNames   () const;
  MESH     * getMesh           ( const string & meshName ) const throw (MEDEXCEPTION) ;
  MESH     * getMesh           ( const  FIELD_ * const field ) const throw (MEDEXCEPTION) ;
  void       getFieldNames     ( string * fieldNames        ) const throw (MEDEXCEPTION) ;
  deque<string> getFieldNames  () const;
  // A FAIRE DES ROUTINES QUI DONNENT LES PDT ET ITE
  deque<DT_IT_> getFieldIteration (const string & fieldName) const throw (MEDEXCEPTION) ;
  FIELD_   * getField          ( const string & fieldName,
                                 const int dt,  const int it) const throw (MEDEXCEPTION) ;

  const map<MED_FR::med_entite_maillage,SUPPORT *> & getSupports(const string & meshName) const throw (MEDEXCEPTION) ;

  SUPPORT *  getSupport (const string & meshName,MED_FR::med_entite_maillage entity) const throw (MEDEXCEPTION) ;

  void       updateSupport () ;

  // GERER LE CAS DE L'APPARITION DES MEMES NOMS DS DES FICHIERS <> !!!!!
 
  //friend ostream & operator<<(ostream &os,const MED & med);

};
};

#endif

//REM : AJOUTER DS LES DRIVERS des attributes INTERLACE, NO INTERLACE .........
//      AJOUTER L'OPERATEUR DE RECOPIE
