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

#include "MED_Algorithm.hxx"
#include "MED_Wrapper.hxx"

#include "MED_Utilities.hxx"
 
#ifdef _DEBUG_
static int MYDEBUG = 0;
static int MYVALUEDEBUG = 0;
#else
static int MYDEBUG = 0;
static int MYVALUEDEBUG = 0;
#endif

namespace MED{

  TElemGroup 
  GetElemsByEntity(TWrapper& theWrapper, 
		   const PMeshInfo& theMeshInfo,
		   const MED::TEntityInfo& theEntityInfo)
  {
    MSG(MYDEBUG,"GetElemsByEntity(...)");
    TElemGroup aGroup;
    MED::TEntityInfo::const_iterator anIter = theEntityInfo.begin();
    for(; anIter != theEntityInfo.end(); anIter++){
      const EEntiteMaillage& anEntity = anIter->first;
      const TGeom2Size& aGeom2Size = anIter->second;
      TElemMap& anElemMap = aGroup[anEntity];

      if(anEntity == eNOEUD){
	anElemMap[ePOINT1] = theWrapper.GetPNodeInfo(theMeshInfo);
	continue;
      }

      TGeom2Size::const_iterator anIter2 = aGeom2Size.begin();
      for(; anIter2 != aGeom2Size.end(); anIter2++){
	const EGeometrieElement& aGeom = anIter2->first;
	switch(aGeom){
	case ePOLYGONE: {
	  anElemMap[ePOLYGONE] = theWrapper.GetPPolygoneInfo(theMeshInfo,anEntity,aGeom);
	  break;
	}
	case ePOLYEDRE: {
	  anElemMap[ePOLYEDRE] = theWrapper.GetPPolyedreInfo(theMeshInfo,anEntity,aGeom);
	  break;
	}
	default: {
	  anElemMap[aGeom] = theWrapper.GetPCellInfo(theMeshInfo,anEntity,aGeom);
	}}

      }
    }
    ADDMSG(MYDEBUG,"\n");
    return aGroup;
  }
  
  
  //---------------------------------------------------------------
  TFamilyGroup 
  GetFamilies(TWrapper& theWrapper,
	      const PMeshInfo& theMeshInfo)
  {
    MSG(MYDEBUG,"GetFamilies(...)");
    TErr anErr;
    TFamilyGroup aGroup;
    TInt aNbFam = theWrapper.GetNbFamilies(*theMeshInfo);
    INITMSG(MYDEBUG,"GetNbFamilies() = "<<aNbFam<<"\n");
    for(TInt iFam = 1; iFam <= aNbFam; iFam++){
      PFamilyInfo aFamilyInfo = theWrapper.GetPFamilyInfo(theMeshInfo,iFam,&anErr);
      if(anErr >= 0)
	aGroup.insert(aFamilyInfo);
    }
    ADDMSG(MYDEBUG,"\n");
    return aGroup;
  }


  //---------------------------------------------------------------
  TGroupInfo 
  GetFamiliesByGroup(const TFamilyGroup& theGroupInfo)
  {
    MSG(MYDEBUG,"GetFamiliesByGroup(...)");
    TGroupInfo aGroup;
    TFamilyGroup::const_iterator anIter = theGroupInfo.begin();
    for(; anIter != theGroupInfo.end(); anIter++){
      const PFamilyInfo& aFamilyInfo = *anIter;
      TInt aNbGroup = aFamilyInfo->GetNbGroup();
      for(TInt iGroup = 0; iGroup < aNbGroup; iGroup++){
	aGroup[aFamilyInfo->GetGroupName(iGroup)].insert(aFamilyInfo);
      } 
    }

#ifdef _DEBUG_
    if(MYDEBUG){
      TGroupInfo::const_iterator anIter = aGroup.begin();
      for(; anIter != aGroup.end(); anIter++){
	string aName = anIter->first;
	INITMSG(MYDEBUG,"aGroupName = '"<<aName<<"'\n");
	const TFamilyGroup& aFamilyGroup = anIter->second;
	TFamilyGroup::const_iterator anFamIter = aFamilyGroup.begin();
	for(; anFamIter != aFamilyGroup.end(); anFamIter++){
	  const PFamilyInfo& aFamilyInfo = *anFamIter;
	  INITMSG(MYDEBUG,"aFamilyName = '"<<aFamilyInfo->GetName()<<"'\n");
	}
      }
      ADDMSG(MYDEBUG,"\n");
    }
#endif

    return aGroup;
  }


