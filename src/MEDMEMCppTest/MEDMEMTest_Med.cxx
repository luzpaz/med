//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
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
#include "MEDMEMTest.hxx"
#include <cppunit/TestAssert.h>

#include "MEDMEM_Med.hxx"

#include <sstream>
#include <cmath>

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

// #30: MEDMEM_Med.hxx  }  MEDMEMTest_Med.cxx

/*!
 *  Check methods (26), defined in MEDMEM_Med.hxx:
 *  class MED {
 *   (+) MED();
 *   (+) MED (driverTypes driverType, const string & fileName);
 *   (+) ~MED();
 *   (+) void addField (FIELD_ * const ptrField) throw (MED_EXCEPTION);
 *   (+) void addMesh  (MESH   * const ptrMesh) throw (MED_EXCEPTION);
 *   (+) int addDriver (driverTypes driverType, const string & fileName,
 *                          MED_EN::med_mode_acces access=MED_EN::MED_REMP);
 *   (+) int addDriver (GENDRIVER & driver);
 *   (+) void rmDriver (int index=0) throw (MEDEXCEPTION);
 *   (+) void readFileStruct(int index=0) throw (MEDEXCEPTION);
 *   (+) void read (int index=0) throw (MEDEXCEPTION);
 *   (+) void writeFrom (int index=0) throw (MEDEXCEPTION);
 *   (+) void write (int index=0) throw (MEDEXCEPTION);
 *   (+) int  getNumberOfMeshes (void) const;
 *   (+) int  getNumberOfFields (void) const;
 *   (+) void getMeshNames (string * meshNames) const throw (MEDEXCEPTION);
 *   (+) deque<string> getMeshNames   () const;
 *   (+) MESH * getMesh (const string & meshName) const throw (MEDEXCEPTION);
 *   (+) MESH * getMesh (const  FIELD_ * const field) const throw (MEDEXCEPTION);
 *   (+) void getFieldNames (string * fieldNames) const throw (MEDEXCEPTION);
 *   (+) deque<string> getFieldNames () const;
 *   (+) deque<DT_IT_> getFieldIteration (const string & fieldName) const throw (MEDEXCEPTION);
 *   (+) FIELD_ * getField (const string & fieldName, const int dt,
 *                              const int it) const throw (MEDEXCEPTION);
 *   (NOT COMPILABLE!!!) template<class T> FIELD<T> * getFieldT
 *           (const string & fieldName, const int dt,  const int it) const throw (MEDEXCEPTION);
 *   (+) FIELD_ * getField2 (const string & fieldName, double time, int it=0) const throw (MEDEXCEPTION);
 *   (+) const map<MED_EN::medEntityMesh,SUPPORT *> & getSupports
 *           (const string & meshName) const throw (MEDEXCEPTION);
 *   (+) SUPPORT *  getSupport (const string & meshName,
 *                                  MED_EN::medEntityMesh entity) const throw (MEDEXCEPTION);
 *   (-, as it is temporary and called from driver after Med object reading from file) void updateSupport ();
 *  }
 */
