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

#include "MED_Utilities.hxx"
#include "MED_Algorithm.hxx"
#include "MED_Factory.hxx"

#ifdef _DEBUG_
static int MYDEBUG = 1;
#else
static int MYDEBUG = 0;
#endif

static int MYWRITEDEBUG = 1;

using namespace MED;

#undef _DEBUG_

void CheckMed(const std::string& theFileName)
{
  MSG(MYDEBUG,"CheckMed - theFileName = '"<<theFileName<<"'");
  {
    //TErr anErr;
    PWrapper aMed = CrWrapper(theFileName);

    TKey2Gauss aKey2Gauss = GetKey2Gauss(aMed);
    TMKey2Profile aMKey2Profile = GetMKey2Profile(aMed);
    INITMSG(MYDEBUG,"aMed->GetNbProfiles() = "<<aMed->GetNbProfiles()<<endl);

    TInt aNbMeshes = aMed->GetNbMeshes();
    BEGMSG(MYDEBUG,"GetNbMeshes() = "<<aNbMeshes<<endl);
    for(TInt iMesh = 1; iMesh <= aNbMeshes; iMesh++){
      
      PMeshInfo aMeshInfo = aMed->GetPMeshInfo(iMesh);
      INITMSG(MYDEBUG,"aMeshInfo->GetName() = '"<<aMeshInfo->GetName()<<"'"<<endl);

      TEntityInfo aEntityInfo = aMed->GetEntityInfo(aMeshInfo);
      
      TElemGroup aElemGroup = GetElemsByEntity(aMed,aMeshInfo,aEntityInfo);

      TTimeStampGroup aTimeStampGroup = GetFieldsByEntity(aMed,aMeshInfo,aEntityInfo);
      
      TFieldGroup aFieldGroup = GetFieldsByEntity(aTimeStampGroup);

      TFieldGroup::const_iterator aFieldGroupIter = aFieldGroup.begin();
      for(; aFieldGroupIter != aFieldGroup.end(); aFieldGroupIter++){
	const TTimeStampGroup& aTTimeStampGroup = aFieldGroupIter->second;
	TTimeStampGroup::const_iterator aTTimeStampGroupIter = aTTimeStampGroup.begin();
	for(; aTTimeStampGroupIter != aTTimeStampGroup.end(); aTTimeStampGroupIter++){
	  PFieldInfo aFieldInfo = aTTimeStampGroupIter->first;
	  const TTimeStampSet& aTimeStampSet = aTTimeStampGroupIter->second;
	  TTimeStampSet::const_iterator aTTimeStampSetIter = aTimeStampSet.begin();
	  for(; aTTimeStampSetIter != aTimeStampSet.end(); aTTimeStampSetIter++){
	    PTimeStampInfo aTimeStampInfo = *aTTimeStampSetIter;
	    PTimeStampVal aTimeStampVal = aMed->GetPTimeStampVal(aTimeStampInfo,
								 aMKey2Profile,
								 aKey2Gauss);
	  }
	}
      }

      continue;

      TFamilyGroup aFamilyGroup = GetFamilies(aMed,aMeshInfo);
      
      TFamilyByEntity aFamilyByEntity = GetFamiliesByEntity(aMed,aElemGroup,aFamilyGroup);
      
      TGroupInfo aGroupInfo = GetFamiliesByGroup(aFamilyGroup);
      
      PNodeInfo aNodeInfo = aMed->GetPNodeInfo(aMeshInfo);
      TElemGroup::const_iterator anIter = aElemGroup.begin();
      for(; anIter != aElemGroup.end(); anIter++){
	const EEntiteMaillage& anEntity = anIter->first;
	if(anEntity != eNOEUD){
	  const TElemMap& anElemMap = anIter->second;
	  TKey2Gauss::const_iterator anIter2 = aKey2Gauss.begin();
	  for(; anIter2 != aKey2Gauss.end(); anIter2++){
	    const TGaussInfo::TKey& aKey = anIter2->first;
	    EGeometrieElement aGeom = boost::get<0>(aKey);
	    TElemMap::const_iterator anIter3 = anElemMap.find(aGeom);
	    if(anIter3 != anElemMap.end()){
	      PCellInfo aCellInfo = anIter3->second;
	      PGaussInfo aGaussInfo = anIter2->second;
	      TGaussCoord aGaussCoord;
	      GetGaussCoord3D(aGaussInfo,aCellInfo,aNodeInfo,aGaussCoord);
	    }
	  }
	}
      }
      
    }
  }
  MSG(MYDEBUG,"OK");
}


