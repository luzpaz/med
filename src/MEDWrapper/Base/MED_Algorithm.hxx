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

#ifndef MED_Algorithm_HeaderFile
#define MED_Algorithm_HeaderFile

#include "MED_Structures.hxx"

#include <set>

namespace MED{

  struct TWrapper;

  //---------------------------------------------------------------
  typedef std::map<EGeometrieElement,PElemInfo> TElemMap;
  typedef std::map<EEntiteMaillage,TElemMap> TElemGroup;

  TElemGroup
  GetElemsByEntity(TWrapper& theWrapper, 
		   const PMeshInfo& theMeshInfo,
		   const MED::TEntityInfo& theEntityInfo);


  //---------------------------------------------------------------
  typedef std::set<PFamilyInfo> TFamilyGroup;

  TFamilyGroup
  GetFamilies(TWrapper& theWrapper, 
	      const PMeshInfo& theMeshInfo);
  

  //---------------------------------------------------------------
  typedef std::map<EEntiteMaillage,TFamilyGroup> TFamilyByEntity;
  
  TFamilyByEntity 
  GetFamiliesByEntity(TWrapper& theWrapper, 
		      const TElemGroup& theElemGroup,
		      const TFamilyGroup& theFamilyGroup);
  

  //---------------------------------------------------------------
  typedef std::map<std::string,TFamilyGroup> TGroupInfo;
  
  TGroupInfo
  GetFamiliesByGroup(const TFamilyGroup& theGroupInfo);
  
  
  //---------------------------------------------------------------
  typedef std::set<PTimeStampInfo> TTimeStampSet;
  typedef std::map<PFieldInfo,TTimeStampSet> TTimeStampGroup;

  TTimeStampGroup
  GetFieldsByEntity(TWrapper& theWrapper, 
		    const PMeshInfo& theMeshInfo,
		    const MED::TEntityInfo& theEntityInfo);
  

  //---------------------------------------------------------------
  typedef std::map<EEntiteMaillage,TTimeStampGroup> TFieldGroup;

  TFieldGroup
  GetFieldsByEntity(const TTimeStampGroup& theTimeStampGroup);


  //---------------------------------------------------------------
  typedef std::map<TGaussInfo::TKey,PGaussInfo,TGaussInfo::TLess> TKey2Gauss;

  TKey2Gauss
  GetKey2Gauss(TWrapper& theWrapper, 
	       TErr* theErr = NULL,
	       EModeSwitch theMode = eFULL_INTERLACE);


  //---------------------------------------------------------------
  PProfileInfo
  GetProfileInfo(TWrapper& theWrapper, 
		 const std::string& theProfileName,
		 TErr* theErr = NULL,
		 EModeProfil theMode = eCOMPACT);


  //---------------------------------------------------------------
  typedef std::map<TProfileInfo::TKey,PProfileInfo> TKey2Profile;
  typedef boost::tuple<EModeProfil,TKey2Profile> TMKey2Profile;
  TMKey2Profile
  GetMKey2Profile(TWrapper& theWrapper, 
		  TErr* theErr = NULL,
		  EModeProfil theMode = eCOMPACT);


  //---------------------------------------------------------------
  typedef std::vector<TCCoordSlice> TCCoordSliceArr;
  typedef std::vector<TCoordSlice> TCoordSliceArr;

  class TGaussCoord:
    virtual TModeSwitchInfo 
  {
    TInt myNbElem;
    TInt myNbGauss;
    TInt myDim;

    TInt myGaussStep;

    TNodeCoord myGaussCoord;

  public:

    TCCoordSliceArr
    GetCoordSliceArr(TInt theElemId) const;

    TCoordSliceArr 
    GetCoordSliceArr(TInt theElemId);

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

    TInt
    size() const { return GetNbElem(); }
  };


  //---------------------------------------------------------------
  void
  GetGaussCoord3D(const TGaussInfo& theGaussInfo, 
		  const TCellInfo& theCellInfo,
		  const TNodeInfo& theNodeInfo,
		  TGaussCoord& theGaussCoord,
		  const TElemNum& theElemNum = TElemNum(),
		  EModeSwitch theMode = eFULL_INTERLACE);
}

#endif
