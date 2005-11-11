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

#ifndef MED_Vector_HeaderFile
#define MED_Vector_HeaderFile

#include <vector>

#if !defined(__GNUC_2__) && defined(__GNUC__) && (__GNUC__ == 2)
  #define __GNUC_2__
#endif

namespace MED
{

  //! Main purpose to introduce the class was to customize operator [] 
  template<typename _Tp, typename _Alloc = std::allocator<_Tp> >
  class TVector : public std::vector<_Tp, _Alloc>
  {
  public:
    typedef size_t size_type;

    typedef std::vector<_Tp, _Alloc> superclass;
    typedef typename superclass::allocator_type allocator_type;

    typedef _Tp	value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;

  public:
    explicit
    TVector(const allocator_type& __a = allocator_type()): 
      superclass(__a) 
    {}
    
    TVector(size_type __n, const value_type& __value,
	    const allocator_type& __a = allocator_type()):
      superclass(__n, __value, __a)
    {}
    
    explicit
    TVector(size_type __n):
      superclass(__n)
    {}

    TVector(const TVector& __x):
      superclass(__x)
    {}

    template<typename _InputIterator>
    TVector(_InputIterator __first, _InputIterator __last,
	    const allocator_type& __a = allocator_type()):
      superclass(__first, __last, __a)
    {}

    template<typename _Yp, typename _Al>
    TVector(TVector<_Yp, _Al> __y):
      superclass(__y.begin(), __y.end())
    {}

    TVector&
    operator=(const TVector& __x)
    {
      superclass::operator=(__x);
      return *this;
    }

    template<typename _Yp, typename _Al>
    TVector&
    operator=(TVector<_Yp, _Al> __y)
    {
      this->assign(__y.begin(), __y.end());
      return *this;
    }

    reference
    operator[](size_type __n)
    {
#if defined(__GNUC_2__) || !defined(_DEBUG_)
      return superclass::operator[](__n);
#else
      return this->at(__n);
#endif
    }

    const_reference
    operator[](size_type __n) const
    {
#if defined(__GNUC_2__) || !defined(_DEBUG_)
      return superclass::operator[](__n);
#else
      return this->at(__n);
#endif
    }
  };

}


#endif
