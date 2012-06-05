// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
#include <stdlib.h>
#include <iostream>
#include <stdio.h>

#include "MEDMEM_ArrayInterface.hxx"
#include "MEDMEM_Field.hxx"
#include "MEDMEM_IndexCheckingPolicy.hxx"
#include "MEDMEM_ArrayConvert.hxx"

using namespace MEDMEM;

int main(void) {

  const int mdim = 2;
  const int nbelem1 = 20;

  // TEST n° 1  FullInterlace et NoGauss
  std::cout << std::endl;
  std::cout << "Test de MEDMEM_Array en FullInterlace et NoGauss : " << std::endl;
  std::cout << std::endl;

  bool test1ok = true;
  const double * myArray1Ptr = 0;
  const double array1Ref[]  = { 11 , 12 , 21 , 22 , 31 , 32 , 41 , 42 , 51 , 52 ,
                               61 , 62 , 71 , 72 , 81 , 82 , 91 , 92 , 101 , 102 ,
                               111 , 112 , 121 , 122 , 131 , 132 , 141 , 142 ,
                               151 , 152 , 161 , 162 , 171 , 172 , 181 , 182 ,
                               191 , 192 , 201 , 202 };

  const double array2Ref[] = { 11 , 21 , 31 , 41 , 51 , 61 , 71 , 81 , 91 , 101
                               , 111 , 121 , 131 , 141 , 151 , 161 , 171 , 181
                               , 191 , 201 ,
                               12 , 22 , 32 , 42 , 52 , 62 , 72 , 82 , 92 , 102
                               , 112 , 122 , 132 , 142 , 152 , 162 , 172 , 182
                               , 192 , 202 };

  MEDMEM_Array<double>  myArray1(mdim,nbelem1);

  for (int i =1; i <= nbelem1; i++)
    for (int j =1; j <= mdim; j++)
      myArray1.setIJ(i,j,i*10+j);

  myArray1Ptr = myArray1.getPtr();
  for (int i =0; i < mdim*nbelem1; i++)
        test1ok = test1ok && ( myArray1Ptr[i] == array1Ref[i] );

  std::cout << "- Egualité de myArray1 et de myArray1Ref : " << test1ok << std::endl ;

  test1ok = true;
  MEDMEM_Array<double> myArray1bis(myArray1, false);
  myArray1Ptr = myArray1bis.getPtr();
  for (int i =0; i < mdim*nbelem1; i++)
        test1ok = test1ok && ( myArray1Ptr[i] == array1Ref[i] );
  std::cout << "- Constructeur par recopie en FullInterlace et NoGauss : " << test1ok << std::endl;

  test1ok = true;
  MEDMEM_Array<double> myArray1ter;
  myArray1ter = myArray1;
  myArray1Ptr = myArray1ter.getPtr();
  for (int i =0; i < mdim*nbelem1; i++)
        test1ok = test1ok && ( myArray1Ptr[i] == array1Ref[i] );
  std::cout << "- Opérateur d'affectation en FullInterlace et NoGauss : " << test1ok << std::endl;


  test1ok = true;
  MEDMEM_Array<double> myArray1qua(mdim,nbelem1);
  myArray1Ptr = myArray1qua.getPtr();

  for (int i =1; i <= nbelem1; i++)
    myArray1qua.setRow(i,&array1Ref[(i-1)*mdim]);

  for (int i =0; i < mdim*nbelem1; i++)
        test1ok = test1ok && ( myArray1Ptr[i] == array1Ref[i] );
  std::cout << "- Méthode setRow en FullInterlace et NoGauss : " << test1ok << std::endl;

  std::cout << "- Test de levée d'exception pour getColumn : ";
  try {
    myArray1qua.getColumn(1);
  }
  catch (MEDMEM::MEDEXCEPTION &m) {
    std::cout << m.what() << endl ;
  }

  test1ok = true;
  MEDMEM_Array<double,NoInterlaceNoGaussPolicy> * myArray1cin = ArrayConvert(myArray1);
  myArray1Ptr = myArray1cin->getPtr();
  int elemno = 0;
  for (int j=1; j <= mdim; j++)
    for (int  i=1; i <= nbelem1 ; i++ ) {
      test1ok = test1ok
        && (  myArray1cin->getIJ(i,j) ==  array2Ref[elemno] )
        && (  myArray1Ptr[elemno]     ==  array2Ref[elemno] );
      elemno++;
    }
  std::cout << "- Convertion du mode FullInterlaceNoGaussPolicy au mode NoInterlaceNoGaussPolicy : "
            << test1ok << std::endl;



  // TEST n°2   NoInterlace et NoGauss
  std::cout << std::endl;
  std::cout << "Test de MEDMEM_Array en NoInterlace et NoGauss : " << std::endl;
  std::cout << std::endl;

  bool test2ok = true;
  const double * myArray2Ptr = 0;

  MEDMEM_Array<double,NoInterlaceNoGaussPolicy> myArray2(mdim,nbelem1);

  for (int i =1; i <= nbelem1; i++)
    for (int j =1; j <= mdim; j++)
      myArray2.setIJ(i,j,i*10+j);

  myArray2Ptr = myArray2.getPtr();
  for (int i =0; i < mdim*nbelem1; i++)
        test2ok = test2ok && ( myArray2Ptr[i] == array2Ref[i] );

  std::cout << "- Egualité de myArray2 et de myArray2Ref : " << test2ok << std::endl ;

  test2ok = true;
  MEDMEM_Array<double,NoInterlaceNoGaussPolicy> myArray2bis(myArray2, false);
  myArray2Ptr = myArray2bis.getPtr();
  for (int i =0; i < mdim*nbelem1; i++)
        test2ok = test2ok && ( myArray2Ptr[i] == array2Ref[i] );
  std::cout << "- Constructeur par recopie en NoInterlace et NoGauss : " << test2ok << std::endl;

  test2ok = true;
  MEDMEM_Array<double,NoInterlaceNoGaussPolicy> myArray2ter;
  myArray2ter = myArray2;
  myArray2Ptr = myArray2ter.getPtr();
  for (int i =0; i < mdim*nbelem1; i++)
        test2ok = test2ok && ( myArray2Ptr[i] == array2Ref[i] );
  std::cout << "- Opérateur d'affectation en NoInterlace et NoGauss : " << test2ok << std::endl;

  test2ok = true;
  MEDMEM_Array<double,NoInterlaceNoGaussPolicy> myArray2qua(mdim,nbelem1);
  myArray2Ptr = myArray2qua.getPtr();

  for (int j =1; j <= mdim; j++)
    myArray2qua.setColumn(j,&array2Ref[nbelem1*(j-1)]);

  for (int i =0; i < mdim*nbelem1; i++)
        test2ok = test2ok && ( myArray2Ptr[i] == array2Ref[i] );
  std::cout << "- Méthode setColumn en NoInterlace et NoGauss : " << test2ok << std::endl;

  std::cout << "- Test de levée d'exception pour getRow : " ;
  try {
    myArray2qua.getRow(1);
  }
  catch (MEDMEM::MEDEXCEPTION &m) {
    std::cout << m.what() << endl ;
  }

  test2ok = true;
  MEDMEM_Array<double,FullInterlaceNoGaussPolicy> * myArray2cin = ArrayConvert(myArray2);
  myArray2Ptr = myArray2cin->getPtr();
  elemno = 0;
  for (int  i=1; i <= nbelem1 ; i++ )
    for (int j=1; j <= mdim; j++) {
      test2ok = test2ok
        && (  myArray2cin->getIJ(i,j) ==  array1Ref[elemno] )
        && (  myArray2Ptr[elemno]     ==  array1Ref[elemno] );
      elemno++;
    }
  std::cout << "- Convertion du mode NoInterlaceNoGaussPolicy au mode FullInterlaceNoGaussPolicy : "
            << test2ok << std::endl;


  // TEST n°3   FullInterlace et Gauss
  const int nbelem2 = 5;
  const int nbtypegeo = 2;
  const int nbelgeoc[nbtypegeo+1]   = {1,3,6};
  const int nbgaussgeo[nbtypegeo+1] = {-1,2,3};
 
  std::cout << std::endl;
  std::cout << "Test de MEDMEM_Array en FullInterlace et Gauss : " << std::endl;
  std::cout << std::endl;

  bool test3ok = true;
  const double * myArray3Ptr = 0;
  const double array3Ref[] = {1.11 , 1.12 , 1.21 , 1.22 , 
                              2.11 , 2.12 , 2.21 , 2.22 , 
                              13.11 , 13.12 , 13.21 , 13.22 , 13.31 , 13.32 , 
                              14.11 , 14.12 , 14.21 , 14.22 , 14.31 , 14.32 , 
                              15.11 , 15.12 , 15.21 , 15.22 , 15.31 , 15.32 };

  const double array4Ref[] = { 1.11 , 1.21 , 2.11 , 2.21,
                               13.11 , 13.21 , 13.31 , 14.11 , 14.21 , 14.31 ,
                               15.11 , 15.21 , 15.31 ,
                               1.12 , 1.22 , 2.12 , 2.22 ,
                               13.12 , 13.22 , 13.32 , 14.12 , 14.22 , 14.32 ,
                               15.12 , 15.22 , 15.32 };

  MEDMEM_Array<double,FullInterlaceGaussPolicy> myArray3(mdim,nbelem2,nbtypegeo,nbelgeoc,nbgaussgeo);

  elemno = 1;
   for (int ntyp=1; ntyp <= nbtypegeo; ntyp++ ) {
    for (int  i=0; i < (nbelgeoc[ntyp]-nbelgeoc[ntyp-1]) ; i++ ) {
      for (int k=1; k <= nbgaussgeo[ntyp]; k++)
        for (int j=1; j <= mdim; j++) {
          myArray3.setIJK(elemno,j,k,elemno+(ntyp-1)*10+0.1*k+0.01*j);
        }
      elemno++;
    }
  }
  myArray3Ptr = myArray3.getPtr();
  for (int i =0; i < myArray3.getArraySize(); i++)
    test3ok = test3ok && ( myArray3Ptr[i] == array3Ref[i] );

  std::cout << "- Egualité de myArray3 et de myArray3Ref : " << test3ok << std::endl ;

  test3ok = true;
  MEDMEM_Array<double,FullInterlaceGaussPolicy> myArray3bis(myArray3, false);
  myArray3Ptr = myArray3bis.getPtr();

  elemno = 0;
  for (int  i=1; i < nbelgeoc[nbtypegeo] ; i++ )
    for (int k=1; k <= myArray3bis.getNbGauss(i); k++)
      for (int j=1; j <= mdim; j++) {
        test3ok = test3ok
          && (  myArray3bis.getIJK(i,j,k)    ==  array3Ref[elemno] )
          && (  myArray3Ptr[elemno]          ==  array3Ref[elemno] );
        elemno++;
      }
  std::cout << "- Constructeur par recopie en FullInterlace et Gauss : " << test3ok << std::endl;

  test3ok = true;
  MEDMEM_Array<double,FullInterlaceGaussPolicy> myArray3ter;
  myArray3ter = myArray3; //Opérateur d'affectation
  myArray3Ptr = myArray3ter.getPtr();

  elemno = 0;
  for (int  i=1; i < nbelgeoc[nbtypegeo] ; i++ )
    for (int k=1; k <= myArray3ter.getNbGauss(i); k++)
      for (int j=1; j <= mdim; j++) {
        test3ok = test3ok
          && (  myArray3ter.getIJK(i,j,k)    ==  array3Ref[elemno] )
          && (  myArray3Ptr[elemno]          ==  array3Ref[elemno] );
        elemno++;
      }

  std::cout << "- Opérateur d'affectation en FullInterlace et Gauss : " << test3ok << std::endl;


  test3ok = true;
  MEDMEM_Array<double,FullInterlaceGaussPolicy> myArray3qua(mdim,nbelem2,
                                                            nbtypegeo,nbelgeoc,nbgaussgeo);
  myArray3Ptr = myArray3qua.getPtr();

  int cumul = 0;
  for (int i =1; i <= nbelem2; i++){
    myArray3qua.setRow(i,&array3Ref[cumul]);
    cumul+=myArray3qua.getNbGauss(i)*mdim;
  };

  for (int i =0; i < myArray3qua.getArraySize(); i++)
        test3ok = test3ok && ( myArray3Ptr[i] == array3Ref[i] );
  std::cout << "- Méthode setRow en FullInterlace et Gauss : " << test3ok << std::endl;

  std::cout << "- Test de levée d'exception pour getColumn : " ;
  try {
    myArray3qua.getColumn(1);
  }
  catch (MEDMEM::MEDEXCEPTION &m) {
    std::cout << m.what() << endl ;
  }

  test3ok = true;
  MEDMEM_Array<double,NoInterlaceGaussPolicy> * myArray3cin = ArrayConvert(myArray3);
  myArray3Ptr = myArray3cin->getPtr();
  elemno = 0;
  for (int j=1; j <= mdim; j++)
    for (int  i=1; i < nbelgeoc[nbtypegeo] ; i++ )
      for (int k=1; k <= myArray3cin->getNbGauss(i); k++) {
        test3ok = test3ok
          && (  myArray3cin->getIJK(i,j,k) ==  array4Ref[elemno] )
          && (  myArray3Ptr[elemno]        ==  array4Ref[elemno] );
        elemno++;
      }
  std::cout << "- Convertion du mode FullInterlaceGaussPolicy au mode NoInterlaceGaussPolicy : " 
            << test3ok << std::endl;


  // TEST n°4   NoInterlace et Gauss
  std::cout << std::endl;
  std::cout << "Test de MEDMEM_Array en NoInterlace et Gauss : " << std::endl;
  std::cout << std::endl;

  bool test4ok = true;
  const double * myArray4Ptr = 0;

  MEDMEM_Array<double,NoInterlaceGaussPolicy> myArray4(mdim,nbelem2,nbtypegeo,nbelgeoc,nbgaussgeo);

  elemno = 1;
   for (int ntyp=1; ntyp <= nbtypegeo; ntyp++ ) {
    for (int  i=0; i < (nbelgeoc[ntyp]-nbelgeoc[ntyp-1]) ; i++ ) {
      for (int k=1; k <= nbgaussgeo[ntyp]; k++)
        for (int j=1; j <= mdim; j++) {
          myArray4.setIJK(elemno,j,k,elemno+(ntyp-1)*10+0.1*k+0.01*j);
        }
      elemno++;
    }
  }
  myArray4Ptr = myArray4.getPtr();
  for (int i =0; i < myArray4.getArraySize(); i++) {
    test4ok = test4ok && ( myArray4Ptr[i] == array4Ref[i] );
  }
  std::cout << "- Egualité de myArray4 et de myArray4Ref : " << test4ok << std::endl ;

  test4ok = true;
  MEDMEM_Array<double,NoInterlaceGaussPolicy> myArray4bis(myArray4, false);
  myArray4Ptr = myArray4bis.getPtr();

  elemno = 0;
  for (int j=1; j <= mdim; j++)
    for (int  i=1; i < nbelgeoc[nbtypegeo] ; i++ )
      for (int k=1; k <= myArray4bis.getNbGauss(i); k++) {
        test4ok = test4ok
          && (  myArray4bis.getIJK(i,j,k)    ==  array4Ref[elemno] )
          && (  myArray4Ptr[elemno]          ==  array4Ref[elemno] );
        elemno++;
      }
  std::cout << "- Constructeur par recopie en NoInterlace et Gauss : " << test4ok << std::endl;

  test4ok = true;
  MEDMEM_Array<double,NoInterlaceGaussPolicy> myArray4ter;
  myArray4ter = myArray4; // Opérateur d'affectation
  myArray4Ptr = myArray4ter.getPtr();

  elemno = 0;
  for (int j=1; j <= mdim; j++)
    for (int  i=1; i < nbelgeoc[nbtypegeo] ; i++ )
      for (int k=1; k <= myArray4ter.getNbGauss(i); k++) {
        test4ok = test4ok
          && (  myArray4ter.getIJK(i,j,k)    ==  array4Ref[elemno] )
          && (  myArray4Ptr[elemno]          ==  array4Ref[elemno] );
        elemno++;
      }

  std::cout << "- Opérateur d'affectation en NoInterlace et Gauss : " << test4ok << std::endl;


  test4ok = true;
  MEDMEM_Array<double,NoInterlaceGaussPolicy> myArray4qua(mdim,nbelem2,nbtypegeo,nbelgeoc,nbgaussgeo);
  myArray4Ptr = myArray4qua.getPtr();

  for (int j =1; j <= mdim; j++)
    myArray4qua.setColumn(j,&array4Ref[(myArray4qua.getArraySize()/mdim)*(j-1)]);

  for (int i =0; i < myArray4qua.getArraySize(); i++)
        test4ok = test4ok && ( myArray4Ptr[i] == array4Ref[i] );
  std::cout << "- Méthode setColumn en NoInterlace et Gauss : " << test4ok << std::endl;

  std::cout << "- Test de levée d'exception pour getRow : " ;
  try {
    myArray4qua.getRow(1);
  }
  catch (MEDMEM::MEDEXCEPTION &m) {
    std::cout << m.what() << endl ;
  }


  test4ok = true;
  MEDMEM_Array<double,FullInterlaceGaussPolicy> * myArray4cin = ArrayConvert(myArray4);
  //// MEDMEM_Array<double,FullInterlaceGaussPolicy> * myArray4cin;
  ////ArrayConvert(myArray4,myArray4cin);
  // typedef MEDMEM_Array<double,FullInterlaceGaussPolicy> MyArray;
  //   MyArray myArray4cin MyArray(myArray4);
  //   myArray4cin.convert(myArray4);
  //  MEDMEM_Array<double,FullInterlaceGaussPolicy> myArray4cin(myArray4);
  myArray4Ptr = myArray4cin->getPtr();
  elemno = 0;
  for (int  i=1; i < nbelgeoc[nbtypegeo] ; i++ )
    for (int k=1; k <= myArray4cin->getNbGauss(i); k++)
      for (int j=1; j <= mdim; j++) {
        test4ok = test4ok
          && (  myArray4cin->getIJK(i,j,k)   ==  array3Ref[elemno] )
          && (  myArray4cin->getIJK(i,j,k)   ==  myArray3.getIJK(i,j,k) )
          && (  myArray4Ptr[elemno]          ==  array3Ref[elemno] );
        elemno++;
      }

  std::cout << "- Convertion du mode NoInterlaceGaussPolicy au mode FullInterlaceGaussPolicy : " 
            << test4ok << std::endl;


  //TEST DES METHODES D'INTERROGATION en NoInterlace et Gauss :
  //  MEDMEM_ArrayInterface<double,NoInterlace,Gauss>::Array & myArray4bis = myField4bis.getArray();
  
  std::cout << std::endl;
  std::cout << "Test de la méthode getDim : " << myArray4bis.getDim() << std::endl;
  std::cout << "Test de la méthode getNbelem : " << myArray4bis.getNbElem() << std::endl;
  std::cout << "Test de la méthode getArraySize : " << myArray4bis.getArraySize() << std::endl;
  std::cout << "Test de la méthode getInterlacingType : " << myArray4bis.getInterlacingType() << std::endl;


  // Test de levée d'exception si MED_ARRAY compilée avec une politique de checking
  std::cout << std::endl;
  std::cout << "Test de levée d'exception pour indice incohérent " << std::endl;
  try {
    myArray4.getIJ(0,2); //L'exception génère des blocks perdus (A voir)
  }
  catch (MEDMEM::MEDEXCEPTION &m) {
    std::cout << m.what() << endl ;
  }

 

  return 0;

}