  //---------------------------------------------------------------
  TTimeStampGroup 
  GetFieldsByEntity(TWrapper& theWrapper, 
		    const PMeshInfo& theMeshInfo,
		    const MED::TEntityInfo& theEntityInfo)
  {
    MSG(MYDEBUG,"GetFieldsByEntity(...)");
    TTimeStampGroup aGroup;
    TInt aNbFields = theWrapper.GetNbFields();
    INITMSG(MYDEBUG,"GetNbFields() = "<<aNbFields<<"\n");
    for(TInt iField = 1; iField <= aNbFields; iField++){
      PFieldInfo aFieldInfo = theWrapper.GetPFieldInfo(theMeshInfo,iField);
      INITMSG(MYDEBUG,"aFieldName = '"<<aFieldInfo->GetName()<<
	      "'; aNbComp = "<<aFieldInfo->GetNbComp()<<"; ");
      TGeom2Size aGeom2Size;
      EEntiteMaillage anEntity = EEntiteMaillage(-1);
      TInt aNbTimeStamps = theWrapper.GetNbTimeStamps(aFieldInfo,theEntityInfo,anEntity,aGeom2Size);
      ADDMSG(MYDEBUG,"anEntity = "<<anEntity<<"; GetNbTimeStamps = "<<aNbTimeStamps<<"\n");
      for(TInt iTimeStamp = 1; iTimeStamp <= aNbTimeStamps; iTimeStamp++){
	PTimeStampInfo aTimeStamp = 
	  theWrapper.GetPTimeStampInfo(aFieldInfo,anEntity,aGeom2Size,iTimeStamp);
	aGroup[aFieldInfo].insert(aTimeStamp);
	INITMSG(MYDEBUG,"aDt = "<<aTimeStamp->GetDt()
		<<", Unit = \'"<<aTimeStamp->GetUnitDt()
		<<"\', aNbGauss = "<<aTimeStamp->GetNbGauss()<<"\n");
      }
    }
    ADDMSG(MYDEBUG,"\n");
    return aGroup;
  }
  

  //---------------------------------------------------------------
  TFieldGroup 
  GetFieldsByEntity(const TTimeStampGroup& theTimeStampGroup)
  {
    TFieldGroup aGroup;
    TTimeStampGroup::const_iterator anIter = theTimeStampGroup.begin();
    for(; anIter != theTimeStampGroup.end(); anIter++){
      //const PFieldInfo& aFieldInfo = anIter->first;
      const TTimeStampSet& aTimeStampSet = anIter->second;
      if(aTimeStampSet.empty()) continue;
      const PTimeStampInfo& aTimeStampInfo = *aTimeStampSet.begin();
      aGroup[aTimeStampInfo->GetEntity()].insert(*anIter);
    }
    return aGroup;
  }
  

