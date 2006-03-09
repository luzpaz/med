#include <stdlib.h>
#include <string>

#define  INTERLACING_TAG FullInterlace

#include "MEDMEM_Exception.hxx"
#include "MEDMEM_define.hxx"
#include "MEDMEM_GaussLocalization.hxx"
#include "MEDMEM_ArrayInterface.hxx"

using namespace std;
using namespace MEDMEM;
using namespace MED_EN;

#define _a 0.446948490915965
#define _b 0.091576213509771
#define _p1 0.11169079483905
#define _p2 0.0549758718227661

int main (int argc, char ** argv) {

  typedef MEDMEM_ArrayInterface<double,INTERLACING_TAG,NoGauss>::Array Array;


 /* Caract�ristiques du model n� 1 de localisation des points de gauss pour le champ n�1*/
  MED_EN::medGeometryElement typeGeo = MED_TRIA6;

  int       ngauss1_1 = 6;
  string    gauss1_1("Model n1");
  double    refcoo1 [12] = { -1.0,1.0, -1.0,-1.0, 1.0,-1.0, -1.0,0.0, 0.0,-1.0, 0.0,0.0 };

  double    gscoo1_1[12] = { 2*_b-1, 1-4*_b, 2*_b-1, 2*_b-1, 1-4*_b,
			     2*_b-1, 1-4*_a, 2*_a-1, 2*_a-1, 1-4*_a, 2*_a-1, 2*_a-1 };
  double    wg1_1[6]     = { 4*_p2, 4*_p2, 4*_p2, 4*_p1, 4*_p1, 4*_p1 };


  assert( ( (typeGeo%100)*(typeGeo/100) ) == 12 );

  GAUSS_LOCALIZATION<INTERLACING_TAG> myloc1 = GAUSS_LOCALIZATION<INTERLACING_TAG>(gauss1_1,typeGeo,ngauss1_1,
										   Array(refcoo1,typeGeo/100,(typeGeo%100) ),
										   Array(gscoo1_1,typeGeo/100,ngauss1_1),
										   vector<double> (wg1_1,wg1_1+ngauss1_1)
										   );
  GAUSS_LOCALIZATION<INTERLACING_TAG> myloc1bis = GAUSS_LOCALIZATION<INTERLACING_TAG>(gauss1_1,typeGeo,ngauss1_1,
										      refcoo1,gscoo1_1,
										      wg1_1);

  cout << "myloc1 == myloc1bis " << (myloc1 == myloc1bis) << endl;
  cout << "myloc1 : " << endl << myloc1 << endl;
}
