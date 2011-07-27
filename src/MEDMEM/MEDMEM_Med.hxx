//  Copyright (C) 2007-2011  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

# ifndef MED_HXX
# define MED_HXX

#include <MEDMEM.hxx>

// STL
# include <string>
# include <map>
# include <vector>
# include <deque>

// LOCAL
# include "MEDMEM_define.hxx"
# include "MEDMEM_Exception.hxx"
# include "MEDMEM_GenDriver.hxx"
# include "MEDMEM_Field.hxx"

namespace MEDMEM {
class MESH;
class FIELD_;
class SUPPORT ;

typedef string MESH_NAME_;
typedef string FIELD_NAME_;
typedef struct { int dt; int it; } DT_IT_;
struct MEDMEM_EXPORT LT_DT_IT_
{
  bool operator() (const DT_IT_ &p1, const DT_IT_ &p2) const
  {
    if ( p1.dt == p2.dt)
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

class MEDMEM_EXPORT MED
{
  // Add your personnal driver line (step 2)
  friend class MED_MED_RDONLY_DRIVER21;
  friend class MED_MED_WRONLY_DRIVER21;
  friend class MED_MED_RDWR_DRIVER21;
  friend class MED_MED_RDONLY_DRIVER22;
  friend class MED_MED_WRONLY_DRIVER22;
  friend class MED_MED_RDWR_DRIVER22;

private:

  map<MESH_NAME_,MESH*>        _meshes;     // We can't have two MESHes with the same meshName.
                                            // The string key is a meshName.

  map<FIELD_NAME_,MAP_DT_IT_>  _fields;     // We can't have two FIELD_s with the same fieldName.

  map<FIELD_ *, MESH_NAME_>    _meshName;   // Get the meshName associated with a FIELD_ *
                                            // in order to get the MESH* from _meshes

  // POURQUOI MED_FR::med_entite_maillage ? devrait être MED_EN !
  map < MESH_NAME_, map < MED_EN::medEntityMesh, SUPPORT * > > _support ;
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
  void updateNamesInMaps();
  int  addDriver     (driverTypes driverType,
                      const string & fileName,
                      MED_EN::med_mode_acces access=MED_EN::RDWR);
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
  template<class T>
  FIELD<T> * getFieldT         ( const string & fieldName,
                                 const int dt,  const int it) const throw (MEDEXCEPTION) ;

  FIELD_   * getField2          ( const string & fieldName,
                                 double time, int it=0) const throw (MEDEXCEPTION) ;

  const map<MED_EN::medEntityMesh,SUPPORT *> & getSupports(const string & meshName) const throw (MEDEXCEPTION) ;

  SUPPORT *  getSupport (const string & meshName,MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION) ;

  void       updateSupport () ;

  // GERER LE CAS DE L'APPARITION DES MEMES NOMS DS DES FICHIERS <> !!!!!

  //friend ostream & operator<<(ostream &os,const MED & med);

};

template<class T>
FIELD<T> * MED::getFieldT( const string & fieldName, const int dt,  const int it) const throw (MEDEXCEPTION)
{
  const char *LOC="Unexpected type of field";
  FIELD_* retUp=getField(fieldName,dt,it);
  FIELD<T> *ret=dynamic_cast< FIELD<T> * >(retUp);
  if(!ret)
    throw MED_EXCEPTION ( LOCALIZED( STRING(LOC) ) );
  return ret;
}

}

#endif

//REM : AJOUTER DS LES DRIVERS des attributes INTERLACE, NO INTERLACE .........
//      AJOUTER L'OPERATEUR DE RECOPIE