  //---------------------------------------------------------------
  TFamilyByEntity
  GetFamiliesByEntity(TWrapper& theWrapper, 
		      const TElemGroup& theElemGroup,
		      const TFamilyGroup& theFamilyGroup)
  {
    MSG(MYDEBUG,"GetFamiliesByEntity(...)");
    TFamilyByEntity aFamilyByEntity;
    
    typedef map<TInt,PFamilyInfo> TFamilyByIdMap;
    TFamilyByIdMap aFamilyByIdMap;
    TFamilyGroup::const_iterator anIter = theFamilyGroup.begin();
    for(; anIter != theFamilyGroup.end(); anIter++){
      const PFamilyInfo& aFamilyInfo = *anIter;
      aFamilyByIdMap.insert(TFamilyByIdMap::value_type(aFamilyInfo->GetId(),aFamilyInfo));
    }
    
    if(!aFamilyByIdMap.empty()){
      typedef set<TInt> TFamilyIdSet;
      typedef map<EEntiteMaillage,TFamilyIdSet> TFamilyIdByEntity;
      TFamilyIdByEntity aFamilyIdByEntity;
      
      if(!theElemGroup.empty()){
	TElemGroup::const_iterator anIter = theElemGroup.begin();
	for(; anIter != theElemGroup.end(); anIter++){
	  const EEntiteMaillage& anEntity = anIter->first;
	  TFamilyIdSet& aFamilyIdSet = aFamilyIdByEntity[anEntity];
	  const TElemMap& anElemMap = anIter->second;
	  TElemMap::const_iterator anElemIter = anElemMap.begin();
	  for(; anElemIter != anElemMap.end(); anElemIter++){
	    const PElemInfo& aElemInfo = anElemIter->second;
	    if(TInt aNbElem = aElemInfo->GetNbElem()){
	      for(TInt i = 0; i < aNbElem; i++){
		aFamilyIdSet.insert(aElemInfo->GetFamNum(i));
	      }
	    }
	  }
	}
      }
      
      if(!aFamilyIdByEntity.empty()){
	TFamilyIdByEntity::const_iterator anIter = aFamilyIdByEntity.begin();
	for(; anIter != aFamilyIdByEntity.end(); anIter++){
	  const EEntiteMaillage& anEntity = anIter->first;
	  INITMSG(MYDEBUG,"anEntity = "<<anEntity<<":\n");
	  const TFamilyIdSet& aFamilyIdSet = anIter->second;
	  TFamilyIdSet::const_iterator anFamilyIdIter = aFamilyIdSet.begin();
	  for(; anFamilyIdIter != aFamilyIdSet.end(); anFamilyIdIter++){
	    const TInt& aFamilyId = *anFamilyIdIter;
	    TFamilyByIdMap::const_iterator 
	      anFamilyByIdMapIter = aFamilyByIdMap.find(aFamilyId);
	    if(anFamilyByIdMapIter != aFamilyByIdMap.end()){
	      const PFamilyInfo& aFamilyInfo = anFamilyByIdMapIter->second;
	      aFamilyByEntity[anEntity].insert(aFamilyInfo);
	      INITMSG(MYDEBUG,
		      "aFamilyName = '"<<aFamilyInfo->GetName()<<
		      "' anId = "<<aFamilyInfo->GetId()<<"\n");
	    }
	  }
	}
      }
    }    
    ADDMSG(MYDEBUG,"\n");
    return aFamilyByEntity;
  }
  

  //---------------------------------------------------------------
  TKey2Gauss
  GetKey2Gauss(TWrapper& theWrapper, 
	       TErr* theErr,
	       EModeSwitch theMode)
  {
    TKey2Gauss aKey2Gauss;
    TInt aNbGauss = theWrapper.GetNbGauss(theErr);
    for(TInt anId = 1; anId <= aNbGauss; anId++){
      TGaussInfo::TInfo aPreInfo = theWrapper.GetGaussPreInfo(anId);
      PGaussInfo anInfo = theWrapper.CrGaussInfo(aPreInfo,theMode);
      theWrapper.GetGaussInfo(anId,anInfo,theErr);
      TGaussInfo::TKey aKey = boost::get<0>(aPreInfo);
      aKey2Gauss[aKey] = anInfo;
      break;
    }
    return aKey2Gauss;
  }


  //---------------------------------------------------------------
  PProfileInfo
  GetProfileInfo(TWrapper& theWrapper, 
		 const std::string& theProfileName,
		 TErr* theErr,
		 EModeProfil theMode)
  {
    PProfileInfo anInfo;
    TInt aNbProfiles = theWrapper.GetNbProfiles(theErr);
    for(TInt anId = 1; anId <= aNbProfiles; anId++){
      TProfileInfo::TInfo aTInfo = theWrapper.GetProfilePreInfo(anId);
      std::string aName = boost::get<0>(aTInfo);
      if(aName == theProfileName){
	TInt aSize = boost::get<1>(aTInfo);
	anInfo = theWrapper.CrProfileInfo(aName,theMode,aSize);
	theWrapper.GetProfileInfo(anId,anInfo,theErr);
	break;
      }
    }
    return anInfo;
  }
  

