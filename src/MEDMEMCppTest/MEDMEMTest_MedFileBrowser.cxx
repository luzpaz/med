// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "MEDMEMTest.hxx"
#include <cppunit/TestAssert.h>

#include "MEDMEM_MedFileBrowser.hxx"

// #include <sstream>
// #include <cmath>

#ifdef WIN32
#include <windows.h>
#endif

// use this define to enable lines, execution of which leads to Segmentation Fault
//#define ENABLE_FAULTS

// use this define to enable CPPUNIT asserts and fails, showing bugs
//#define ENABLE_FORCED_FAILURES

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

// #30: MEDMEM_MedFileBrowser.hxx  }  MEDMEMTest_MedFileBrowser.cxx

/*!
 *  Check methods defined in MEDMEM_MedFileBrowser.hxx:
 *  class MEDFILEBROWSER {
 *   (+) MEDFILEBROWSER();
 *   (+) MEDFILEBROWSER (const string & fileName) throw (MEDEXCEPTION);
 *   (+) void readFileStruct(const string & fileName) throw (MEDEXCEPTION);
 *   (+) std::string  getFileName() const;
 *   (+) int  getNumberOfMeshes (void) const;
 *   (+) int  getNumberOfFields (void) const;
 *   (+) void getMeshNames (string * meshNames) const throw (MEDEXCEPTION);
 *   (+) vector<string> getMeshNames   () const;
 *   (+) void getFieldNames (string * fieldNames) const throw (MEDEXCEPTION);
 *   (+) vector<string> getFieldNames () const;
 *   (+) bool isStructuredMesh(const std::string & meshName) const throw (MEDEXCEPTION);
 *   (+) MED_EN::med_type_champ getFieldType (const std::string & fieldName) const throw (MEDEXCEPTION) ;
 *   (+) std::string getMeshName (const std::string & fieldName) const throw (MEDEXCEPTION) ;
 *   (+) vector<DT_IT_> getFieldIteration (const string & fieldName) const throw (MEDEXCEPTION);
 *  }
 */
