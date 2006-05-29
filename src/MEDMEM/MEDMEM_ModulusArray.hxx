// Copyright (C) 2005  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#ifndef __MEDMODULUSARRAY_H__
#define __MEDMODULUSARRAY_H__

#include "MEDMEM_Utilities.hxx"

/*
  This class is use to set cyclic (modulus length) array.
  (array[length+1]==array[1])

  We use it in particular to compare faces nodes lists to know if
  they are the same (positive or negative cycle) !
*/

namespace MEDMEM {
class MEDMODULUSARRAY {
private:
  int   _length ;
  const int * _array ;

public:
  MEDMODULUSARRAY(int length, const int * array) ;
  ~MEDMODULUSARRAY() ;
  
  const int & operator[](const int &i) const ;

  int compare(const MEDMODULUSARRAY &modulusArray) const;

};

MEDMODULUSARRAY::MEDMODULUSARRAY(int length, const int * array) : 
  _length(length), _array(array)
{
//    SCRUTE(_length);
//    for (int i=0;i<_length;i++){
//      MESSAGE("MEDMODULUSARRAY["<<i<<"]="<<_array[i]);
//    }
};

MEDMODULUSARRAY::~MEDMODULUSARRAY()
{
  // do nothing because nothing is allocated !
  //  MESSAGE("MEDMODULUSARRAY::~MEDMODULUSARRAY()") ;
};


const int & MEDMODULUSARRAY::operator[](const int &i) const
{
  int position = i%_length ;
  if (position<0)
    position+=_length ;
  return _array[position] ;
};

int MEDMODULUSARRAY::compare(const MEDMODULUSARRAY &modulusArray) const
{
  int ret = 0 ;

  if (modulusArray._length != _length)
    return ret ;

  if (_length==1)
    if (_array[0]==modulusArray[0])
      return 1;
    else 
      return 0;

  if (_length==2)
    if ((_array[0]==modulusArray[0])&(_array[1]==modulusArray[1]))
      return 1;
    else if ((_array[0]==modulusArray[1])&(_array[1]==modulusArray[0]))
      return -1;
    else
      return 0;

  //search if there is one start point in common in two array
  for(int i=0;i<_length;i++)
    if ( _array[0] == modulusArray[i] ) {
      // we search if cycle is the same
      if (_array[1]==modulusArray[i+1]){ // positive order
	ret=1;
	for(int j=2;j<_length;j++)
	  if (_array[j]!=modulusArray[i+j]) {
	    ret = 0 ;
	    break ;
	  }
      } else if(_array[1]==modulusArray[i-1]) { //negative order
	ret=-1;
	for(int j=2;j<_length;j++)
	  if (_array[j]!=modulusArray[i-j]) {
	    ret = 0 ;
	    break ;
	  }
      }
      if (ret!=0) {// we have found it !
	break ;
      }
      // else we continue if there is another start point i
    }
    return ret ;
}
}

# endif 	/* # ifndef __MEDMODULUSARRAY_H__ */