  //---------------------------------------------------------------
  TMKey2Profile
  GetMKey2Profile(TWrapper& theWrapper, 
		  TErr* theErr,
		  EModeProfil theMode)
  {
    TKey2Profile aKey2Profile;
    TInt aNbProfiles = theWrapper.GetNbProfiles(theErr);
    for(TInt anId = 1; anId <= aNbProfiles; anId++){
      TProfileInfo::TInfo aPreInfo = theWrapper.GetProfilePreInfo(anId);
      std::string aName = boost::get<0>(aPreInfo);
      TInt aSize = boost::get<1>(aPreInfo);
      PProfileInfo anInfo = theWrapper.CrProfileInfo(aName,theMode,aSize);
      theWrapper.GetProfileInfo(anId,anInfo,theErr);
      aKey2Profile[aName] = anInfo;

#ifdef _DEBUG_
    INITMSG(MYDEBUG,
	    "GetProfileInfo - aName = '"<<anInfo->GetName()<<"'"<<
	    endl);
    BEGMSG(MYVALUEDEBUG,"GetProfileInfo - GetElemNum: ");
    TInt aNbElem = anInfo->myElemNum.size();
    for(TInt iElem = 0; iElem < aNbElem; iElem++){
      ADDMSG(MYVALUEDEBUG,anInfo->GetElemNum(iElem)<<", ");
    }
    ADDMSG(MYVALUEDEBUG,endl);
#endif

    }
    return TMKey2Profile(theMode,aKey2Profile);
  }


  //---------------------------------------------------------------
  inline
  bool 
  IsEqual(TFloat theLeft, TFloat theRight)
  {
    static TFloat EPS = 1.0E-3;
    return fabs(theLeft-theRight)/(fabs(theLeft)+fabs(theRight)) < EPS;
  }


  struct TShapeFun
  {
    typedef std::vector<TCCoordSlice> TSliceArr;

    void 
    GetFun(const TShapeFun::TSliceArr& theRef,
	   const TShapeFun::TSliceArr& theGauss,
	   TFloatVector& theFun) const
    {
      TInt aNbRef = theRef.size();
      TInt aNbGauss = theGauss.size();
      theFun.resize(aNbGauss*aNbRef);
    }

    virtual 
    void
    InitFun(const TShapeFun::TSliceArr& theRef,
	    const TShapeFun::TSliceArr& theGauss,
	    TFloatVector& theFun) const
    {
      GetFun(theRef,theGauss,theFun);
    }
    
    virtual
    bool 
    IsSatisfy(const TShapeFun::TSliceArr& theRefCoord) const
    {
#ifdef _DEBUG_
      int MYDEBUG = true;

      TInt aNbRef = theRefCoord.size();
      INITMSG(MYDEBUG,"TShapeFun::IsSatisfy - aNbRef = "<<aNbRef<<": ");

      for(TInt aRefId = 0; aRefId < aNbRef; aRefId++){
	const TCCoordSlice& aCoord = theRefCoord[aRefId];
	TInt aDim = aCoord.size();
	ADDMSG(MYDEBUG,"{");
	for(TInt anId = 0; anId < aDim; anId++){
	  ADDMSG(MYDEBUG,aCoord[anId]<<" ");
	}
	ADDMSG(MYDEBUG,"} ");
      }
      ADDMSG(MYDEBUG,endl);
#endif

      return false;
    }