void CopyMed(const PWrapper& theMed, 
	     const PWrapper& theMed2, 
	     int theIncr)
{
  TKey2Gauss aKey2Gauss = GetKey2Gauss(*theMed);
  TMKey2Profile aMKey2Profile = GetMKey2Profile(*theMed);

  TInt aNbMeshes = theMed->GetNbMeshes();
  MSG(MYDEBUG,"aNbMeshes = "<<aNbMeshes);
  std::string aName;
  for(TInt iMesh = 0; iMesh < aNbMeshes; iMesh++){
    PMeshInfo aMeshInfo = theMed->GetPMeshInfo(iMesh+1);
    TInt aDim = aMeshInfo->myDim;
    aName = aMeshInfo->GetName();
    INITMSG(MYDEBUG,"GetMeshInfo - aName = '"<<aName<<"'; aDim = "<<aDim<<"\n");
    PMeshInfo aMeshInfo2 = theMed2->CrMeshInfo(aMeshInfo);
    if(MYWRITEDEBUG){
      aName = aMeshInfo2->GetName();

      aName[0] += theIncr;
      aMeshInfo2->SetName(aName);
      theMed2->SetMeshInfo(aMeshInfo2);
      BEGMSG(MYDEBUG,"aNbMeshes2 = "<<theMed2->GetNbMeshes()<<"\n");
    }
    
    TEntityInfo aEntityInfo = theMed->GetEntityInfo(aMeshInfo);

    //continue;

    TInt aNbFields = theMed->GetNbFields(); 
    MSG(MYDEBUG,"GetNbFields() = "<<aNbFields);
    for(TInt iField = 0; iField < aNbFields; iField++){
      PFieldInfo aFieldInfo = theMed->GetPFieldInfo(aMeshInfo,iField+1);
      TInt aNbComp = aFieldInfo->GetNbComp();
      INITMSG(MYDEBUG,"aName = '"<<aFieldInfo->GetName()<<"'; aNbComp = "<<aNbComp<<";\n");
      PFieldInfo aFieldInfo2 = theMed->CrFieldInfo(aMeshInfo2,aFieldInfo);

      if(MYWRITEDEBUG){
	aName = aFieldInfo->GetName();
	aName[0] += theIncr;
	aFieldInfo2->SetName(aName);
	theMed2->SetFieldInfo(aFieldInfo2);
      }

      EEntiteMaillage anEntity;
      TGeom2Size aGeom2Size;
      TInt aNbTimeStamps = 
	theMed->GetNbTimeStamps(aFieldInfo,aEntityInfo,anEntity,aGeom2Size);
      {
	INITMSG(MYDEBUG,"GetNbTimeStamps = "<<aNbTimeStamps<<endl);
	for(TInt iTimeStamp = 0; iTimeStamp < aNbTimeStamps; iTimeStamp++){
	  PTimeStampInfo aTimeStampInfo = 
	  theMed->GetPTimeStampInfo(aFieldInfo,anEntity,aGeom2Size,iTimeStamp+1);
	  TInt aNumDt = aTimeStampInfo->GetNumDt();
	  TInt aNbGauss = aTimeStampInfo->GetNbGauss();
	  INITMSG(MYDEBUG,"aNbGauss = "<<aNbGauss<<"; aNumDt = "<<aNumDt<<"\n");
	  
	  PTimeStampInfo aTimeStampInfo2 = 
	    theMed->CrTimeStampInfo(aFieldInfo2,aTimeStampInfo);
	  
	  PTimeStampVal aTimeStampVal = theMed->GetPTimeStampVal(aTimeStampInfo,
								 aMKey2Profile,
								 aKey2Gauss);
	  
	  PTimeStampVal aTimeStampVal2 = theMed->CrTimeStampVal(aTimeStampInfo2,
								aTimeStampVal);
	  
	  if(MYWRITEDEBUG) theMed2->SetTimeStamp(aTimeStampVal2);
	}
      }
    }

    TInt aNbFam = theMed->GetNbFamilies(aMeshInfo);
    MSG(MYDEBUG,"GetNbFamilies() = "<<aNbFam);
    for(TInt iFam = 0; iFam < aNbFam; iFam++){
      PFamilyInfo aFamilyInfo = theMed->GetPFamilyInfo(aMeshInfo,iFam+1);
      TInt aNbGroup = aFamilyInfo->GetNbGroup();
      TInt aNbAttr = aFamilyInfo->GetNbAttr();
      TInt anId = aFamilyInfo->GetId();
      if(anId == 0)
	continue;

      aName = aFamilyInfo->GetName();
      INITMSG(MYDEBUG,"aName = '"<<aName<<"'; anId = "<<anId<<
	      "; aNbAttr = "<<aNbAttr<<"; aNbGroup = "<<aNbGroup<<"\n");
      PFamilyInfo aFamilyInfo2 = theMed->CrFamilyInfo(aMeshInfo2,aFamilyInfo);
      for(TInt iGroup = 0; iGroup < aNbGroup; iGroup++){
	aName = aFamilyInfo->GetGroupName(iGroup);
	INITMSG(MYDEBUG,"aGroupName = '"<<aName<<"'\n");
	aName[0] += theIncr;
	aFamilyInfo2->SetGroupName(iGroup,aName);
      }

      if(MYWRITEDEBUG){
	aName = aFamilyInfo->GetName();
	aName[0] += theIncr;
	aFamilyInfo2->SetName(aName);
	theMed2->SetFamilyInfo(aFamilyInfo2);
	INITMSG(MYDEBUG,"GetNbFamilies = "<<theMed2->GetNbFamilies(aMeshInfo2)<<endl);
      }
    }

    MSG(MYDEBUG,"GetEntityInfo - aNbEntities = "<<aEntityInfo.size());
    TEntityInfo::iterator anEntityInfoIter = aEntityInfo.begin();
    for(; anEntityInfoIter != aEntityInfo.end(); anEntityInfoIter++){
      const EEntiteMaillage& anEntity = anEntityInfoIter->first;
      INITMSG(MYDEBUG,"anEntity = "<<anEntity<<"\n");
      if(anEntity == eNOEUD){
	PNodeInfo aNodeInfo = theMed->GetPNodeInfo(aMeshInfo);
	PNodeInfo aNodeInfo2 = theMed->CrNodeInfo(aMeshInfo2,aNodeInfo);
	if(MYWRITEDEBUG) theMed2->SetNodeInfo(aNodeInfo2);
	continue;
      }
      TGeom2Size& aGeom2Size = anEntityInfoIter->second;
      TGeom2Size::iterator aGeomIter = aGeom2Size.begin();
      for(; aGeomIter != aGeom2Size.end(); aGeomIter++){
	const EGeometrieElement& aGeom = aGeomIter->first;
	const TInt& aNbElem = aGeomIter->second;
	INITMSG(MYDEBUG,"aGeom = "<<aGeom<<"; aNbElem = "<<aNbElem<<": ");
	switch(aGeom){
	case ePOLYGONE: {
	  PPolygoneInfo aPolygoneInfo = theMed->GetPPolygoneInfo(aMeshInfo,anEntity,aGeom);
	  PPolygoneInfo aPolygoneInfo2 = theMed->CrPolygoneInfo(aMeshInfo2,aPolygoneInfo);
	  if(MYWRITEDEBUG) theMed2->SetPolygoneInfo(aPolygoneInfo2);
	  break;
	}
	case ePOLYEDRE: {
	  PPolyedreInfo aPolyedreInfo = theMed->GetPPolyedreInfo(aMeshInfo,anEntity,aGeom);
	  PPolyedreInfo aPolyedreInfo2 = theMed->CrPolyedreInfo(aMeshInfo2,aPolyedreInfo);
	  if(MYWRITEDEBUG) theMed2->SetPolyedreInfo(aPolyedreInfo2);
	  break;
	}
	default:
	  PCellInfo aCellInfo = theMed->GetPCellInfo(aMeshInfo,anEntity,aGeom);
	  PCellInfo aCellInfo2 = theMed2->CrCellInfo(aMeshInfo2,aCellInfo);
	  if(MYWRITEDEBUG) theMed2->SetCellInfo(aCellInfo2);
	}
      }
    }
    
  }
  MSG(MYDEBUG,"OK");
}


