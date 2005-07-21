//  
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : 
//  Author : 
//  Module : 
//  $Header$

#ifndef MED_SliceArray_HeaderFile
#define MED_SliceArray_HeaderFile

#include <valarray>
#include <stdexcept>

namespace MED
{
  //---------------------------------------------------------------
  template<class TContainer> 
  class TCSlice
  {
    typedef const TContainer* PCContainer;
    PCContainer myCContainer;
    std::slice mySlice;
  protected:
    size_t
    GetID(size_t theId) const
    {
#ifdef _DEBUG_
      if(theId < mySlice.size()){
	size_t anId = mySlice.start() + theId*mySlice.stride();
	if(anId < myCContainer->size())
	  return anId;
      }
      throw std::out_of_range(std::string("TCSlice::GetID"));
      return -1;
#else
      return mySlice.start() + theId*mySlice.stride();
#endif
    }
    
  public:
    typedef typename TContainer::value_type value_type;

    TCSlice(const TContainer& theContainer,
	    const std::slice& theSlice): 
      myCContainer(&theContainer),
      mySlice(theSlice)
    {}
    
    TCSlice():
      myCContainer(NULL)
    {}

    const value_type& 
    operator[](size_t theId) const
    {
      return (*myCContainer)[GetID(theId)];
    }
    
    size_t
    size() const
    {
      return mySlice.size();
    }
  };
  

  //---------------------------------------------------------------
  template<class TContainer> 
  class TSlice: public TCSlice<TContainer>
  {
    typedef TContainer* PContainer;
    PContainer myContainer;
    
  public:
    typedef typename TContainer::value_type value_type;
    typedef TCSlice<TContainer> TSupperClass;

    TSlice(TContainer& theContainer,
	   const std::slice& theSlice): 
      TSupperClass(theContainer,theSlice),
      myContainer(&theContainer)
    {
    }
    
    TSlice():
      myContainer(NULL)
    {}

    value_type& 
    operator[](size_t theId)
    {
      return (*myContainer)[this->GetID(theId)];
    }
  };

}


#endif