    bool
    Eval(const TCellInfo& theCellInfo,
	 const TNodeInfo& theNodeInfo,
	 const TElemNum& theElemNum,
	 const TShapeFun::TSliceArr& theRef,
	 const TShapeFun::TSliceArr& theGauss,
	 TNodeCoord& theGaussCoord)
    {
      int MYDEBUG = true;
      INITMSG(MYDEBUG,"TShapeFun::Eval"<<endl);

      if(IsSatisfy(theRef)){
	const PMeshInfo& aMeshInfo = theCellInfo.GetMeshInfo();
	TInt aDim = aMeshInfo->GetDim();
	TInt aNbGauss = theGauss.size();
	
	bool anIsSubMesh = !theElemNum.empty();
	TInt aNbElem;
	if(anIsSubMesh)
	  aNbElem = theElemNum.size();
	else
	  aNbElem = theCellInfo.GetNbElem();
	
	theGaussCoord.resize(aNbElem*aNbGauss*aDim);
	
	TFloatVector aFun;
	InitFun(theRef,theGauss,aFun);
	TInt aConnDim = theCellInfo.GetConnDim();
	
	TInt aNbNodes = theNodeInfo.GetNbElem();
	INITMSG(MYDEBUG,"aDim = "<<aDim<<
		"; aNbGauss = "<<aNbGauss<<
		"; aNbElem = "<<aNbElem<<
		"; aNbNodes = "<<aNbNodes<<
		endl);
	for(TInt anElemId = 0; anElemId < aNbElem; anElemId++){
	  TInt aCellId = anElemId;
	  if(anIsSubMesh)
	    aCellId = theElemNum[anElemId] - 1;
	  INITMSG(MYDEBUG,"aCellId = "<<aCellId<<":\n");

	  TCConnSlice aConnSlice = theCellInfo.GetConnSlice(anElemId);

	  for(TInt aGaussId = 0; aGaussId < aNbGauss; aGaussId++){
	    INITMSG(MYDEBUG,"aGaussId = "<<aGaussId<<": ");
	    for(TInt aConnId = 0; aConnId < aConnDim; aConnId++){
	      TInt aNodeId = aConnSlice[aConnId] - 1;      
	      TCCoordSlice aCoordSlice = theNodeInfo.GetCoordSlice(aNodeId);
	      TInt anGaussFunId = aConnId*aNbGauss+aGaussId;
	      ADDMSG(MYDEBUG,aNodeId<<"-"<<aConnId<<" {");
	      for(TInt aDimId = 0; aDimId < aDim; aDimId++){
		TInt anGaussPntId = anElemId*aNbGauss*aDim+aGaussId*aDim+aDimId;
		theGaussCoord[anGaussPntId] += aCoordSlice[aDimId] * aFun[anGaussFunId];
		//ADDMSG(MYDEBUG,anGaussPntId<<"-"<<anGaussFunId<<" ");
		//ADDMSG(MYDEBUG,anGaussPntId<<" ");
	      }
	      ADDMSG(MYDEBUG,"} ");
	    }
	    ADDMSG(MYDEBUG,endl);
	  }
	}

#ifdef _DEBUG_
	for(TInt anElemId = 0, anId = 0; anElemId < aNbElem; anElemId++){
	  INITMSG(MYDEBUG,anElemId<<": ");
	  for(TInt aGaussId = 0; aGaussId < aNbGauss; aGaussId++){
	    ADDMSG(MYDEBUG,"{");
	    for(TInt aDimId = 0; aDimId < aDim; aDimId++){
	      ADDMSG(MYDEBUG,theGaussCoord[anId++]<<" ");
	    }
	    ADDMSG(MYDEBUG,"} ");
	  }
	  ADDMSG(MYDEBUG,endl);
	}
#endif
	return true;
      }

      return false;
    }
  };


  struct TTria6a: TShapeFun
  {
    virtual 
    void
    InitFun(const TShapeFun::TSliceArr& theRef,
	    const TShapeFun::TSliceArr& theGauss,
	    TFloatVector& theFun) const
    {
    }

    virtual 
    bool 
    IsSatisfy(const TShapeFun::TSliceArr& theRef) const
    {
      return IsEqual(theRef[0][0],-1.0);
    }
  };


