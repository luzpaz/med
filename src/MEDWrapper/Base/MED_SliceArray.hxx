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
  //! This class intends to provide an uniform way to handle multy-dimention data (const version)
  /*! 
    It just contains pointer to real sequence and implement proper calcultion of its indexes.
    This class deal with constant pointer to the sources data and provide const method to
    read the them (data).
   */
  template<class TContainer> 
  class TCSlice
  {
    typedef const TContainer* PCContainer;
    PCContainer myCContainer; //!< Reference to source multy-dimension data
    std::slice mySlice; //!< Defines algorithm of index calculation
  protected:
    //! Calculate internal index to get proper element from the source multy-dimension data
    size_t
    GetID(size_t theId) const
    {
#ifdef _DEBUG_
      size_t anId = -1;
      if(theId < mySlice.size()){
	anId = mySlice.start() + theId*mySlice.stride();
	if(anId < myCContainer->size())
	  return anId;
      }
      throw std::out_of_range(std::string("TCSlice::GetID"));
      return anId;
#else
      return mySlice.start() + theId*mySlice.stride();
#endif
    }
    
  public:
    typedef typename TContainer::value_type value_type;

    //! Construct the class
    TCSlice(const TContainer& theContainer,
	    const std::slice& theSlice): 
      myCContainer(&theContainer),
      mySlice(theSlice)
    {}
    
    //! Default constructor (dangerous)
    TCSlice():
      myCContainer(NULL)
    {}

    //! Get element by its number (const version)
    const value_type& 
    operator[](size_t theId) const
    {
      return (*myCContainer)[GetID(theId)];
    }
    
    //! Get range of the order numbers
    size_t
    size() const
    {
      return mySlice.size();
    }
  };
  

  //---------------------------------------------------------------
  //! This class extend TCSlice functionality for non-constant case
  template<class TContainer> 
  class TSlice: public TCSlice<TContainer>
  {
    typedef TContainer* PContainer;
    PContainer myContainer;
    
  public:
    typedef typename TContainer::value_type value_type;
    typedef TCSlice<TContainer> TSupperClass;

    //! Construct the class
    TSlice(TContainer& theContainer,
	   const std::slice& theSlice): 
      TSupperClass(theContainer,theSlice),
      myContainer(&theContainer)
    {
    }
    
    //! Default constructor (dangerous)
    TSlice():
      myContainer(NULL)
    {}

    //! Get element by its number
    value_type& 
    operator[](size_t theId)
    {
      return (*myContainer)[this->GetID(theId)];
    }
  };

}


#endif
