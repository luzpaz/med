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

#ifndef MED_GaussUtils_HeaderFile
#define MED_GaussUtils_HeaderFile

#include "MED_WrapperBase.hxx"

#include "MED_Structures.hxx"

namespace MED
{
  //---------------------------------------------------------------
  typedef TVector<TCCoordSlice> TCCoordSliceArr;
  typedef TVector<TCoordSlice> TCoordSliceArr;

  //! Define a helper class to handle Gauss Points coordinates
  class MEDWRAPPER_EXPORT TGaussCoord:
    virtual TModeSwitchInfo 
  {
    TInt myNbElem;
    TInt myNbGauss;
    TInt myDim;

    TInt myGaussStep;

    TNodeCoord myGaussCoord;

  public:
    
    TGaussCoord();

    //! Get slice of the coordinate that corresponds to defined cell (const version)
    TCCoordSliceArr
    GetCoordSliceArr(TInt theElemId) const;

    //! Get slice of the coordinate that corresponds to defined cell
    TCoordSliceArr 
    GetCoordSliceArr(TInt theElemId);

    //! To init the class
    void
    Init(TInt theNbElem,
	 TInt theNbGauss,
	 TInt theDim,
	 EModeSwitch theMode = eFULL_INTERLACE);

    TInt
    GetNbElem() const { return myNbElem; }

    TInt
    GetNbGauss() const { return myNbGauss; }

    TInt
    GetDim() const { return myDim; }
  };


  //---------------------------------------------------------------
  //! To calculate Gauss Points coordinates
  MEDWRAPPER_EXPORT bool
  GetGaussCoord3D(const TGaussInfo& theGaussInfo, 
		  const TCellInfo& theCellInfo,
		  const TNodeInfo& theNodeInfo,
		  TGaussCoord& theGaussCoord,
		  const TElemNum& theElemNum = TElemNum(),
		  EModeSwitch theMode = eFULL_INTERLACE);


  //---------------------------------------------------------------
  //! To calculate Gauss Points coordinates for defined TCellInfo as its bary center
  MEDWRAPPER_EXPORT bool
  GetBaryCenter(const TCellInfo& theCellInfo,
		const TNodeInfo& theNodeInfo,
		TGaussCoord& theGaussCoord,
		const TElemNum& theElemNum = TElemNum(),
		EModeSwitch theMode = eFULL_INTERLACE);

  //! To calculate Gauss Points coordinates for defined TPolygoneInfo as its bary center
  MEDWRAPPER_EXPORT bool
  GetBaryCenter(const TPolygoneInfo& thePolygoneInfo,
		const TNodeInfo& theNodeInfo,
		TGaussCoord& theGaussCoord,
		const TElemNum& theElemNum = TElemNum(),
		EModeSwitch theMode = eFULL_INTERLACE);

  //! To calculate Gauss Points coordinates for defined TPolyedreInfo as its bary center
  MEDWRAPPER_EXPORT bool
  GetBaryCenter(const TPolyedreInfo& thePolyedreInfo,
		const TNodeInfo& theNodeInfo,
		TGaussCoord& theGaussCoord,
		const TElemNum& theElemNum = TElemNum(),
		EModeSwitch theMode = eFULL_INTERLACE);
}

#endif