void CopyMed(const std::string& theFileName, 
	     const std::string& theFileName2, 
	     MED::EVersion theVersion,
	     int theNbCopy)
{
  MSG(MYDEBUG,"CopyMed - theFileName = '"<<theFileName<<"'; theFileName2 = '"<<theFileName2<<"', theVersion = "<<theVersion);

  PWrapper aMed = CrWrapper(theFileName);

  PWrapper aMed2 = CrWrapper(theFileName2,theVersion);

  for(int i = 0; i < theNbCopy; i++) 
    CopyMed(aMed,aMed2,i);
}


void ReadMed(const char* theFileName, 
	     const char* theFileName2,
	     MED::EVersion theVersion = eV2_2,
	     int theNbCopy = 1)
{
  MSG(MYDEBUG,"theFileName = '"<<theFileName<<"'; "<<
      "theFileName2 = '"<<theFileName2<<"'; "<<
      "theVersion = "<<theVersion<<"; "<<
      "theNbCopy = "<<theNbCopy);

  CopyMed(theFileName,theFileName2,theVersion,theNbCopy);
}


int main(int argc, char** argv){ 
#ifndef _DEBUG_
  try{
#endif
    if(argc == 2)
      CheckMed(argv[1]);
    if(argc == 3)
      ReadMed(argv[1],argv[2]);
    if(argc == 4)
      ReadMed(argv[1],argv[2],MED::EVersion(atoi(argv[3])));
    if(argc == 5)
      ReadMed(argv[1],argv[2],MED::EVersion(atoi(argv[3])),atoi(argv[4]));
    return 0;
#ifndef _DEBUG_
  }catch(std::exception& exc){
    MSG(MYDEBUG,"Follow exception was accured :\n"<<exc.what());
  }catch(...){
    MSG(MYDEBUG,"Unknown exception was accured");
  } 
#endif 
  return 1;
}
