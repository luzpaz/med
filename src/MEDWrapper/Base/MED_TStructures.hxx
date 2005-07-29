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

#ifndef MED_TStructures_HeaderFile
#define MED_TStructures_HeaderFile

#include "MED_Structures.hxx"

namespace MED{

  const TInt DESC = 200;
  const TInt IDENT = 8;
  const TInt NOM = 32;
  const TInt LNOM = 80;

  //---------------------------------------------------------------
  template<TInt nPNOM, TInt nDESC, TInt nIDENT, TInt nNOM, TInt nLNOM, EVersion nV>
  struct TTNameInfo: virtual TNameInfo
  {
    TTNameInfo(const std::string& theValue)
    {
      myName.resize(nNOM+1);
      SetName(theValue);
    }

    virtual
    std::string
    GetName() const 
    { 
      return GetString(0,nNOM,myName);
    }

    virtual
    void
    SetName(const std::string& theValue)
    {
      SetString(0,nNOM,myName,theValue);
    }
  };


  //---------------------------------------------------------------
  template<TInt nPNOM, TInt nDESC, TInt nIDENT, TInt nNOM, TInt nLNOM, EVersion nV>
  struct TTMeshInfo: 
    virtual TMeshInfo, 
    virtual TTNameInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV>
  {
    typedef TTNameInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV> TNameInfoBase;

    TTMeshInfo(const PMeshInfo& theInfo):
      TNameInfoBase(theInfo->GetName())
    {
      myDim = theInfo->GetDim();
      myType = theInfo->GetType();
      
      myDesc.resize(nDESC+1);
      SetDesc(theInfo->GetDesc());
    }

    TTMeshInfo(TInt theDim,
	       const std::string& theValue,
	       EMaillage theType,
	       const std::string& theDesc):
      TNameInfoBase(theValue)
    {
      myDim = theDim;
      myType = theType;
      
      myDesc.resize(nDESC+1);
      SetDesc(theDesc);
    }

    virtual 
    std::string
    GetDesc() const 
    { 
      return GetString(0,nDESC,myDesc);
    }

    virtual
    void
    SetDesc(const std::string& theValue)
    {
      SetString(0,nDESC,myDesc,theValue);
    }
  };