namespace
{
  void check_bad_file( MEDFILEBROWSER& myMed )
  {
    vector<string> names(1);
    CPPUNIT_ASSERT_EQUAL( 0, myMed.getNumberOfMeshes() );
    CPPUNIT_ASSERT_EQUAL( 0, myMed.getNumberOfFields() );
    CPPUNIT_ASSERT_NO_THROW( myMed.getMeshNames( & names[0] ));
    CPPUNIT_ASSERT_NO_THROW( myMed.getFieldNames( & names[0] ));
    CPPUNIT_ASSERT_THROW( myMed.isStructuredMesh( "meshName" ), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW( myMed.getFieldType( "fieldName" ), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW( myMed.getMeshName( "fieldName" ), MEDEXCEPTION);
    CPPUNIT_ASSERT_THROW( myMed.getFieldIteration( "" ), MEDEXCEPTION);
  }
}
// CppUnit::assertion_traits<> to use CPPUNIT_ASSERT_EQUAL for VEC_DT_IT_
namespace CppUnit
{
  struct EQ_DT_IT_
  {
    bool operator() (const DT_IT_ &p1, const DT_IT_ &p2) const
    {
      return p1.dt == p2.dt && p1.it == p2.it;
    }
  };
  template <>
  struct assertion_traits<MEDMEM::VEC_DT_IT_>
  {  
    static bool equal( MEDMEM::VEC_DT_IT_ x, MEDMEM::VEC_DT_IT_ y )
    {
      return x.size() == y.size() && std::equal(x.begin(),x.end(), y.begin(), EQ_DT_IT_());
    }

    static std::string toString( const MEDMEM::VEC_DT_IT_& x )
    {
      MEDMEM::STRING ost("{ ");
      for ( unsigned i = 0; i < x.size(); ++i )
        ost << "{ " << x[i].dt << ", " << x[i].it << " }" << ( i+1==x.size() ? " " : ", ");
      return ost << "}";
    }
  };
}

void MEDMEMTest::testMedFileBrowser()
{
  string filename = getResourceFile("pointe.med");
  string filename_21 = getResourceFile("pointe_V21.med");
  string filename_inexist = "InexistentFile.med";
  string filename_otherFormat = getResourceFile("test_2D.sauve");

  vector<string> names;

  // 1. constructor MEDFILEBROWSER()
  // --------------------------------
  MEDFILEBROWSER myMed;
  check_bad_file( myMed );

  // 2. Constructor MEDFILEBROWSER (const std::string & fileName) throw (MEDEXCEPTION)
  // ----------------------------------------------------------------------------------

  // Existent med file (pointe.med)
  CPPUNIT_ASSERT_NO_THROW( MEDFILEBROWSER myMed( filename ));

  // Inexistent med file
  CPPUNIT_ASSERT_THROW( MEDFILEBROWSER myMed( filename_inexist ), MEDEXCEPTION); 

  // Unsupported version med file
  CPPUNIT_ASSERT_THROW( MEDFILEBROWSER myMed( filename_21 ), MEDEXCEPTION); 

  // Not med file
  CPPUNIT_ASSERT_THROW( MEDFILEBROWSER myMed( filename_otherFormat ), MEDEXCEPTION); 

  // 3. void readFileStruct(const std::string & fileName) throw (MEDEXCEPTION)
  // --------------------------------------------------------------------------
  MEDFILEBROWSER pointe_med, empty_med, ko_med;
  
  // Inexistent med file
  CPPUNIT_ASSERT_THROW( ko_med.readFileStruct( filename_inexist ), MEDEXCEPTION);
  check_bad_file( ko_med );

  // Unsupported version med file
  CPPUNIT_ASSERT_THROW( ko_med.readFileStruct( filename_21 ), MEDEXCEPTION);
  check_bad_file( ko_med );

  // Not med file
  CPPUNIT_ASSERT_THROW( ko_med.readFileStruct( filename_otherFormat ), MEDEXCEPTION);
  check_bad_file( ko_med );

  // Existent med file (pointe.med)
  CPPUNIT_ASSERT_NO_THROW( pointe_med.readFileStruct( filename ));

  // 4. std::string  getFileName() const
  // ------------------------------------
  CPPUNIT_ASSERT( empty_med.getFileName().empty() );

  CPPUNIT_ASSERT_EQUAL( filename_otherFormat, ko_med.getFileName() );

  CPPUNIT_ASSERT_EQUAL( filename, pointe_med.getFileName() );

  // 5. int getNumberOfMeshes ( void ) const
  // ----------------------------------------
  int nbMeshes;
  CPPUNIT_ASSERT_NO_THROW( nbMeshes = empty_med.getNumberOfMeshes());
  CPPUNIT_ASSERT_EQUAL( 0, nbMeshes );
  CPPUNIT_ASSERT_NO_THROW( nbMeshes = ko_med.getNumberOfMeshes());
  CPPUNIT_ASSERT_EQUAL( 0, nbMeshes );
  CPPUNIT_ASSERT_NO_THROW( nbMeshes = pointe_med.getNumberOfMeshes());
  CPPUNIT_ASSERT_EQUAL( 1, nbMeshes );

  // 6. int        getNumberOfFields ( void ) const;
  // ------------------------------------------------
  int nbFields;
  CPPUNIT_ASSERT_NO_THROW( nbFields = empty_med.getNumberOfFields());
  CPPUNIT_ASSERT_EQUAL( 0, nbFields );
  CPPUNIT_ASSERT_NO_THROW( nbFields = ko_med.getNumberOfFields());
  CPPUNIT_ASSERT_EQUAL( 0, nbFields );
  CPPUNIT_ASSERT_NO_THROW( nbFields = pointe_med.getNumberOfFields());
  CPPUNIT_ASSERT_EQUAL( 4, nbFields );

  // 7. void       getMeshNames      ( std::string * meshNames ) const;
  // -------------------------------------------------------------------
  names.resize(2,"");
  string emptyStr, maa1("maa1");

  CPPUNIT_ASSERT_NO_THROW( empty_med.getMeshNames( & names[0] ));
  CPPUNIT_ASSERT_EQUAL( emptyStr, names[0] );
  CPPUNIT_ASSERT_NO_THROW( ko_med.getMeshNames( & names[0] ));
  CPPUNIT_ASSERT_EQUAL( emptyStr, names[0] );
  CPPUNIT_ASSERT_NO_THROW( pointe_med.getMeshNames( & names[0] ));
  CPPUNIT_ASSERT_EQUAL( maa1, names[0] );
  CPPUNIT_ASSERT_EQUAL( emptyStr, names[1] );
  
  //8. void       getFieldNames     ( std::string * fieldNames ) const;
  // -------------------------------------------------------------------
  names.clear();
  names.resize(5,"");
  string
    fieldcelldoublescalar("fieldcelldoublescalar"),
    fieldcelldoublevector("fieldcelldoublevector" ),
    fieldnodedouble("fieldnodedouble"       ),
    fieldnodeint("fieldnodeint"          );
    
  
  CPPUNIT_ASSERT_NO_THROW( empty_med.getFieldNames( & names[0] ));
  CPPUNIT_ASSERT_EQUAL( emptyStr, names[0] );
  CPPUNIT_ASSERT_NO_THROW( ko_med.getFieldNames( & names[0] ));
  CPPUNIT_ASSERT_EQUAL( emptyStr, names[0] );
  CPPUNIT_ASSERT_NO_THROW( pointe_med.getFieldNames( & names[0] ));
  CPPUNIT_ASSERT_EQUAL( fieldcelldoublescalar, names[0] );
  CPPUNIT_ASSERT_EQUAL( fieldcelldoublevector, names[1] );
  CPPUNIT_ASSERT_EQUAL( fieldnodedouble      , names[2] );
  CPPUNIT_ASSERT_EQUAL( fieldnodeint         , names[3] );
  CPPUNIT_ASSERT_EQUAL( emptyStr, names[4] );

  //9. std::vector< std::string > getMeshNames () const;
  // ----------------------------------------------------
  CPPUNIT_ASSERT_NO_THROW( names = empty_med.getMeshNames() );
  CPPUNIT_ASSERT_EQUAL(0, int( names.size() ));
  CPPUNIT_ASSERT_NO_THROW( names = ko_med.getMeshNames() );
  CPPUNIT_ASSERT_EQUAL(0, int( names.size() ));
  CPPUNIT_ASSERT_NO_THROW( names = pointe_med.getMeshNames() );
  CPPUNIT_ASSERT_EQUAL(1, int( names.size() ));
  CPPUNIT_ASSERT_EQUAL(maa1, names[0] );

  // 10. std::vector< std::string > getFieldNames() const;
  // ------------------------------------------------------
  CPPUNIT_ASSERT_NO_THROW( names = empty_med.getFieldNames() );
  CPPUNIT_ASSERT_EQUAL(0, int( names.size() ));
  CPPUNIT_ASSERT_NO_THROW( names = ko_med.getFieldNames() );
  CPPUNIT_ASSERT_EQUAL(0, int( names.size() ));
  CPPUNIT_ASSERT_NO_THROW( names = pointe_med.getFieldNames() );
  CPPUNIT_ASSERT_EQUAL(4, int( names.size() ));
  CPPUNIT_ASSERT_EQUAL( fieldcelldoublescalar, names[0] );
  CPPUNIT_ASSERT_EQUAL( fieldcelldoublevector, names[1] );
  CPPUNIT_ASSERT_EQUAL( fieldnodedouble      , names[2] );
  CPPUNIT_ASSERT_EQUAL( fieldnodeint         , names[3] );

  // 11. bool isStructuredMesh(const std::string & meshName) const throw (MEDEXCEPTION);
  // ------------------------------------------------------------------------------------

  CPPUNIT_ASSERT_THROW( empty_med. isStructuredMesh( "meshName" ), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW( ko_med.    isStructuredMesh( "meshName" ), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW( pointe_med.isStructuredMesh( "meshName" ), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW( pointe_med.isStructuredMesh( "maa1" ));
  CPPUNIT_ASSERT( !pointe_med.isStructuredMesh( "maa1" ));

  // 12. med_type_champ getFieldType (const string & fieldName) const throw (MEDEXCEPTION) ;
  // ----------------------------------------------------------------------------------------
  CPPUNIT_ASSERT_THROW( empty_med.getFieldType( "fieldName" ), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW( pointe_med.getFieldType( "fieldName" ), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW( pointe_med.getFieldType( "fieldnodeint" ));
  CPPUNIT_ASSERT_EQUAL( MED_REEL64, pointe_med.getFieldType( "fieldcelldoublescalar" ));
  CPPUNIT_ASSERT_EQUAL( MED_REEL64, pointe_med.getFieldType( "fieldcelldoublevector" ));
  CPPUNIT_ASSERT_EQUAL( MED_INT32, pointe_med.getFieldType( "fieldnodeint" ));

  // 13. string getMeshName (const std::string & fieldName) const throw (MEDEXCEPTION) ;
  // ------------------------------------------------------------------------------------
  CPPUNIT_ASSERT_THROW( empty_med.getMeshName( "fieldnodeint" ), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW( ko_med.getMeshName( "fieldnodeint" ), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW( pointe_med.getMeshName( "fieldName" ), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW( pointe_med.getMeshName( "fieldnodeint" ));
  names = pointe_med.getFieldNames();
  for ( unsigned i = 0; i < names.size(); ++i )
    CPPUNIT_ASSERT_EQUAL( maa1, pointe_med.getMeshName( names[i] ));

  // 14. VEC_DT_IT_  getFieldIteration (const std::string & fieldName) const throw (MEDEXCEPTION)
  // ---------------------------------------------------------------------------------------------
  CPPUNIT_ASSERT_THROW( empty_med.getFieldIteration( "fieldnodeint" ), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW( ko_med.getFieldIteration( "fieldnodeint" ), MEDEXCEPTION);
  CPPUNIT_ASSERT_THROW( pointe_med.getFieldIteration( "" ), MEDEXCEPTION);
  CPPUNIT_ASSERT_NO_THROW( pointe_med.getFieldIteration( "fieldnodeint" ));

  VEC_DT_IT_ vec1dtit( 1 );
  vec1dtit[0].dt = -1;
  vec1dtit[0].it = -1;
  CPPUNIT_ASSERT_EQUAL( vec1dtit, pointe_med.getFieldIteration( "fieldcelldoublescalar" ) );
  CPPUNIT_ASSERT_EQUAL( vec1dtit, pointe_med.getFieldIteration( "fieldcelldoublevector" ) );
  CPPUNIT_ASSERT_EQUAL( vec1dtit, pointe_med.getFieldIteration( "fieldnodeint" ) );
  VEC_DT_IT_ vec3dtit( 3, vec1dtit[0] );
  vec3dtit[1].dt = 1;
  vec3dtit[2].dt = 2;
  CPPUNIT_ASSERT_EQUAL( vec3dtit, pointe_med.getFieldIteration( "fieldnodedouble" ) );
}
