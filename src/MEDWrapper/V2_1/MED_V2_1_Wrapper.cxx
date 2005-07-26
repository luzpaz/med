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

#include "MED_V2_1_Wrapper.hxx"
#include "MED_Algorithm.hxx"
#include "MED_Utilities.hxx"

#include "med.hxx"
using namespace med_2_1;

#ifdef _DEBUG_
static int MYDEBUG = 0;
#else
static int MYDEBUG = 0;
#endif



namespace MED{
  namespace V2_1{

    //---------------------------------------------------------------
    class TFile{
      TFile();
      TFile(const TFile&);
      
    public:
      TFile(const std::string& theFileName): 
	myFid(-1), 
	myCount(0),
	myFileName(theFileName)
      {}
      
      ~TFile(){ 
	Close();
      }
      
      void Open(EModeAcces theMode, TErr* theErr = NULL){
	if(myCount++ == 0){
	  char* aFileName = const_cast<char*>(myFileName.c_str());
	  myFid = MEDouvrir(aFileName,med_mode_acces(theMode));
	}
	if(theErr){
	  *theErr = TErr(myFid);
	  INITMSG(MYDEBUG && myFid < 0,"TFile::Open - MED_MODE_ACCES = "<<theMode<<"; myFid = "<<myFid<<endl);
	}else if(myFid < 0)
	  EXCEPTION(runtime_error,"TFile - MEDouvrir('"<<myFileName<<"',"<<theMode<<")");
      }

      const TIdt& Id() const { 
	if(myFid < 0)
	  EXCEPTION(runtime_error,"TFile - GetFid() < 0");
	return myFid;
      }

      void Close(){ 
	if(--myCount == 0)
	  MEDfermer(myFid);
      }

    protected:
      std::string myFileName;
      TInt myCount;
      TIdt myFid;
    };


    //---------------------------------------------------------------
    class TFileWrapper{
      PFile myFile;

    public:
      TFileWrapper(const PFile& theFile, EModeAcces theMode, TErr* theErr = NULL): 
	myFile(theFile)
      {
	myFile->Open(theMode,theErr);
      }
      
      ~TFileWrapper(){
	myFile->Close();
      }
    };