  //---------------------------------------------------------------
  template<TInt nPNOM, TInt nDESC, TInt nIDENT, TInt nNOM, TInt nLNOM, EVersion nV>
  struct TTFamilyInfo: 
    virtual TFamilyInfo, 
    virtual TTNameInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV>
  {
    typedef TTNameInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV> TNameInfoBase;

    TTFamilyInfo(const PMeshInfo& theMeshInfo, const PFamilyInfo& theInfo):
      TNameInfoBase(theInfo->GetName())
    {
      myMeshInfo = theMeshInfo;

      myId = theInfo->GetId();

      myNbGroup = theInfo->GetNbGroup();
      myGroupNames.resize(myNbGroup*nLNOM+1);
      if(myNbGroup){
	for(TInt anId = 0; anId < myNbGroup; anId++){
	  SetGroupName(anId,theInfo->GetGroupName(anId));
	}
      }

      myNbAttr = theInfo->GetNbAttr();
      myAttrId.resize(myNbAttr);
      myAttrVal.resize(myNbAttr);
      myAttrDesc.resize(myNbAttr*nDESC+1);
      if(myNbAttr){
	for(TInt anId = 0; anId < myNbAttr; anId++){
	  SetAttrDesc(anId,theInfo->GetAttrDesc(anId));
	  myAttrVal[anId] = theInfo->GetAttrVal(anId);
	  myAttrId[anId] = theInfo->GetAttrId(anId);
	}
      }
    }

    TTFamilyInfo(const PMeshInfo& theMeshInfo,
		 TInt theNbGroup, 
		 TInt theNbAttr,
		 TInt theId,
		 const std::string& theValue):
      TNameInfoBase(theValue)
    {
      myMeshInfo = theMeshInfo;

      myId = theId;

      myNbGroup = theNbGroup;
      if(!theNbGroup)
	theNbGroup = 1;
      myGroupNames.resize(theNbGroup*nLNOM+1);

      myNbAttr = theNbAttr;

      if(!theNbAttr)
	theNbAttr = 1;
      myAttrId.resize(theNbAttr);
      myAttrVal.resize(theNbAttr);
      myAttrDesc.resize(theNbAttr*nDESC+1);
    }

    TTFamilyInfo(const PMeshInfo& theMeshInfo,
		 const std::string& theValue,
		 TInt theId,
		 const TStringSet& theGroupNames, 
		 const TStringVector& theAttrDescs, 
		 const TIntVector& theAttrIds, 
		 const TIntVector& theAttrVals):
      TNameInfoBase(theValue)
    {
      myMeshInfo = theMeshInfo;

      myId = theId;

      myNbGroup = theGroupNames.size();
      myGroupNames.resize(myNbGroup*nLNOM+1);
      if(myNbGroup){
	TStringSet::const_iterator anIter = theGroupNames.begin();
	for(TInt anId = 0; anIter != theGroupNames.end(); anIter++, anId++){
	  const std::string& aVal = *anIter;
	  SetGroupName(anId,aVal);
	}
      }

      myNbAttr = theAttrDescs.size();
      myAttrId.resize(myNbAttr);
      myAttrVal.resize(myNbAttr);
      myAttrDesc.resize(myNbAttr*nDESC+1);
      if(myNbAttr){
	for(TInt anId = 0, anEnd = theAttrDescs.size(); anId < anEnd; anId++){
	  SetAttrDesc(anId,theAttrDescs[anId]);
	  myAttrVal[anId] = theAttrVals[anId];
	  myAttrId[anId] = theAttrIds[anId];
	}
      }
    }

    virtual
    std::string
    GetGroupName(TInt theId) const 
    { 
      return GetString(theId,nLNOM,myGroupNames);
    }

    virtual
    void
    SetGroupName(TInt theId, const std::string& theValue)
    {
      SetString(theId,nLNOM,myGroupNames,theValue);
    }

    virtual
    std::string
    GetAttrDesc(TInt theId) const 
    { 
      return GetString(theId,nDESC,myAttrDesc);
    }

    virtual
    void
    SetAttrDesc(TInt theId, const std::string& theValue)
    {
      SetString(theId,nDESC,myAttrDesc,theValue);
    }
  };


  //---------------------------------------------------------------
  template<TInt nPNOM, TInt nDESC, TInt nIDENT, TInt nNOM, TInt nLNOM, EVersion nV>
  struct TTElemInfo: virtual TElemInfo
  {
    TTElemInfo(const PMeshInfo& theMeshInfo, const PElemInfo& theInfo)
    {
      myMeshInfo = theMeshInfo;
      
      myNbElem = theInfo->GetNbElem();
      myFamNum.resize(myNbElem);

      myIsElemNum = theInfo->IsElemNum();
      myElemNum.resize(myIsElemNum == eFAUX? 0: myNbElem);

      myIsElemNames = theInfo->IsElemNames();
      myElemNames.resize(myNbElem*nPNOM+1);

      if(myNbElem){
	for(TInt anId = 0; anId < myNbElem; anId++){
	  myFamNum[anId] = theInfo->GetFamNum(anId);
	}
	if(myIsElemNum == eVRAI){
	  for(TInt anId = 0; anId < myNbElem; anId++){
	    myElemNum[anId] = theInfo->GetElemNum(anId);
	  }
	}
	if(myIsElemNames == eVRAI){
	  for(TInt anId = 0; anId < myNbElem; anId++){
	    SetElemName(anId,theInfo->GetElemName(anId));
	  }
	}
      }
    }

    TTElemInfo(const PMeshInfo& theMeshInfo, 
	       TInt theNbElem,
	       EBooleen theIsElemNum,
	       EBooleen theIsElemNames)
    {
      myMeshInfo = theMeshInfo;

      myNbElem = theNbElem;
      myFamNum.resize(theNbElem);

      myIsElemNum = theIsElemNum;
      if(theIsElemNum)
	myElemNum.resize(theNbElem);

      myIsElemNames = theIsElemNames;
      if(theIsElemNames)
	myElemNames.resize(theNbElem*nPNOM+1);
    }
    
