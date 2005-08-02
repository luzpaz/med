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

namespace MED
{
  //---------------------------------------------------------------
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
	const std::string& aName = anIter->first;
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
    INITMSG(MYDEBUG,"GetKey2Gauss - theMode = "<<theMode<<endl);
    TKey2Gauss aKey2Gauss;
    TInt aNbGauss = theWrapper.GetNbGauss(theErr);
    for(TInt anId = 1; anId <= aNbGauss; anId++){
      TGaussInfo::TInfo aPreInfo = theWrapper.GetGaussPreInfo(anId);
      PGaussInfo anInfo = theWrapper.CrGaussInfo(aPreInfo,theMode);
      theWrapper.GetGaussInfo(anId,anInfo,theErr);
      TGaussInfo::TKey aKey = boost::get<0>(aPreInfo);
      aKey2Gauss[aKey] = anInfo;

#ifdef _DEBUG_
      const EGeometrieElement& aGeom = boost::get<0>(aKey);
      const std::string& aName = boost::get<1>(aKey);
      INITMSG(MYDEBUG,
	      "- aGeom = "<<aGeom<<
	      "; aName = '"<<aName<<"'"<<
	      endl);
#endif

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
      TProfileInfo::TInfo aPreInfo = theWrapper.GetProfilePreInfo(anId);
      const std::string& aName = boost::get<0>(aPreInfo);
      if(aName == theProfileName)
	return theWrapper.GetPProfileInfo(anId,theMode,theErr);
    }
    return anInfo;
  }
  

  //---------------------------------------------------------------
  TMKey2Profile
  GetMKey2Profile(TWrapper& theWrapper, 
		  TErr* theErr,
		  EModeProfil theMode)
  {
    INITMSG(MYDEBUG,"GetMKey2Profile - theMode = "<<theMode<<endl);
    TKey2Profile aKey2Profile;
    TInt aNbProfiles = theWrapper.GetNbProfiles(theErr);
    for(TInt anId = 1; anId <= aNbProfiles; anId++){
      TProfileInfo::TInfo aPreInfo = theWrapper.GetProfilePreInfo(anId);
      PProfileInfo anInfo = theWrapper.GetPProfileInfo(anId,theMode,theErr);
      const std::string& aName = boost::get<0>(aPreInfo);
      aKey2Profile[aName] = anInfo;
      
#ifdef _DEBUG_
      INITMSG(MYDEBUG,
	      "- aName = '"<<aName<<"'"<<
	      " : "<<
	      endl);
      TInt aNbElem = anInfo->myElemNum.size();
      for(TInt iElem = 0; iElem < aNbElem; iElem++){
	ADDMSG(MYVALUEDEBUG,anInfo->GetElemNum(iElem)<<", ");
      }
      ADDMSG(MYVALUEDEBUG,endl);
#endif
      
    }
    return TMKey2Profile(theMode,aKey2Profile);
  }

}
