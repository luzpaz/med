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

#ifndef MED_SharedPtr_HeaderFile
#define MED_SharedPtr_HeaderFile

#include <boost/shared_ptr.hpp>

namespace MED
{

  template<class T> class SharedPtr: public boost::shared_ptr<T>
  {
  public:
    SharedPtr() {}

    template<class Y>
    explicit SharedPtr(Y * p): 
      boost::shared_ptr<T>(p) 
    {}

    template<class Y>
    SharedPtr(SharedPtr<Y> const & r):
      boost::shared_ptr<T>(r,boost::detail::polymorphic_cast_tag())
    {}

    template<class Y>
    SharedPtr& operator=(SharedPtr<Y> const & r)
    {
      boost::shared_ptr<T>(r,boost::detail::polymorphic_cast_tag()).swap(*this);
      return *this;
    }

    template<class Y> SharedPtr& operator()(Y * p) // Y must be complete
    {
      return operator=<Y>(SharedPtr<Y>(p));
    }

    operator const T& () const 
    { 
      return *(this->get());
    }

    operator T& () 
    { 
      return *(this->get());
    }
  };

}


#endif