    TTElemInfo(const PMeshInfo& theMeshInfo, 
	       TInt theNbElem,
	       const TIntVector& theFamilyNums,
	       const TIntVector& theElemNums,
	       const TStringVector& theElemNames)
    {
      myMeshInfo = theMeshInfo;
      
      myNbElem = theNbElem;
      
      myIsElemNum = theElemNums.size()? eVRAI: eFAUX;
      if(myIsElemNum)
	myElemNum.resize(theNbElem);
      
      myIsElemNames = theElemNames.size()? eVRAI: eFAUX;
      if(myIsElemNames)
	myElemNames.resize(theNbElem*nPNOM+1);
      
      if(theNbElem){

	myFamNum.resize(theNbElem);
	if(theFamilyNums.size())
	  myFamNum = theFamilyNums;

	if(myIsElemNum)
	  myElemNum = theElemNums;

	if(myIsElemNames){
	  for(TInt anId = 0; anId < theNbElem; anId++){
	    const std::string& aVal = theElemNames[anId];
	    SetElemName(anId,aVal);
	  }
	}
      }
    }

    virtual
    std::string
    GetElemName(TInt theId) const 
    { 
      return GetString(theId,nPNOM,myElemNames);
    }

    virtual
    void
    SetElemName(TInt theId, const std::string& theValue)
    {
      SetString(theId,nPNOM,myElemNames,theValue);
    }
  };


  //---------------------------------------------------------------
  template<TInt nPNOM, TInt nDESC, TInt nIDENT, TInt nNOM, TInt nLNOM, EVersion nV>
  struct TTNodeInfo: 
    virtual TNodeInfo, 
    virtual TTElemInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV>
  {
    typedef TTElemInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV> TElemInfoBase;

    TTNodeInfo(const PMeshInfo& theMeshInfo, const PNodeInfo& theInfo):
      TElemInfoBase(theMeshInfo,theInfo),
      TNodeInfo(theInfo)
    {
      myModeSwitch = theInfo->GetModeSwitch();
      
      mySystem = theInfo->GetSystem();
      
      myCoord = theInfo->myCoord;
      
      TInt aDim = theMeshInfo->GetDim();

      myCoordNames.resize(aDim*nPNOM+1);
      for(TInt anId = 0; anId < aDim; anId++)
	SetCoordName(anId,theInfo->GetCoordName(anId));
      
      myCoordUnits.resize(aDim*nPNOM+1);
      for(TInt anId = 0; anId < aDim; anId++)
	SetCoordUnit(anId,theInfo->GetCoordUnit(anId));
    }

    TTNodeInfo(const PMeshInfo& theMeshInfo, 
	       TInt theNbElem,
	       EModeSwitch theMode,
	       ERepere theSystem, 
	       EBooleen theIsElemNum,
	       EBooleen theIsElemNames):
      TElemInfoBase(theMeshInfo,
		    theNbElem,
		    theIsElemNum,
		    theIsElemNames),
      TModeSwitchInfo(theMode)
    {
      mySystem = theSystem;

      myCoord.resize(theNbElem*theMeshInfo->myDim);

      if(theIsElemNum)
	myCoordUnits.resize(theMeshInfo->myDim*nPNOM+1);

      if(theIsElemNames)
	myCoordNames.resize(theMeshInfo->myDim*nPNOM+1);
    }

    
    TTNodeInfo(const PMeshInfo& theMeshInfo, 
	       const TFloatVector& theNodeCoords,
	       EModeSwitch theMode,
	       ERepere theSystem, 
	       const TStringVector& theCoordNames,
	       const TStringVector& theCoordUnits,
	       const TIntVector& theFamilyNums,
	       const TIntVector& theElemNums,
	       const TStringVector& theElemNames):
      TElemInfoBase(theMeshInfo,
		    theNodeCoords.size(),
		    theFamilyNums,
		    theElemNums,
		    theElemNames),
      TModeSwitchInfo(theMode)
    {
      mySystem = theSystem;

      myCoord = theNodeCoords;
      
      TInt aDim = theMeshInfo->GetDim();

      myCoordNames.resize(aDim*nPNOM+1);
      if(!theCoordNames.empty())
	for(TInt anId = 0; anId < aDim; anId++)
	  SetCoordName(anId,theCoordNames[anId]);
      
      myCoordUnits.resize(aDim*nPNOM+1);
      if(!theCoordUnits.empty())
	for(TInt anId = 0; anId < aDim; anId++)
	  SetCoordUnit(anId,theCoordUnits[anId]);
    }

    virtual
    std::string
    GetCoordName(TInt theId) const 
    { 
      return GetString(theId,nPNOM,myCoordNames);
    }

    virtual
    void
    SetCoordName(TInt theId, const std::string& theValue)
    {
      SetString(theId,nPNOM,myCoordNames,theValue);
    }

    virtual
    std::string 
    GetCoordUnit(TInt theId) const 
    { 
      return GetString(theId,nPNOM,myCoordUnits);
    }

    virtual
    void
    SetCoordUnit(TInt theId, const std::string& theValue)
    {
      SetString(theId,nPNOM,myCoordUnits,theValue);
    }
  };