    //---------------------------------------------------------------
    TVWrapper
    ::TVWrapper(const std::string& theFileName): 
      myFile(new TFile(theFileName))
    {}
    
    
    TInt
    TVWrapper
    ::GetNbMeshes(TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      return MEDnMaa(myFile->Id());
    }
    
    
    void
    TVWrapper
    ::GetMeshInfo(TInt theMeshId, 
		  TMeshInfo& theInfo,
		  TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TErr aRet = MEDmaaInfo(myFile->Id(),
			     theMeshId,
			     &theInfo.myName[0],
			     (med_int*)&theInfo.myDim);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetMeshInfo - MEDmaaInfo(...)");
    }
    
    
    void 
    TVWrapper
    ::SetMeshInfo(const TMeshInfo& theInfo,
		  EModeAcces theMode,
		  TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,theMode,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TMeshInfo& anInfo = const_cast<TMeshInfo&>(theInfo);
      
      TErr aRet = MEDmaaCr(myFile->Id(),
			   &anInfo.myName[0],
			   anInfo.myDim);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"SetMeshInfo - MEDmaaCr(...)");
    }
    
    
    void
    TVWrapper
    ::SetMeshInfo(const TMeshInfo& theInfo,
		  TErr* theErr)
    {
      TErr aRet;
      SetMeshInfo(theInfo,eECRI,&aRet);
      
      if(aRet < 0)
	SetMeshInfo(theInfo,eREMP,theErr);
    }
    
    
    TInt
    TVWrapper
    ::GetNbFamilies(const TMeshInfo& theInfo,
		    TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      TMeshInfo& anInfo = const_cast<TMeshInfo&>(theInfo);

      return MEDnFam(myFile->Id(),
		     &anInfo.myName[0],
		     0,
		     MED_FAMILLE);
    }
    
    
    TInt
    TVWrapper
    ::GetNbFamAttr(TInt theFamId, 
		   const TMeshInfo& theInfo,
		   TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      TMeshInfo& anInfo = const_cast<TMeshInfo&>(theInfo);

      return MEDnFam(myFile->Id(),
		     &anInfo.myName[0],
		     theFamId,
		     MED_ATTR);
    }
    
    
    TInt
    TVWrapper
    ::GetNbFamGroup(TInt theFamId, 
		    const TMeshInfo& theInfo,
		    TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      TMeshInfo& anInfo = const_cast<TMeshInfo&>(theInfo);

      return MEDnFam(myFile->Id(),
		     &anInfo.myName[0],
		     theFamId,
		     MED_GROUPE);
    }
    
    
    void
    TVWrapper
    ::GetFamilyInfo(TInt theFamId, 
		    TFamilyInfo& theInfo,
		    TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TMeshInfo& aMeshInfo = *theInfo.myMeshInfo;
      
      TErr aRet = MEDfamInfo(myFile->Id(),
			     &aMeshInfo.myName[0],
			     theFamId,
			     &theInfo.myName[0],
			     (med_int*)&theInfo.myId,
			     (med_int*)&theInfo.myAttrId[0],
			     (med_int*)&theInfo.myAttrVal[0],
			     &theInfo.myAttrDesc[0],
			     (med_int*)&theInfo.myNbAttr,
			     &theInfo.myGroupNames[0],
			     (med_int*)&theInfo.myNbGroup);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetFamilyInfo - MEDfamInfo - "<<
		  "&aMeshInfo.myName[0] = '"<<&aMeshInfo.myName[0]<<"'; "<<
		  "theFamId = "<<theFamId<<"; "<<
		  "&theInfo.myName[0] = '"<<&theInfo.myName[0]<<"'; "<<
		  "theInfo.myId = "<<theInfo.myId);
    }
    
    
    void 
    TVWrapper
    ::SetFamilyInfo(const TFamilyInfo& theInfo,
		    EModeAcces theMode,
		    TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,theMode,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TFamilyInfo& anInfo = const_cast<TFamilyInfo&>(theInfo);
      TMeshInfo& aMeshInfo = *anInfo.myMeshInfo;
      
      TErr aRet = MEDfamCr(myFile->Id(),
			   &aMeshInfo.myName[0],
			   &anInfo.myName[0],
			   anInfo.myId,
			   (med_int*)&anInfo.myAttrId[0],
			   (med_int*)&anInfo.myAttrVal[0],
			   &anInfo.myAttrDesc[0],
			   anInfo.myNbAttr,
			   &anInfo.myGroupNames[0],
			   anInfo.myNbGroup);
      
      INITMSG(MYDEBUG && aRet,"TVWrapper::SetFamilyInfo - MED_MODE_ACCES = "<<theMode<<"; aRet = "<<aRet<<endl);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"SetFamilyInfo - MEDfamCr(...)");
    }
    
    
    void 
    TVWrapper
    ::SetFamilyInfo(const TFamilyInfo& theInfo,
		    TErr* theErr)
    {
      TErr aRet;
      SetFamilyInfo(theInfo,eECRI,&aRet);
      
      if(aRet < 0)
	SetFamilyInfo(theInfo,eREMP,theErr);
    }
    
    
    TInt
    TVWrapper
    ::GetNbNodes(const TMeshInfo& theMeshInfo,
		 TErr* theErr)
    {
      MSG(MYDEBUG,"TVWrapper::GetNbNodes");
      INITMSG(MYDEBUG,"GetNbNodes ... ");
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      TMeshInfo& aMeshInfo = const_cast<TMeshInfo&>(theMeshInfo);
      
      TInt aRet = MEDnEntMaa(myFile->Id(),
			     &aMeshInfo.myName[0],
			     MED_COOR,
			     MED_NOEUD,
			     med_geometrie_element(0),
			     med_connectivite(0));
      
      ADDMSG(MYDEBUG," nbnodes="<<aRet<<" ... OK"<<endl);
      return aRet;
    }
    
    
    void 
    TVWrapper
    ::GetNodeInfo(TNodeInfo& theInfo,
		  TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0 || theInfo.myNbElem<=0)
	return;
      
      TMeshInfo& aMeshInfo = *theInfo.myMeshInfo;

      TErr aRet = MEDnoeudsLire(myFile->Id(),
				&aMeshInfo.myName[0],
				aMeshInfo.myDim,
				&theInfo.myCoord[0],
				MED_FULL_INTERLACE,
				(med_repere*)&theInfo.mySystem,
				&theInfo.myCoordNames[0],
				&theInfo.myCoordUnits[0],
				&theInfo.myElemNames[0],
				(med_booleen*)&theInfo.myIsElemNames,
				(med_int*)&theInfo.myElemNum[0],
				(med_booleen*)&theInfo.myIsElemNum,
				(med_int*)&theInfo.myFamNum[0],
				theInfo.myNbElem);

      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetNodeInfo - MEDnoeudsLire(...)");
    }
    
    
    void 
    TVWrapper
    ::SetNodeInfo(const MED::TNodeInfo& theInfo,
		  EModeAcces theMode,
		  TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,theMode,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      MED::TNodeInfo& anInfo = const_cast<MED::TNodeInfo&>(theInfo);
      MED::TMeshInfo& aMeshInfo = *anInfo.myMeshInfo;
      
      TErr aRet = MEDnoeudsEcr(myFile->Id(),
			       &aMeshInfo.myName[0],
			       aMeshInfo.myDim,
			       &anInfo.myCoord[0],
			       MED_FULL_INTERLACE,
			       med_repere(theInfo.mySystem),
			       &anInfo.myCoordNames[0],
			       &anInfo.myCoordUnits[0],
			       &anInfo.myElemNames[0],
			       med_booleen(theInfo.myIsElemNames),
			       (med_int*)&anInfo.myElemNum[0],
			       med_booleen(theInfo.myIsElemNum),
			       (med_int*)&anInfo.myFamNum[0],
			       anInfo.myNbElem,
			       MED_REMP);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"SetNodeInfo - MEDnoeudsEcr(...)");
    }
    
    
    void 
    TVWrapper
    ::SetNodeInfo(const MED::TNodeInfo& theInfo,
		  TErr* theErr)
    {
      TErr aRet;
      SetNodeInfo(theInfo,eECRI,&aRet);
      
      if(aRet < 0)
	SetNodeInfo(theInfo,eREMP,theErr);
    }
    
    
    TEntityInfo
    TVWrapper
    ::GetEntityInfo(const TMeshInfo& theMeshInfo,
		    EConnectivite theConnMode,
		    TErr* theErr)
    {
      TEntityInfo anInfo;
      
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
	return anInfo;
      
      TInt aNbElem = GetNbNodes(theMeshInfo);
      if(aNbElem > 0){
	anInfo[eNOEUD][ePOINT1] = aNbElem;
	const TEntity2GeomSet& anEntity2GeomSet = GetEntity2GeomSet();
	TEntity2GeomSet::const_iterator anIter = anEntity2GeomSet.begin();
	TEntity2GeomSet::const_iterator anIterEnd = anEntity2GeomSet.end();
	for(; anIter != anIterEnd; anIter++){
	  const EEntiteMaillage& anEntity = anIter->first;
	  const TGeomSet& aGeomSet = anIter->second;
	  TGeomSet::const_iterator anIter2 = aGeomSet.begin();
	  TGeomSet::const_iterator anIterEnd2 = aGeomSet.end();
	  for(; anIter2 != anIterEnd2; anIter2++){
	    const EGeometrieElement& aGeom = *anIter2;
	    aNbElem = GetNbCells(theMeshInfo,anEntity,aGeom,theConnMode,theErr);
	    if(aNbElem > 0)
	      anInfo[anEntity][aGeom] = aNbElem;
	  }
	}
      }
      return anInfo;
    }
    
    
    TInt
    TVWrapper
    ::GetNbCells(const MED::TMeshInfo& theMeshInfo, 
		 EEntiteMaillage theEntity, 
		 EGeometrieElement theGeom, 
		 EConnectivite theConnMode,
		 TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      MED::TMeshInfo& aMeshInfo = const_cast<MED::TMeshInfo&>(theMeshInfo);
      
      return MEDnEntMaa(myFile->Id(),
			&aMeshInfo.myName[0],
			MED_CONN,
			med_entite_maillage(theEntity),
			med_geometrie_element(theGeom),
			med_connectivite(theConnMode)); 
    }
    
    
    void 
    TVWrapper
    ::GetCellInfo(MED::TCellInfo& theInfo,
		  TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);

      if(theErr && *theErr < 0)
	return;
      
      MED::TMeshInfo& aMeshInfo = *theInfo.myMeshInfo;
      TInt aNbElem = theInfo.myElemNum.size();

      TErr aRet;
      aRet = MEDelementsLire(myFile->Id(),
			     &aMeshInfo.myName[0],
			     aMeshInfo.myDim,
			     (med_int*)&theInfo.myConn[0],
			     MED_FULL_INTERLACE,
			     &theInfo.myElemNames[0],
			     (med_booleen*)&theInfo.myIsElemNames,
			     (med_int*)&theInfo.myElemNum[0],
			     (med_booleen*)&theInfo.myIsElemNum,
			     (med_int*)&theInfo.myFamNum[0],
			     aNbElem,
			     med_entite_maillage(theInfo.myEntity),
			     med_geometrie_element(theInfo.myGeom),
			     med_connectivite(theInfo.myConnMode));

      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetCellInfo - MEDelementsLire(...)");
    }
    
    
    void 
    TVWrapper
    ::SetCellInfo(const MED::TCellInfo& theInfo,
		  EModeAcces theMode,
		  TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,theMode,theErr);
      
      if(theErr && *theErr < 0)
	return;

      MED::TCellInfo& anInfo = const_cast<MED::TCellInfo&>(theInfo);
      MED::TMeshInfo& aMeshInfo = *anInfo.myMeshInfo;

      TErr aRet;
      aRet = MEDelementsEcr(myFile->Id(),
			    &aMeshInfo.myName[0],
			    aMeshInfo.myDim,
			    (med_int*)&anInfo.myConn[0],
			    MED_FULL_INTERLACE,
			    &anInfo.myElemNames[0],
			    med_booleen(theInfo.myIsElemNames),
			    (med_int*)&anInfo.myElemNum[0],
			    med_booleen(theInfo.myIsElemNum),
			    (med_int*)&anInfo.myFamNum[0],
			    anInfo.myNbElem,
			    med_entite_maillage(theInfo.myEntity),
			    med_geometrie_element(theInfo.myGeom),
			    med_connectivite(theInfo.myConnMode),
			    MED_REMP);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetCellInfo - MEDelementsLire(...)");
    }
    

    void
    TVWrapper
    ::SetCellInfo(const MED::TCellInfo& theInfo,
		  TErr* theErr)
    {
      TErr aRet;
      SetCellInfo(theInfo,eECRI,&aRet);
      
      if(aRet < 0)
	SetCellInfo(theInfo,eREMP,theErr);
    }
    

    TInt
    TVWrapper
    ::GetNbFields(TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      return MEDnChamp(myFile->Id(),0);
    }
    
    
    TInt
    TVWrapper
    ::GetNbComp(TInt theFieldId,
		TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      return MEDnChamp(myFile->Id(),theFieldId);
    }
    
    
    void 
    TVWrapper
    ::GetFieldInfo(TInt theFieldId, 
		   MED::TFieldInfo& theInfo,
		   TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TErr aRet;
      aRet = MEDchampInfo(myFile->Id(),
			  theFieldId,
			  &theInfo.myName[0],
			  (med_type_champ*)&theInfo.myType,
			  &theInfo.myCompNames[0],
			  &theInfo.myUnitNames[0],
			  theInfo.myNbComp);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetFieldInfo - MEDchampInfo(...)");
    }
    
    
    void
    TVWrapper
    ::SetFieldInfo(const MED::TFieldInfo& theInfo,
		   EModeAcces theMode,
		   TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,theMode,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      MED::TFieldInfo& anInfo = const_cast<MED::TFieldInfo&>(theInfo);
      
      TErr aRet;
      aRet = MEDchampCr(myFile->Id(),
			&anInfo.myName[0],
			med_type_champ(theInfo.myType),
			&anInfo.myCompNames[0],
			&anInfo.myUnitNames[0],
			anInfo.myNbComp);

      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"SetFieldInfo - MEDchampCr(...)");
    }
    
    
    void
    TVWrapper
    ::SetFieldInfo(const MED::TFieldInfo& theInfo,
		   TErr* theErr)
    {
      try{

	TErr aRet;
	SetFieldInfo(theInfo,eECRI,&aRet);
      
	if(aRet < 0)
	  SetFieldInfo(theInfo,eREMP,theErr);

      }catch(const std::exception& theExc){
	EXCEPTION(runtime_error,"SetFieldInfo(...)"<<endl<<
		  theExc.what());
      }catch(...){
	throw;
      }
    }
    
    //-----------------------------------------------------------------
    TInt
    TVWrapper
    ::GetNbProfiles(TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      return MEDnProfil(myFile->Id());
    }


    TProfileInfo::TInfo
    TVWrapper
    ::GetProfilePreInfo(TInt theId, 
			TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);

      if(theErr && *theErr < 0)
	return TProfileInfo::TInfo("",-1);
      
      TInt aSize = -1;
      std::vector<char> aName(NOM+1);

      TErr aRet;
      aRet = MEDprofilInfo(myFile->Id(),
			   theId,
			   &aName[0],
			   &aSize);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetProfilePreInfo - MEDprofilInfo(...)");
      
      return TProfileInfo::TInfo(&aName[0],aSize);
    }


    void
    TVWrapper
    ::GetProfileInfo(TInt theId, 
		     TProfileInfo& theInfo,
		     TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TErr aRet;
      aRet = MEDprofilLire(myFile->Id(),
			   &theInfo.myElemNum[0],
			   &theInfo.myName[0]);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"SetNodeInfo - MEDprofilLire(...)");
    }


    //-----------------------------------------------------------------
    TInt
    TVWrapper
    ::GetNbTimeStamps(const MED::TFieldInfo& theInfo, 
		      const MED::TEntityInfo& theEntityInfo,
		      EEntiteMaillage& theEntity,
		      TGeom2Size& theGeom2Size,
		      TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr){
	if(theEntityInfo.empty())
	  *theErr = -1;
	if(*theErr < 0)
	  return -1;
      }else if(theEntityInfo.empty()) 
	EXCEPTION(runtime_error,"GetNbTimeStamps - There is no any Entity on the Mesh");
      
      theGeom2Size.clear();
      TInt aNbTimeStamps = 0;
      MED::TFieldInfo& anInfo = const_cast<MED::TFieldInfo&>(theInfo);
      TEntityInfo::const_iterator anIter = theEntityInfo.begin();
      for(; anIter != theEntityInfo.end(); anIter++){
	med_entite_maillage anEntity = med_entite_maillage(anIter->first);
	const TGeom2Size& aGeom2Size = anIter->second;
	TGeom2Size::const_iterator anGeomIter = aGeom2Size.begin();
	for(; anGeomIter != aGeom2Size.end(); anGeomIter++){
	  med_geometrie_element aGeom = med_geometrie_element(anGeomIter->first);
	  TInt aTmp = MEDnPasdetemps(myFile->Id(),&anInfo.myName[0],anEntity,aGeom);
	  aNbTimeStamps = max(aTmp,aNbTimeStamps);
	  if(aNbTimeStamps < 1)
	    continue;
	  INITMSG(MYDEBUG,"GetNbTimeStamps aNbTimeStamps = "<<aTmp<<"; aGeom = "<<aGeom<<"; anEntity = "<<anEntity<<"\n");
	  if(aTmp){
	    theEntity = EEntiteMaillage(anEntity);
	    theGeom2Size[EGeometrieElement(aGeom)] = anGeomIter->second;
	  }
	}
	if(!theGeom2Size.empty()) 
	  break;
      }
      return aNbTimeStamps;
    }
    
    
    void
    TVWrapper
    ::GetTimeStampInfo(TInt theTimeStampId, 
		       MED::TTimeStampInfo& theInfo,
		       TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      TGeom2Size& aGeom2Size = theInfo.myGeom2Size;
      
      if(theErr){
	if(aGeom2Size.empty())
	  *theErr = -1;
	if(*theErr < 0)
	  return;
      }else if(aGeom2Size.empty())
	EXCEPTION(runtime_error,"GetTimeStampInfo - There is no any cell");
      
      MED::TFieldInfo& aFieldInfo = *theInfo.myFieldInfo;
      MED::TMeshInfo& aMeshInfo = *aFieldInfo.myMeshInfo;
      
      TGeom2Size::iterator anIter = aGeom2Size.begin();
      
      TErr aRet;
      aRet = MEDpasdetempsInfo(myFile->Id(),
			       &aFieldInfo.myName[0],
			       med_entite_maillage(theInfo.myEntity),
			       med_geometrie_element(anIter->first),
			       theTimeStampId,
			       &aMeshInfo.myName[0],
			       (med_int*)&theInfo.myNbGauss,
			       (med_int*)&theInfo.myNumDt,
			       &theInfo.myUnitDt[0],
			       &theInfo.myDt,
			       (med_int*)&theInfo.myNumOrd);

      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetTimeStampInfo - MEDpasdetempsInfo(...)");
      
      static TInt MAX_NB_GAUSS_POINTS = 32;
      if(theInfo.myNbGauss <= 0 || theInfo.myNbGauss > MAX_NB_GAUSS_POINTS)
	theInfo.myNbGauss = 1;
    }
    

    void 
    TVWrapper
    ::GetTimeStampVal(TTimeStampVal& theVal,
		      const TMKey2Profile& theMKey2Profile,
		      const TKey2Gauss& theKey2Gauss,
		      TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TIdt anId = myFile->Id();
      
      MED::TTimeStampInfo& aTimeStampInfo = *theVal.myTimeStampInfo;
      MED::TFieldInfo& aFieldInfo = *aTimeStampInfo.myFieldInfo;
      MED::TMeshInfo& aMeshInfo = *aFieldInfo.myMeshInfo;
      
      MED::TKey2Profile aKey2Profile = boost::get<1>(theMKey2Profile);
      char aProfileName[NOM+1] = "";

      TGeom2Size& aGeom2Size = aTimeStampInfo.myGeom2Size;
      TGeom2Size::iterator anIter = aGeom2Size.begin();
      for(; anIter != aGeom2Size.end(); anIter++){
	EGeometrieElement aGeom = anIter->first;
	TInt aNbVal = MEDnVal(anId,
			      &aFieldInfo.myName[0],
			      med_entite_maillage(aTimeStampInfo.myEntity),
			      med_geometrie_element(aGeom),
			      aTimeStampInfo.myNumDt,
			      aTimeStampInfo.myNumOrd);
	if(aNbVal <= 0){
	  if(theErr){
	    *theErr = -1;
	    return;
	  }
	  EXCEPTION(runtime_error,"GetTimeStampInfo - MEDnVal(...) - aNbVal == "<<aNbVal<<" <= 0");
	}
	
	TValue& aValue = theVal.myMeshValue[aGeom];
	TInt iEnd = aNbVal*aFieldInfo.myNbComp;
	aValue.resize(iEnd);
	
	TErr aRet;
	switch(aFieldInfo.myType){
	case eFLOAT64: {
	  std::vector<TFloat> anArray(iEnd);
	  aRet = MEDchampLire(anId,
			      &aMeshInfo.myName[0],
			      &aFieldInfo.myName[0],
			      (unsigned char*)&anArray[0],
			      MED_FULL_INTERLACE,
			      MED_ALL,
			      aProfileName,
			      med_entite_maillage(aTimeStampInfo.myEntity),
			      med_geometrie_element(aGeom),
			      aTimeStampInfo.myNumDt,
			      aTimeStampInfo.myNumOrd);
	  if(aRet >= 0) 
	    for(TInt i = 0; i < iEnd; i++) 
	      aValue[i] = anArray[i];
	  break;
	}
	default: {
	  std::vector<TInt> anArray(iEnd);
	  aRet = MEDchampLire(anId,
			      &aMeshInfo.myName[0],
			      &aFieldInfo.myName[0],
			      (unsigned char*)&anArray[0],
			      MED_FULL_INTERLACE,
			      MED_ALL,
			      aProfileName,
			      med_entite_maillage(aTimeStampInfo.myEntity),
			      med_geometrie_element(aGeom),
			      aTimeStampInfo.myNumDt,
			      aTimeStampInfo.myNumOrd);
	  if(aRet >= 0) 
	    for(med_int i = 0; i < iEnd; i++) 
	      aValue[i] = anArray[i];
	  break;
	}}

	if(aRet < 0){
	  if(theErr){
	    *theErr = aRet;
	    return;
	  }
	  EXCEPTION(runtime_error,"GetValTimeStamp - MEDchampLire(...)");
	}

	MED::PProfileInfo aProfileInfo;
	if(strcmp(aProfileName,"") != 0){
	  MED::TKey2Profile::const_iterator anIter = aKey2Profile.find(aProfileName);
	  if(anIter != aKey2Profile.end()){
	    aProfileInfo = anIter->second;
	    theVal.myGeom2Profile[aGeom] = aProfileInfo;
	  }
	}
	  
	if(aProfileInfo && aProfileInfo->IsPresent()){
	  TInt aSize = aProfileInfo->GetSize()*aFieldInfo.myNbComp*aTimeStampInfo.myNbGauss;
	  if(aSize != aValue.size()){
	    if(theErr){
	      *theErr = -1;
	      return;
	    }
	    EXCEPTION(runtime_error,
		      "GetTimeStampInfo - aSize("<<aSize<<
		      ") != aValue.size()("<<aValue.size()<<
		      "); aNbVal = "<<aNbVal<<
		      "; anEntity = "<<aTimeStampInfo.myEntity<<
		      "; aGeom = "<<aGeom);
	  }else{
	    if(iEnd != aValue.size()){
	      if(theErr){
		*theErr = -1;
		return;
	      }
	      EXCEPTION(runtime_error,
			"GetTimeStampInfo - iEnd("<<iEnd<<
			") != aValue.size()("<<aValue.size()<<
			"); aNbVal = "<<aNbVal<<
			"; anEntity = "<<aTimeStampInfo.myEntity<<
			"; aGeom = "<<aGeom);
	    }
	  }
	}
      }
    }
    
    
    void
    TVWrapper
    ::SetTimeStamp(const MED::TTimeStampVal& theVal,
		   EModeAcces theMode,
		   TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,theMode,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TErr aRet;
      TIdt anId = myFile->Id();
      
      MED::TTimeStampVal& aVal = const_cast<MED::TTimeStampVal&>(theVal);
      MED::TGeom2Profile& aGeom2Profile = aVal.myGeom2Profile;
      MED::TTimeStampInfo& aTimeStampInfo = *aVal.myTimeStampInfo;
      MED::TFieldInfo& aFieldInfo = *aTimeStampInfo.myFieldInfo;
      MED::TMeshInfo& aMeshInfo = *aFieldInfo.myMeshInfo;
      MED::TMeshValue& aMeshValue = aVal.myMeshValue;
      
      med_entite_maillage& anEntity = (med_entite_maillage&)(aTimeStampInfo.myEntity);
      TMeshValue::iterator anIter = aMeshValue.begin();
      for(; anIter != aMeshValue.end(); anIter++){
	EGeometrieElement aGeom = anIter->first;
	med_geometrie_element aMEDGeom = med_geometrie_element(aGeom);

	MED::TProfileInfo& aProfileInfo = aGeom2Profile[aGeom];

	TValue& aValue = aVal.myMeshValue[aGeom];
	med_int iEnd = aValue.size();
	med_int aNbVal = iEnd / aFieldInfo.myNbComp;
	
	switch(aFieldInfo.myType){
	case eFLOAT64: {
	  std::vector<TFloat>& anArray = aValue;
	  
	  aRet = MEDchampEcr(anId,
			     &aMeshInfo.myName[0],
			     &aFieldInfo.myName[0],
			     (unsigned char*)&anArray[0],
			     MED_FULL_INTERLACE,
			     aNbVal,
			     aTimeStampInfo.myNbGauss,
			     MED_ALL,
			     &aProfileInfo.myName[0],
			     MED_ECRI, 
			     anEntity,
			     aMEDGeom,
			     aTimeStampInfo.myNumDt,
			     &aTimeStampInfo.myUnitDt[0],
			     aTimeStampInfo.myDt,
			     aTimeStampInfo.myNumOrd);
	  break;
	}
	default: {
	  vector<TInt> anArray(iEnd);
	  for(TInt i = 0; i< iEnd; i++) anArray[i] = TInt(aValue[i]);
	  
	  aRet = MEDchampEcr(anId,
			     &aMeshInfo.myName[0],
			     &aFieldInfo.myName[0],
			     (unsigned char*)&anArray[0],
			     MED_FULL_INTERLACE,
			     aNbVal,
			     aTimeStampInfo.myNbGauss,
			     MED_ALL,
			     &aProfileInfo.myName[0],
			     MED_ECRI, 
			     anEntity,
			     aMEDGeom,
			     aTimeStampInfo.myNumDt,
			     &aTimeStampInfo.myUnitDt[0],
			     aTimeStampInfo.myDt,
			     aTimeStampInfo.myNumOrd);
	  break;
	}}
	
	if(aRet < 0){
	  if(theErr){
	    *theErr = aRet;
	    break;
	  }
	  EXCEPTION(runtime_error,"SetTimeStamp - MEDchampEcr(...)");
	}
	
      }
      
      INITMSG(MYDEBUG,"TVWrapper::SetMeshInfo - MED_MODE_ACCES = "<<theMode<<"; aRet = "<<aRet<<endl);
    }

    
    void TVWrapper::SetTimeStamp(const MED::TTimeStampVal& theVal,
				TErr* theErr)
    {
      TErr aRet;
      SetTimeStamp(theVal,eECRI,&aRet);
      
      if(aRet < 0)
	SetTimeStamp(theVal,eREMP,theErr);
    }
    
  }
}