void MEDMEMTest::testMed()
{
  string datadir  = getenv("MED_ROOT_DIR");
  string filename = datadir + "/share/salome/resources/med/pointe.med";

  string tmp_dir  = getenv("TMP");
  if (tmp_dir == "")
    tmp_dir = "/tmp";

  string filenameout21      = tmp_dir  + "/myMedWrite_pointe21.med";
  string filenameout21_from = tmp_dir  + "/myMedWrite_pointe21_from.med";

  // To remove tmp files from disk
  MEDMEMTest_TmpFilesRemover aRemover;
  aRemover.Register(filenameout21);
  aRemover.Register(filenameout21_from);

  MED* myMed = new MED(MED_DRIVER, filename);
  //read all objects in the file
  CPPUNIT_ASSERT_NO_THROW(myMed->read());
  //#ifdef ENABLE_FORCED_FAILURES
  // (BUG) Memory problem in MED::updateSupport()!
  // Invalid read of size 4
  //    at 0x34548AD6: std::_Rb_tree_decrement(std::_Rb_tree_node_base*) (in /usr/lib/libstdc++.so.6.0.1)
  //    by 0x34876722: std::_Rb_tree_iterator<std::pair<long const, MEDMEM::SUPPORT*> >::operator--(int) (stl_tree.h:203)
  //    by 0x348733BE: MEDMEM::MED::updateSupport() (MEDMEM_Med.cxx:805)
  //  Address 0x35C67598 is 0 bytes inside a block of size 24 free'd
  //    at 0x3414CBD5: operator delete(void*) (vg_replace_malloc.c:155)
  //    ...
  //    by 0x348766EB: std::map<long, MEDMEM::SUPPORT*, std::less<long>, std::allocator<std::pair<long const, MEDMEM::SUPPORT*> > >::erase(std::_Rb_tree_iterator<std::pair<long const, MEDMEM::SUPPORT*> >) (stl_map.h:410)
  //    by 0x348733A3: MEDMEM::MED::updateSupport() (MEDMEM_Med.cxx:804)
  //CPPUNIT_FAIL("Memory problem in MED::updateSupport(): removing map item while iterating on this map.");

  // Seems to be fixed - to check, BUT THERE ARE OTHER PROBLEMS REPORTED BY VALGRIND
  //#endif

  //getNumberOfMeshes
  int nbMeshes, nbFields;
  try{
    nbMeshes = myMed->getNumberOfMeshes();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  CPPUNIT_ASSERT(nbMeshes);

  //getMeshNames
  deque<string> mesh_names, field_names;
  try{
    mesh_names = myMed->getMeshNames();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  CPPUNIT_ASSERT(mesh_names.size() != 0);

  //getNumberOfFields
  try{
    nbFields = myMed->getNumberOfFields();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  CPPUNIT_ASSERT(nbFields);

  //getFieldNames
  try{
    field_names = myMed->getFieldNames();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  CPPUNIT_ASSERT(field_names.size() != 0);

  string field_names_1[nbFields];
  string mesh_names_1[nbMeshes];
  //get field and mesh names
  try{
    myMed->getMeshNames(mesh_names_1);
    myMed->getFieldNames(field_names_1);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  // Ensure what it's impossible to get mesh object by empty or non-existent name
  CPPUNIT_ASSERT_THROW(myMed->getMesh(""), MEDEXCEPTION);

  //compare mesh names, produced by two different methods
  for(int nb = 0; nb < nbMeshes; nb++ )
  {
    MESH* myMesh;
    CPPUNIT_ASSERT(mesh_names_1[nb] == mesh_names[nb]);
    CPPUNIT_ASSERT_NO_THROW(myMesh = myMed->getMesh(mesh_names[nb]));
    //ensure mesh is not empty
    CPPUNIT_ASSERT(myMesh->getSpaceDimension() != MED_INVALID);
    CPPUNIT_ASSERT(myMesh->getNumberOfNodes() != MED_INVALID);
    CPPUNIT_ASSERT(myMesh->getCoordinateptr() != NULL);
  }

  // Ensure what it's impossible to get field object by empty or non-existent name
  CPPUNIT_ASSERT_THROW(myMed->getField("", 0, 0),MEDEXCEPTION);

  // Cycle by amount of fields
  // Compare field names produced by two different methods
  // Get field object with help of getField(...) and getField2(...)
  for(int nb = 0; nb < nbFields; nb++ )
  {
    CPPUNIT_ASSERT(field_names_1[nb] == field_names[nb]);
    deque<DT_IT_> myFIter;
    CPPUNIT_ASSERT_NO_THROW(myFIter = myMed->getFieldIteration(field_names[nb]));
    CPPUNIT_ASSERT(myFIter.size());
    for(int nbIter = 0; nbIter < myFIter.size(); nbIter++)
    {
      FIELD_ *myField, *myField2;
      CPPUNIT_ASSERT_NO_THROW(myField  = myMed->getField (field_names[nb], myFIter[nbIter].dt, myFIter[nbIter].it));
      CPPUNIT_ASSERT_NO_THROW(myField2 = myMed->getField2(field_names[nb], myField->getTime()));
      CPPUNIT_ASSERT(myField==myField2);
      //compare two fields
      CPPUNIT_ASSERT(myField->getNumberOfComponents() == myField2->getNumberOfComponents());
      CPPUNIT_ASSERT(myField->getNumberOfValues() == myField2->getNumberOfValues());
      CPPUNIT_ASSERT(myField->getSupport() == myField2->getSupport());
      CPPUNIT_ASSERT(myField->getValueType() == myField2->getValueType());
      CPPUNIT_ASSERT(myField->getDescription() == myField2->getDescription());

      med_type_champ type = myField->getValueType();
      switch (type)
      {
      case MED_INT32:
        {
          //#ifdef ENABLE_FORCED_FAILURES
          // (BUG) ERROR in MEDMEM_Med.hxx line 153: FIELD_ retUp=getField(fieldName,dt,it);
          //       But getField returns FIELD_*
          //CPPUNIT_FAIL("Error in MED::getFieldT(const string & fieldName, const int dt,  const int it)");
          // Fixed: * added
          //#endif
          // not compilable
	  //FIELD<int> *myFieldT;
	  //CPPUNIT_ASSERT_NO_THROW(myFieldT = myMed->getFieldT<int>(field_names[nb], myFIter[nbIter].dt, myFIter[nbIter].it))
          // PROBLEM NOT REPRODUCED
	}
        break;
      case MED_REEL64:
	{
          // not compilable
	  //FIELD<double> *myFieldT;
	  //CPPUNIT_ASSERT_NO_THROW(myFieldT = myMed->getFieldT<double>(field_names[nb], myFIter[nbIter].dt, myFIter[nbIter].it))
          // PROBLEM NOT REPRODUCED
	}
        break;
      }
    }
  }

  // For each mesh get a map
  for(int nb = 0; nb < nbMeshes; nb++ )
  {
    map<MED_EN::medEntityMesh,SUPPORT *> myMapSupports;
    CPPUNIT_ASSERT_NO_THROW(myMapSupports = myMed->getSupports(mesh_names[nb]));
    map<MED_EN::medEntityMesh,SUPPORT *>::const_iterator myMapSupportsIter = myMapSupports.begin();
    for( ; myMapSupportsIter != myMapSupports.end(); myMapSupportsIter++ )
    {
      SUPPORT* mySupport;
      CPPUNIT_ASSERT_NO_THROW(mySupport = myMed->getSupport(mesh_names[nb], (*myMapSupportsIter).first));
      CPPUNIT_ASSERT_EQUAL(mySupport, (*myMapSupportsIter).second);
      CPPUNIT_ASSERT(mySupport->deepCompare(*(*myMapSupportsIter).second));
    }
  }

  // add new driver
  int idMedV21;

  try
  {
    idMedV21 = myMed->addDriver(MED_DRIVER,filenameout21);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }

  // write to file
  CPPUNIT_ASSERT_NO_THROW(myMed->write(idMedV21));
  // check, that the file is created on disk
  CPPUNIT_ASSERT(access(filenameout21.data(), F_OK) == 0);

  // writeFrom
  CPPUNIT_ASSERT_THROW(myMed->writeFrom(idMedV21 + 1000), MEDEXCEPTION); // invalid driver index
  int idMedV21_from = myMed->addDriver(MED_DRIVER, filenameout21_from);
  //#ifdef ENABLE_FORCED_FAILURES
  // (BUG) MED::writeFrom(int) always throws MEDEXCEPTION.
  //       ? missed 'else' clause before throw ?
  CPPUNIT_ASSERT_NO_THROW(myMed->writeFrom(idMedV21_from));
  // simple fix
  //#endif
  // check, that the file is created on disk
  //#ifdef ENABLE_FORCED_FAILURES
  // ? (BUG) The file is not created.
  CPPUNIT_ASSERT(access(filenameout21_from.data(), F_OK) == 0);
  // NOT a bug. writeFrom() should be removed from API
  //#endif

  // remove driver from med
  CPPUNIT_ASSERT_NO_THROW(myMed->rmDriver(idMedV21));
  //#ifdef ENABLE_FORCED_FAILURES
  //ERROR: driver withER22::write() : ";
  //index idMedV21 has not been removed
  // Why driver removal is commented out?
  // ensure exception is raised on second attempt to remove driver
  CPPUNIT_ASSERT_THROW(myMed->rmDriver(idMedV21),MEDEXCEPTION);
  // Fixed
  //#endif

  //create empty MED object
  MED* myEmptyMed = new MED();
  // create new driver for med object
  // and add driver to med
  int idDriver;
  GENDRIVER *driver = DRIVERFACTORY::buildDriverForMed(MED_DRIVER,filename,myEmptyMed,RDONLY);
  try{
    idDriver = myEmptyMed->addDriver(*driver);
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  CPPUNIT_ASSERT(idDriver == 0);

  // trying to read file with non-existent index of driver
  // read file and create empty object
  CPPUNIT_ASSERT_NO_THROW(myEmptyMed->readFileStruct());

  //getNumberOfMeshes
  int nbEmptyMeshes, nbEmptyFields;
  try{
    nbEmptyMeshes = myEmptyMed->getNumberOfMeshes();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  CPPUNIT_ASSERT(nbEmptyMeshes == nbMeshes);

  //getMeshNames
  deque<string> empty_mesh_names, empty_field_names;
  try{
    empty_mesh_names = myEmptyMed->getMeshNames();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  CPPUNIT_ASSERT(empty_mesh_names.size() != 0);

  //getNumberOfFields
  try{
    nbEmptyFields = myEmptyMed->getNumberOfFields();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  CPPUNIT_ASSERT(nbFields == nbEmptyFields);

  //getFieldNames
  try{
    empty_field_names = myEmptyMed->getFieldNames();
  }
  catch(MEDEXCEPTION &e)
  {
    CPPUNIT_FAIL(e.what());
  }
  catch( ... )
  {
    CPPUNIT_FAIL("Unknown exception");
  }
  CPPUNIT_ASSERT(empty_field_names.size() != 0);

  //compare mesh names, produced by two different methods
  for(int nb = 0; nb < nbMeshes; nb++ )
  {
    MESH* myMesh;
    CPPUNIT_ASSERT(empty_mesh_names[nb] == mesh_names[nb]);
    CPPUNIT_ASSERT_NO_THROW(myMesh = myEmptyMed->getMesh(empty_mesh_names[nb]));
    //MESH* myMesh1 = myMed->getMesh(mesh_names[nb]);
    CPPUNIT_ASSERT(!myMesh->deepCompare(*(myMed->getMesh(mesh_names[nb]))));
    //ensure mesh is empty
    CPPUNIT_ASSERT(myMesh->getSpaceDimension() == MED_INVALID);
    CPPUNIT_ASSERT(myMesh->getNumberOfNodes() == MED_INVALID);
    CPPUNIT_ASSERT(myMesh->getCoordinateptr() == NULL);
  }

  // trying to add null mesh pointer
  CPPUNIT_ASSERT_THROW(myEmptyMed->addMesh(NULL), MEDEXCEPTION);

  //create a mesh
  MESH * aMesh = MEDMEMTest_createTestMesh();
  // and add it
  CPPUNIT_ASSERT_NO_THROW(myEmptyMed->addMesh(aMesh));

  //compare two meshes
  CPPUNIT_ASSERT(aMesh->deepCompare(*(myEmptyMed->getMesh("meshing"))));

  // add null field, exception should be raised
  CPPUNIT_ASSERT_THROW(myEmptyMed->addField(NULL), MEDEXCEPTION);
  //
  FIELD_* myEmptyField = new FIELD_();
  // add empty field, ensure exception is raised
  CPPUNIT_ASSERT_THROW(myEmptyMed->addField(myEmptyField), MEDEXCEPTION);
  //set field name
  myEmptyField->setName("myField");

  // set support
  SUPPORT* aSupport = new SUPPORT(myEmptyMed->getMesh("meshing"), "Support On Cells");
  myEmptyField->setSupport(aSupport);

  CPPUNIT_ASSERT_NO_THROW(myEmptyMed->addField(myEmptyField));

  CPPUNIT_ASSERT(aSupport->deepCompare(*(myEmptyMed->getSupport("meshing",MED_CELL))));
  CPPUNIT_ASSERT(aMesh->deepCompare(*(myEmptyMed->getMesh("meshing"))));

  delete myMed;
  delete myEmptyMed;
}