  //---------------------------------------------------------------
  template<TInt nPNOM, TInt nDESC, TInt nIDENT, TInt nNOM, TInt nLNOM, EVersion nV>
  struct TTPolygoneInfo: 
    virtual TPolygoneInfo, 
    virtual TTElemInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV>
  {
    typedef TTElemInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV> TElemInfoBase;

    TTPolygoneInfo(const PMeshInfo& theMeshInfo, const PPolygoneInfo& theInfo):
      TElemInfoBase(theMeshInfo,theInfo)
    {
      myEntity = theInfo->GetEntity();
      myGeom = theInfo->GetGeom();

      myIndex = theInfo->myIndex;
      myConn = theInfo->myConn;

      myConnSize = theInfo->GetConnSize();

      myConnMode = theInfo->GetConnMode();
    }

    TTPolygoneInfo(const PMeshInfo& theMeshInfo, 
		   EEntiteMaillage theEntity, 
		   EGeometrieElement theGeom,
		   TInt theNbElem,
		   TInt theConnSize,
		   EConnectivite theConnMode,
		   EBooleen theIsElemNum,
		   EBooleen theIsElemNames):
      TElemInfoBase(theMeshInfo,
		    theNbElem,
		    theIsElemNum,
		    theIsElemNames)
    {
      myEntity = theEntity;
      myGeom = theGeom;

      myIndex.resize(theNbElem+1);
      myConn.resize(theConnSize);

      myConnSize = theConnSize;

      myConnMode = theConnMode;
    }
    
    TTPolygoneInfo(const PMeshInfo& theMeshInfo, 
		   EEntiteMaillage theEntity, 
		   EGeometrieElement theGeom,
		   const TIntVector& theIndexes,
		   const TIntVector& theConnectivities,
		   EConnectivite theConnMode,
		   const TIntVector& theFamilyNums,
		   const TIntVector& theElemNums,
		   const TStringVector& theElemNames):
      TElemInfoBase(theMeshInfo,
		    theIndexes.size()-1,
		    theFamilyNums,
		    theElemNums,
		    theElemNames)
    {
      myEntity = theEntity;
      myGeom = theGeom;

      myIndex = theIndexes;
      myConn = theConnectivities;

      myConnSize = theConnectivities.size();

      myConnMode = theConnMode;
    }
  };
  