  struct TTria6b: TShapeFun
  {
    virtual 
    void
    InitFun(const TShapeFun::TSliceArr& theRef,
	    const TShapeFun::TSliceArr& theGauss,
	    TFloatVector& theFun) const
    {
      GetFun(theRef,theGauss,theFun);

      TInt aNbGauss = theGauss.size();
      for(TInt aGaussId = 0, anId = 0; aGaussId < aNbGauss; aGaussId++){
	const TCCoordSlice& aCoord = theGauss[aGaussId];
	theFun[anId++] = (1.0 - aCoord[0] - aCoord[1])*(1.0 - 2.0*aCoord[0] - 2.0*aCoord[1]);
	theFun[anId++] = aCoord[0]*(2.0*aCoord[0] - 1.0);
	theFun[anId++] = aCoord[1]*(2.0*aCoord[1] - 1.0);
	
	theFun[anId++] = 4.0*aCoord[0]*(1.0 - aCoord[0] - aCoord[1]);
	theFun[anId++] = 4.0*aCoord[0]*aCoord[1];
	theFun[anId++] = 4.0*aCoord[1]*(1.0 - aCoord[0] - aCoord[1]);
      }
    }

    virtual 
    bool
    IsSatisfy(const TShapeFun::TSliceArr& theRef) const
    {
      return IsEqual(theRef[2][0],1.0);
    }
  };


  struct TQuad4a: TShapeFun
  {
    virtual 
    void
    InitFun(const TShapeFun::TSliceArr& theRef,
	    const TShapeFun::TSliceArr& theGauss,
	    TFloatVector& theFun) const
    {
      GetFun(theRef,theGauss,theFun);

      TInt aNbGauss = theGauss.size();
      for(TInt aGaussId = 0, anId = 0; aGaussId < aNbGauss; aGaussId++){
	const TCCoordSlice& aCoord = theGauss[aGaussId];
	theFun[anId++] = 0.25*(1.0 + aCoord[1])*(1.0 - aCoord[0]);
	theFun[anId++] = 0.25*(1.0 - aCoord[1])*(1.0 - aCoord[0]);
	theFun[anId++] = 0.25*(1.0 - aCoord[1])*(1.0 + aCoord[0]);
	theFun[anId++] = 0.25*(1.0 + aCoord[0])*(1.0 + aCoord[1]);
      }
    }

    virtual 
    bool
    IsSatisfy(const TShapeFun::TSliceArr& theRef) const
    {
      return IsEqual(theRef[0][0],-1.0);
    }
  };


  struct THexa8a: TShapeFun
  {
    virtual 
    void
    InitFun(const TShapeFun::TSliceArr& theRef,
	    const TShapeFun::TSliceArr& theGauss,
	    TFloatVector& theFun) const
    {
      GetFun(theRef,theGauss,theFun);

      TInt aNbGauss = theGauss.size();
      for(TInt aGaussId = 0, anId = 0; aGaussId < aNbGauss; aGaussId++){
	const TCCoordSlice& aCoord = theGauss[aGaussId];
	theFun[anId++] = 0.125*(1.0 - aCoord[0])*(1.0 - aCoord[1])*(1.0 - aCoord[2]);
	theFun[anId++] = 0.125*(1.0 + aCoord[0])*(1.0 - aCoord[1])*(1.0 - aCoord[2]);
	theFun[anId++] = 0.125*(1.0 + aCoord[0])*(1.0 + aCoord[1])*(1.0 - aCoord[2]);
	theFun[anId++] = 0.125*(1.0 - aCoord[0])*(1.0 + aCoord[1])*(1.0 - aCoord[2]);

	theFun[anId++] = 0.125*(1.0 - aCoord[0])*(1.0 - aCoord[1])*(1.0 + aCoord[2]);
	theFun[anId++] = 0.125*(1.0 + aCoord[0])*(1.0 - aCoord[1])*(1.0 + aCoord[2]);
	theFun[anId++] = 0.125*(1.0 + aCoord[0])*(1.0 + aCoord[1])*(1.0 + aCoord[2]);
	theFun[anId++] = 0.125*(1.0 - aCoord[0])*(1.0 + aCoord[1])*(1.0 + aCoord[2]);
      }
    }