  //---------------------------------------------------------------
  template<TInt nPNOM, TInt nDESC, TInt nIDENT, TInt nNOM, TInt nLNOM, EVersion nV>
  struct TTPolyedreInfo: 
    virtual TPolyedreInfo, 
    virtual TTElemInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV>
  {
    typedef TTElemInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV> TElemInfoBase;

    TTPolyedreInfo(const PMeshInfo& theMeshInfo, const PPolyedreInfo& theInfo):
      TElemInfoBase(theMeshInfo,theInfo)
    {
      myEntity = theInfo->GetEntity();
      myGeom = theInfo->GetGeom();

      myIndex = theInfo->GetIndex();
      myFaces = theInfo->GetFaces();
      myConn = theInfo->GetConnectivite();

      myConnSize = theInfo->GetConnSize();
      myNbFaces = theInfo->GetNbFaces();

      myConnMode = theInfo->GetConnMode();
    }

    TTPolyedreInfo(const PMeshInfo& theMeshInfo, 
		   EEntiteMaillage theEntity, 
		   EGeometrieElement theGeom,
		   TInt theNbElem,
		   TInt theNbFaces,
		   TInt theConnSize,
		   EConnectivite theConnMode,
		   EBooleen theIsElemNum,
		   EBooleen theIsElemNames):
      TElemInfoBase(theMeshInfo,
		    theNbElem,
		    theIsElemNum,
		    theIsElemNames)
    {
      myEntity = theEntity;
      myGeom = theGeom;

      myIndex.resize(theNbElem+1);
      myFaces.resize(theNbFaces);
      myConn.resize(theConnSize);

      myConnSize = theConnSize;
      myNbFaces = theNbFaces;

      myConnMode = theConnMode;
    }
    
    TTPolyedreInfo(const PMeshInfo& theMeshInfo, 
		   EEntiteMaillage theEntity, 
		   EGeometrieElement theGeom,
		   const TIntVector& theIndexes,
		   const TIntVector& theFaces,
		   const TIntVector& theConnectivities,
		   EConnectivite theConnMode,
		   const TIntVector& theFamilyNums,
		   const TIntVector& theElemNums,
		   const TStringVector& theElemNames):
      TElemInfoBase(theMeshInfo,
		    theIndexes.size()-1,
		    theFamilyNums,
		    theElemNums,
		    theElemNames)
    {
      myEntity = theEntity;
      myGeom = theGeom;

      myIndex = theIndexes;
      myFaces = theFaces;
      myConn = theConnectivities;

      myConnSize = theConnectivities.size();
      myNbFaces = theFaces.size();

      myConnMode = theConnMode;
    }
  };

  //---------------------------------------------------------------
  template<TInt nPNOM, TInt nDESC, TInt nIDENT, TInt nNOM, TInt nLNOM, EVersion nV>
  struct TTCellInfo: 
    virtual TCellInfo, 
    virtual TTElemInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV>
  {
    typedef TTElemInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV> TElemInfoBase;

    TTCellInfo(const PMeshInfo& theMeshInfo, const PCellInfo& theInfo):
      TElemInfoBase(theMeshInfo,theInfo)
    {
      myEntity = theInfo->GetEntity();
      myGeom = theInfo->GetGeom();
      myConnMode  = theInfo->GetConnMode();
      
      TInt aConnDim = GetNbNodes(myGeom);
      myConn.resize(myNbElem*GetNbConn<nV>(myGeom,myEntity,myMeshInfo->myDim));
      for(TInt anElemId = 0; anElemId < myNbElem; anElemId++){
	TConnSlice aConnSlice = GetConnSlice(anElemId);
	TCConnSlice aConnSlice2 = theInfo->GetConnSlice(anElemId);
	for(TInt anConnId = 0; anConnId < aConnDim; anConnId++){
	  aConnSlice[anConnId] = aConnSlice2[anConnId];
	}
      }
    }

    TTCellInfo(const PMeshInfo& theMeshInfo, 
	       EEntiteMaillage theEntity, 
	       EGeometrieElement theGeom,
	       TInt theNbElem,
	       EConnectivite theConnMode,
	       EBooleen theIsElemNum,
	       EBooleen theIsElemNames,
	       EModeSwitch theMode):
      TElemInfoBase(theMeshInfo,
		    theNbElem,
		    theIsElemNum,
		    theIsElemNames),
      TModeSwitchInfo(theMode)
    {
      myEntity = theEntity;
      myGeom = theGeom;

      myConnMode = theConnMode;
      myConn.resize(theNbElem*GetNbConn<nV>(theGeom,myEntity,theMeshInfo->myDim));
    }
    
    TTCellInfo(const PMeshInfo& theMeshInfo, 
	       EEntiteMaillage theEntity, 
	       EGeometrieElement theGeom,
	       const TIntVector& theConnectivities,
	       EConnectivite theConnMode,
	       const TIntVector& theFamilyNums,
	       const TIntVector& theElemNums,
	       const TStringVector& theElemNames,
	       EModeSwitch theMode):
      TElemInfoBase(theMeshInfo,
		    theConnectivities.size()/GetNbNodes(theGeom),
		    theFamilyNums,
		    theElemNums,
		    theElemNames),
      TModeSwitchInfo(theMode)
    {
      myEntity = theEntity;
      myGeom = theGeom;

      myConnMode = theConnMode;
      TInt aConnDim = GetNbNodes(myGeom);
      myConn.resize(myNbElem*GetNbConn<nV>(myGeom,myEntity,myMeshInfo->myDim));
      for(TInt anElemId = 0; anElemId < myNbElem; anElemId++){
	TConnSlice aConnSlice = GetConnSlice(anElemId);
	for(TInt anConnId = 0; anConnId < aConnDim; anConnId++){
	  aConnSlice[anConnId] = theConnectivities[anElemId*aConnDim+anConnId];
	}
      }

    }

    virtual 
    TInt
    GetConnDim() const 
    { 
      return GetNbConn<nV>(myGeom,myEntity,myMeshInfo->myDim);
    }

  };


  //---------------------------------------------------------------
  template<TInt nPNOM, TInt nDESC, TInt nIDENT, TInt nNOM, TInt nLNOM, EVersion nV>
  struct TTFieldInfo: 
    virtual TFieldInfo, 
    virtual TTNameInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV>
  {
    typedef TTNameInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV> TNameInfoBase;

    TTFieldInfo(const PMeshInfo& theMeshInfo, const PFieldInfo& theInfo):
      TNameInfoBase(theInfo->GetName())
    {
      myMeshInfo = theMeshInfo;

      myNbComp = theInfo->GetNbComp();
      myCompNames.resize(myNbComp*nPNOM+1);
      for(TInt anId = 0; anId < myNbComp; anId++){
	SetCompName(anId,theInfo->GetCompName(anId));
      }

      myUnitNames.resize(myNbComp*nPNOM+1);
      for(TInt anId = 0; anId < myNbComp; anId++){
	SetUnitName(anId,theInfo->GetUnitName(anId));
      }

      myType = theInfo->GetType();

      myIsLocal = theInfo->GetIsLocal();
      myNbRef = theInfo->GetNbRef();
    }

    TTFieldInfo(const PMeshInfo& theMeshInfo, 
		TInt theNbComp,
		ETypeChamp theType,
		const std::string& theValue,
		EBooleen theIsLocal,
		TInt theNbRef):
      TNameInfoBase(theValue)
    {
      myMeshInfo = theMeshInfo;

      myNbComp = theNbComp;
      myCompNames.resize(theNbComp*nPNOM+1);
      myUnitNames.resize(theNbComp*nPNOM+1);

      myType = theType;

      myIsLocal = theIsLocal;
      myNbRef = theNbRef;
    }
    
    virtual 
    std::string
    GetCompName(TInt theId) const 
    { 
      return GetString(theId,nPNOM,myCompNames);
    }

    virtual
    void
    SetCompName(TInt theId, const std::string& theValue)
    {
      SetString(theId,nPNOM,myCompNames,theValue);
    }

    virtual
    std::string 
    GetUnitName(TInt theId) const 
    { 
      return GetString(theId,nPNOM,myUnitNames);
    }

    virtual
    void
    SetUnitName(TInt theId, const std::string& theValue)
    {
      SetString(theId,nPNOM,myUnitNames,theValue);
    }
  };


  //---------------------------------------------------------------
  template<TInt nPNOM, TInt nDESC, TInt nIDENT, TInt nNOM, TInt nLNOM, EVersion nV>
  struct TTGaussInfo: 
    virtual TGaussInfo,
    virtual TTNameInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV>
  {
    typedef TTNameInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV> TNameInfoBase;

    TTGaussInfo(const TGaussInfo::TInfo& theInfo,
		EModeSwitch theMode):
      TNameInfoBase(boost::get<1>(boost::get<0>(theInfo))),
      TModeSwitchInfo(theMode)
    {
      const TGaussInfo::TKey& aKey = boost::get<0>(theInfo);

      myGeom = boost::get<0>(aKey);
      myRefCoord.resize(GetNbRef()*GetDim());

      TInt aNbGauss = boost::get<1>(theInfo);
      myGaussCoord.resize(aNbGauss*GetDim());
      myWeight.resize(aNbGauss);
    }
  };