    virtual 
    bool
    IsSatisfy(const TShapeFun::TSliceArr& theRef) const
    {
      return IsEqual(theRef[0][0],-1.0);
    }
  };


  //---------------------------------------------------------------
  void
  GetGaussCoord3D(const TGaussInfo& theGaussInfo, 
		  const TCellInfo& theCellInfo,
		  const TNodeInfo& theNodeInfo,
		  TNodeCoord& theGaussCoord,
		  const TElemNum& theElemNum)
  {
    int MYDEBUG = 1;
    INITMSG(MYDEBUG,"GetGaussCoord3D\n");

    if(theGaussInfo.myGeom == theCellInfo.myGeom){
      EGeometrieElement aGeom = theGaussInfo.myGeom;

      typedef std::vector<TCCoordSlice> TSliceArr;
      TInt aNbRef = theGaussInfo.GetNbRef();
      TSliceArr aRefSlice(aNbRef);
      for(TInt anId = 0; anId < aNbRef; anId++)
	aRefSlice[anId] = theGaussInfo.GetRefCoordSlice(anId);

      TInt aNbGauss = theGaussInfo.GetNbGauss();
      TSliceArr aGaussSlice(aNbGauss);
      for(TInt anId = 0; anId < aNbGauss; anId++)
	aGaussSlice[anId] = theGaussInfo.GetGaussCoordSlice(anId);

      switch(aGeom){
      case eSEG2: {
	INITMSG(MYDEBUG,"eSEG2"<<endl);
	break;
      }
      case eSEG3: {
	INITMSG(MYDEBUG,"eSEG3"<<endl);
	break;
      }
      case eTRIA3: {
	INITMSG(MYDEBUG,"eTRIA3"<<endl);
	break;
      }
      case eTRIA6: {
	INITMSG(MYDEBUG,"eTRIA6"<<endl);

	if(TTria6a().Eval(theCellInfo,theNodeInfo,theElemNum,aRefSlice,aGaussSlice,theGaussCoord))
	  return;

	if(TTria6b().Eval(theCellInfo,theNodeInfo,theElemNum,aRefSlice,aGaussSlice,theGaussCoord))
	  return;
	
	break;
      }
      case eQUAD4: {
	INITMSG(MYDEBUG,"eQUAD4"<<endl);

	if(TQuad4a().Eval(theCellInfo,theNodeInfo,theElemNum,aRefSlice,aGaussSlice,theGaussCoord))
	  return;

	break;
      }
      case eQUAD8: {
	INITMSG(MYDEBUG,"eQUAD8"<<endl);
	break;
      }
      case eTETRA4: {
	INITMSG(MYDEBUG,"eTETRA4"<<endl);
	break;
      }
      case ePYRA5: {
	INITMSG(MYDEBUG,"ePYRA5"<<endl);
	break;
      }
      case ePENTA6: {
	INITMSG(MYDEBUG,"ePENTA6"<<endl);
	break;
      }
      case eHEXA8: {
	INITMSG(MYDEBUG,"eHEXA8"<<endl);

	if(THexa8a().Eval(theCellInfo,theNodeInfo,theElemNum,aRefSlice,aGaussSlice,theGaussCoord))
	  return;

	break;
      }
      case eTETRA10: {
	INITMSG(MYDEBUG,"eTETRA10"<<endl);
	break;
      }
      case ePYRA13: {
	INITMSG(MYDEBUG,"ePYRA13"<<endl);
	break;
      }
      case ePENTA15: {
	INITMSG(MYDEBUG,"ePENTA15"<<endl);
	break;
      }
      case eHEXA20: {
	INITMSG(MYDEBUG,"eHEXA20"<<endl);
	break;
      }}

#ifdef _DEBUG_
#endif
    }
  }
}