  //---------------------------------------------------------------
  template<TInt nPNOM, TInt nDESC, TInt nIDENT, TInt nNOM, TInt nLNOM, EVersion nV>
  struct TTTimeStampInfo: virtual TTimeStampInfo
  {
    TTTimeStampInfo(const PFieldInfo& theFieldInfo, const PTimeStampInfo& theInfo)
    {
      myFieldInfo = theFieldInfo;

      myEntity = theInfo->GetEntity();
      myGeom2Size = theInfo->GetGeom2Size();

      myNbGauss = theInfo->GetNbGauss();
      myNumDt = theInfo->GetNumDt();
      myNumOrd = theInfo->GetNumOrd();
      myDt = theInfo->GetDt();

      myUnitDt.resize(nPNOM+1);
      SetUnitDt(theInfo->GetUnitDt());

      myGeom2Gauss = theInfo->GetGeom2Gauss();
    }

    TTTimeStampInfo(const PFieldInfo& theFieldInfo, 
		    EEntiteMaillage theEntity,
		    const TGeom2Size& theGeom2Size,
		    TInt theNbGauss,
		    TInt theNumDt,
		    TInt theNumOrd,
		    TFloat theDt,
		    const std::string& theUnitDt,
		    const TGeom2Gauss& theGeom2Gauss)
    {
      myFieldInfo = theFieldInfo;

      myEntity = theEntity;
      myGeom2Size = theGeom2Size;

      myNbGauss = theNbGauss;
      myNumDt = theNumDt;
      myNumOrd = theNumDt;
      myDt = theDt;

      myUnitDt.resize(nPNOM+1);
      SetUnitDt(theUnitDt);

      myGeom2Gauss = theGeom2Gauss;
    }

    virtual 
    std::string
    GetUnitDt() const
    { 
      return GetString(0,nPNOM,myUnitDt);
    }

    virtual
    void
    SetUnitDt(const std::string& theValue)
    {
      SetString(0,nPNOM,myUnitDt,theValue);
    }
  };


  //---------------------------------------------------------------
  template<TInt nPNOM, TInt nDESC, TInt nIDENT, TInt nNOM, TInt nLNOM, EVersion nV>
  struct TTProfileInfo: 
    virtual TProfileInfo,
    virtual TTNameInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV>
  {
    typedef TTNameInfo<nPNOM,nDESC,nIDENT,nNOM,nLNOM,nV> TNameInfoBase;

    TTProfileInfo(const TProfileInfo::TInfo& theInfo,
		  EModeProfil theMode):
      TNameInfoBase(boost::get<0>(theInfo))
    {
      TInt aSize = boost::get<1>(theInfo);
      myElemNum.resize(aSize);
      myMode = aSize > 0? theMode: eNO_PFLMOD;
    }
  };


  //---------------------------------------------------------------
  template<TInt nPNOM, TInt nDESC, TInt nIDENT, TInt nNOM, TInt nLNOM, EVersion nV>
  struct TTTimeStampVal: virtual TTimeStampVal
  {
    TTTimeStampVal(const PTimeStampInfo& theTimeStampInfo,
		   const PTimeStampVal& theInfo)
    {
      myTimeStampInfo = theTimeStampInfo;

      myGeom2Profile = theInfo->GetGeom2Profile();

      myGeom2Value = theInfo->myGeom2Value;
    }

    TTTimeStampVal(const PTimeStampInfo& theTimeStampInfo,
		   const TGeom2Profile& theGeom2Profile,
		   EModeSwitch theMode):
      TModeSwitchInfo(theMode)
    {
      myTimeStampInfo = theTimeStampInfo;

      myGeom2Profile = theGeom2Profile;

      TInt aNbComp = theTimeStampInfo->myFieldInfo->myNbComp;
      TInt aNbGauss = theTimeStampInfo->myNbGauss;

      const TGeom2Size& aGeom2Size = theTimeStampInfo->myGeom2Size;
      TGeom2Size::const_iterator anIter = aGeom2Size.begin();
      for(; anIter != aGeom2Size.end(); anIter++){
	const EGeometrieElement& aGeom = anIter->first;
	TInt aNbElem = anIter->second;

	MED::PProfileInfo aProfileInfo;
	MED::TGeom2Profile::const_iterator anIter = theGeom2Profile.find(aGeom);
	if(anIter != theGeom2Profile.end())
	  aProfileInfo = anIter->second;

	if(aProfileInfo && aProfileInfo->IsPresent())
	  aNbElem = aProfileInfo->GetSize();

	TMeshValue& aMeshValue = GetMeshValue(aGeom);
	aMeshValue.Init(aNbElem,aNbGauss,aNbComp);
      }
    }
  };

}

#endif
