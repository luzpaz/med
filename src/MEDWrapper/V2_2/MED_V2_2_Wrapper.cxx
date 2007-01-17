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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : 
//  Author : 
//  Module : 
//  $Header$

#include "MED_V2_2_Wrapper.hxx"
#include "MED_Algorithm.hxx"
#include "MED_Utilities.hxx"

extern "C"
{
#include <med.h>

med_err
MEDgaussInfo(med_idt fid, int indice, char * locname, med_geometrie_element * type_geo,
	     med_int * ngauss );
}

#ifdef _DEBUG_
static int MYDEBUG = 0;
#else
static int MYDEBUG = 0;
#endif



namespace MED
{
  template<>
  TInt
  GetDESCLength<eV2_2>()
  {
    return 200;
  }

  template<>
  TInt
  GetIDENTLength<eV2_2>()
  {
    return 8;
  }

  template<>
  TInt
  GetNOMLength<eV2_2>()
  {
    return 32;
  }

  template<>
  TInt
  GetLNOMLength<eV2_2>()
  {
    return 80;
  }

  template<>
  TInt
  GetPNOMLength<eV2_2>()
  {
    return 16;
  }

  template<>
  TInt
  GetNbConn<eV2_2>(EGeometrieElement typmai,
		   EEntiteMaillage typent,
		   TInt mdim)
  {
    return typmai%100;
  }

  namespace V2_2
  {

    //---------------------------------------------------------------
    class TFile{
      TFile();
      TFile(const TFile&);
      
    public:
      TFile(const std::string& theFileName): 
	myFid(0), 
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
	if(theErr)
	  *theErr = TErr(myFid);
	else if(myFid < 0)
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
    TVWrapper::TVWrapper(const std::string& theFileName): 
      myFile(new TFile(theFileName))
    {
    }
    
    
    TInt
    TVWrapper
    ::GetNbMeshes(TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      return MEDnMaa(myFile->Id());
    }
    
    
    void
    TVWrapper
    ::GetMeshInfo(TInt theMeshId, 
		  MED::TMeshInfo& theInfo,
		  TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TErr aRet = MEDmaaInfo(myFile->Id(),
			     theMeshId,
			     &theInfo.myName[0],
			     (med_int*)&theInfo.myDim,
			     (med_maillage*)&theInfo.myType,
			     &theInfo.myDesc[0]);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetMeshInfo - MEDmaaInfo(...)");
    }
    
    
    void
    TVWrapper
    ::SetMeshInfo(const MED::TMeshInfo& theInfo,
		  EModeAcces theMode,
		  TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,theMode,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      MED::TMeshInfo& anInfo = const_cast<MED::TMeshInfo&>(theInfo);
      
      TErr aRet = MEDmaaCr(myFile->Id(),
			   &anInfo.myName[0],
			   med_int(anInfo.myDim),
			   med_maillage(theInfo.myType),
			   &anInfo.myDesc[0]);
      if(aRet == 0){
	aRet = MEDunvCr(myFile->Id(),
			&anInfo.myName[0]);
      }
      
      INITMSG(MYDEBUG,"TVWrapper::SetMeshInfo - MED_MODE_ACCES = "<<theMode<<"; aRet = "<<aRet<<endl);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"SetMeshInfo - MEDmaaCr(...)");
    }
    
    
    void 
    TVWrapper
    ::SetMeshInfo(const MED::TMeshInfo& theInfo,
		  TErr* theErr)
    {
      TErr aRet;
      SetMeshInfo(theInfo,eLECTURE_ECRITURE,&aRet);
      
      if(aRet < 0)
	SetMeshInfo(theInfo,eLECTURE_AJOUT,&aRet);

      if(aRet < 0)
	SetMeshInfo(theInfo,eCREATION,&aRet);

      if(theErr)
        *theErr = aRet;
    }
    
    
    TInt
    TVWrapper
    ::GetNbFamilies(const MED::TMeshInfo& theInfo,
		    TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      MED::TMeshInfo& anInfo = const_cast<MED::TMeshInfo&>(theInfo);
      return MEDnFam(myFile->Id(),
		     &anInfo.myName[0]);
    }
    
    
    TInt
    TVWrapper
    ::GetNbFamAttr(TInt theFamId, 
		   const MED::TMeshInfo& theInfo,
		   TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      MED::TMeshInfo& anInfo = const_cast<MED::TMeshInfo&>(theInfo);
      return MEDnAttribut(myFile->Id(),
			  &anInfo.myName[0],
			  theFamId);
    }
    
    
    TInt
    TVWrapper
    ::GetNbFamGroup(TInt theFamId, 
		    const MED::TMeshInfo& theInfo,
		    TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      MED::TMeshInfo& anInfo = const_cast<MED::TMeshInfo&>(theInfo);
      return MEDnGroupe(myFile->Id(),
			&anInfo.myName[0],
			theFamId);
    }
    
    
    void
    TVWrapper
    ::GetFamilyInfo(TInt theFamId, 
		    MED::TFamilyInfo& theInfo,
		    TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      MED::TMeshInfo& aMeshInfo = *theInfo.myMeshInfo;
      
      med_int* anAttrId = theInfo.myNbAttr > 0? (med_int*)&theInfo.myAttrId[0]: NULL;
      med_int* anAttrVal = theInfo.myNbAttr > 0? (med_int*)&theInfo.myAttrVal[0]: NULL;
      char* anAttrDesc = theInfo.myNbAttr > 0? &theInfo.myAttrDesc[0]: NULL;
      char* aGroupNames = theInfo.myNbGroup > 0? &theInfo.myGroupNames[0]: NULL;

      TErr aRet = MEDfamInfo(myFile->Id(),
			     &aMeshInfo.myName[0],
			     theFamId,
			     &theInfo.myName[0],
			     (med_int*)&theInfo.myId,
			     anAttrId,
			     anAttrVal,
			     anAttrDesc,
			     (med_int*)&theInfo.myNbAttr,
			     aGroupNames,
			     (med_int*)&theInfo.myNbGroup);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetFamilyInfo - MEDfamInfo(...) - "<<
		  " aMeshInfo.myName = '"<<&aMeshInfo.myName[0]<<
		  "'; theFamId = "<<theFamId<<
		  "; theInfo.myNbGroup = "<<theInfo.myNbGroup<<
		  "; theInfo.myNbAttr = "<<theInfo.myNbAttr);
    }
    
    
    void
    TVWrapper
    ::SetFamilyInfo(const MED::TFamilyInfo& theInfo,
		    EModeAcces theMode,
		    TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,theMode,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      MED::TFamilyInfo& anInfo = const_cast<MED::TFamilyInfo&>(theInfo);
      MED::TMeshInfo& aMeshInfo = *anInfo.myMeshInfo;
      
      med_int* anAttrId = anInfo.myNbAttr > 0? (med_int*)&anInfo.myAttrId[0]: NULL;
      med_int* anAttrVal = anInfo.myNbAttr > 0? (med_int*)&anInfo.myAttrVal[0]: NULL;
      char* anAttrDesc = anInfo.myNbAttr > 0? &anInfo.myAttrDesc[0]: NULL;
      char* aGroupNames = anInfo.myNbGroup > 0? &anInfo.myGroupNames[0]: NULL;

      TErr aRet = MEDfamCr(myFile->Id(),
			   &aMeshInfo.myName[0],
			   &anInfo.myName[0],
			   anInfo.myId,
			   anAttrId,
			   anAttrVal,
			   anAttrDesc,
			   anInfo.myNbAttr,
			   aGroupNames,
			   anInfo.myNbGroup);
      
      INITMSG(MYDEBUG,"TVWrapper::GetFamilyInfo - MED_MODE_ACCES = "<<theMode<<"; aRet = "<<aRet<<endl);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"SetFamilyInfo - MEDfamCr(...)");
    }
    
    
    void
    TVWrapper
    ::SetFamilyInfo(const MED::TFamilyInfo& theInfo,
		    TErr* theErr)
    {
      TErr aRet;
      SetFamilyInfo(theInfo,eLECTURE_ECRITURE,&aRet);
      
      if(aRet < 0)
	SetFamilyInfo(theInfo,eLECTURE_AJOUT,&aRet);

      if(theErr)
        *theErr = aRet;
    }
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    void
    TVWrapper
    ::GetNames(TElemInfo& theInfo,
	       TInt theNb,
	       EEntiteMaillage theEntity, 
	       EGeometrieElement theGeom,
	       TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      MED::TMeshInfo& aMeshInfo = *theInfo.myMeshInfo;

      TErr aRet = MEDnomLire(myFile->Id(),
			     &aMeshInfo.myName[0],
			     &theInfo.myElemNames[0],
			     theNb,
			     med_entite_maillage(theEntity),
			     med_geometrie_element(theGeom));

      theInfo.myIsElemNames = aRet != 0? eFAUX : eVRAI ;

      if(theErr)
        *theErr = aRet;
    }

    void
    TVWrapper
    ::GetNumeration(TElemInfo& theInfo,
		    TInt theNb,
		    EEntiteMaillage theEntity, 
		    EGeometrieElement theGeom,
		    TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      MED::TMeshInfo& aMeshInfo = *theInfo.myMeshInfo;
      
      med_entite_maillage& anEntity = (med_entite_maillage&)(theEntity);
      med_geometrie_element& aGeom = (med_geometrie_element&)(theGeom);

      TErr aRet = MEDnumLire(myFile->Id(),
			     &aMeshInfo.myName[0],
			     (med_int*)&theInfo.myElemNum[0],
			     theNb,
			     anEntity,
			     aGeom);

      theInfo.myIsElemNum = aRet != 0? eFAUX : eVRAI;

      if(theErr)
        *theErr = aRet;
    }

    void
    TVWrapper
    ::GetFamilies(TElemInfo& theInfo,
		  TInt theNb,
		  EEntiteMaillage theEntity, 
		  EGeometrieElement theGeom,
		  TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      MED::TMeshInfo& aMeshInfo = *theInfo.myMeshInfo;
      TErr aRet = MEDfamLire(myFile->Id(),
			     &aMeshInfo.myName[0],
			     (med_int*)&theInfo.myFamNum[0],
			     theNb,
			     med_entite_maillage(theEntity),
			     med_geometrie_element(theGeom));

      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetFamilies - MEDfamLire(...)");
    }

    void
    TVWrapper
    ::SetNames(const TElemInfo& theInfo,
	       EEntiteMaillage theEntity, 
	       EGeometrieElement theGeom,
	       TErr* theErr)
    { 
      SetNames(theInfo,eLECTURE_ECRITURE,theEntity,theGeom,theErr);
    }

    void
    TVWrapper
    ::SetNames(const TElemInfo& theInfo,
	       EModeAcces theMode,
	       EEntiteMaillage theEntity, 
	       EGeometrieElement theGeom,
	       TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,theMode,theErr);
      
      if(theErr && *theErr < 0)
	return;

      MED::TElemInfo& anInfo = const_cast<MED::TElemInfo&>(theInfo);
      MED::TMeshInfo& aMeshInfo = *anInfo.myMeshInfo;

      TErr aRet = 0;
      if(theInfo.myIsElemNames){
	aRet  = MEDnomEcr(myFile->Id(),
			  &aMeshInfo.myName[0],
			  &anInfo.myElemNames[0],
			  anInfo.myElemNames.size(),
			  med_entite_maillage(theEntity),
			  med_geometrie_element(theGeom));
	if(theErr) 
	  *theErr = aRet;
	else if(aRet < 0)
	  EXCEPTION(runtime_error,"SetNames - MEDnomEcr(...)");
      }
    }

    void
    TVWrapper
    ::SetNumeration(const TElemInfo& theInfo,
		    EEntiteMaillage theEntity, 
		    EGeometrieElement theGeom,
		    TErr* theErr)
    { 
      SetNumeration(theInfo,eLECTURE_ECRITURE,theEntity,theGeom,theErr);
    }

    void 
    TVWrapper
    ::SetNumeration(const TElemInfo& theInfo,
		    EModeAcces theMode,
		    EEntiteMaillage theEntity, 
		    EGeometrieElement theGeom,
		    TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,theMode,theErr);
      
      if(theErr && *theErr < 0)
	return;

      MED::TElemInfo& anInfo = const_cast<MED::TElemInfo&>(theInfo);
      MED::TMeshInfo& aMeshInfo = *anInfo.myMeshInfo;

      TErr aRet = 0;
      if(theInfo.myIsElemNum){
	aRet  = MEDnumEcr(myFile->Id(),
			  &aMeshInfo.myName[0],
			  (med_int*)&anInfo.myElemNum[0],
			  anInfo.myElemNum.size(),
			  med_entite_maillage(theEntity),
			  med_geometrie_element(theGeom));
	if(theErr) 
	  *theErr = aRet;
	else if(aRet < 0)
	  EXCEPTION(runtime_error,"SetNumeration - MEDnumEcr(...)");
      }
    }

    void
    TVWrapper
    ::SetFamilies(const TElemInfo& theInfo,
		  EEntiteMaillage theEntity, 
		  EGeometrieElement theGeom,
		  TErr* theErr)
    { 
      SetFamilies(theInfo,eLECTURE_ECRITURE,theEntity,theGeom,theErr);
    }

    void 
    TVWrapper
    ::SetFamilies(const TElemInfo& theInfo,
		  EModeAcces theMode,
		  EEntiteMaillage theEntity, 
		  EGeometrieElement theGeom,
		  TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,theMode,theErr);
      
      if(theErr && *theErr < 0)
	return;

      MED::TElemInfo& anInfo = const_cast<MED::TElemInfo&>(theInfo);
      MED::TMeshInfo& aMeshInfo = *anInfo.myMeshInfo;

      TErr aRet = MEDfamEcr(myFile->Id(),
			    &aMeshInfo.myName[0],
			    (med_int*)&anInfo.myFamNum[0],
			    anInfo.myFamNum.size(),
			    med_entite_maillage(theEntity),
			    med_geometrie_element(theGeom));
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"SetFamilies - MEDfamEcr(...)");
    }
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    TInt
    TVWrapper
    ::GetNbNodes(const MED::TMeshInfo& theMeshInfo,
		 ETable theTable,
		 TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      MED::TMeshInfo& aMeshInfo = const_cast<MED::TMeshInfo&>(theMeshInfo);
      
      return MEDnEntMaa(myFile->Id(),
			&aMeshInfo.myName[0],
			(med_table)theTable,
			MED_NOEUD,
			med_geometrie_element(0),
			med_connectivite(0));
    }
    
    
    void
    TVWrapper
    ::GetNodeInfo(MED::TNodeInfo& theInfo,
		  TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      MED::TMeshInfo& aMeshInfo = *theInfo.myMeshInfo;

      char* anElemNames = theInfo.myIsElemNames? &theInfo.myElemNames[0]: NULL;
      med_int* anElemNum = theInfo.myIsElemNum? &theInfo.myElemNum[0]: NULL;

      TErr aRet = MEDnoeudsLire(myFile->Id(),
				&aMeshInfo.myName[0],
				aMeshInfo.myDim,
				&theInfo.myCoord[0],
				med_mode_switch(theInfo.myModeSwitch),
				(med_repere*)&theInfo.mySystem,
				&theInfo.myCoordNames[0],
				&theInfo.myCoordUnits[0],
				anElemNames,
				(med_booleen*)&theInfo.myIsElemNames,
				anElemNum,
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
      
      char* anElemNames = theInfo.myIsElemNames? &anInfo.myElemNames[0]: NULL;
      med_int* anElemNum = theInfo.myIsElemNum? &anInfo.myElemNum[0]: NULL;

      TErr aRet = MEDnoeudsEcr(myFile->Id(),
			       &aMeshInfo.myName[0],
			       aMeshInfo.myDim,
			       &anInfo.myCoord[0],
			       med_mode_switch(theInfo.myModeSwitch),
			       med_repere(theInfo.mySystem),
			       &anInfo.myCoordNames[0],
			       &anInfo.myCoordUnits[0],
			       anElemNames,
			       med_booleen(theInfo.myIsElemNames),
			       anElemNum,
			       med_booleen(theInfo.myIsElemNum),
			       (med_int*)&anInfo.myFamNum[0],
			       anInfo.myNbElem);
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
      SetNodeInfo(theInfo,eLECTURE_ECRITURE,&aRet);
      
      if(aRet < 0)
	SetNodeInfo(theInfo,eLECTURE_AJOUT,&aRet);

      if(theErr) 
	*theErr = aRet;
    }
    

    //-----------------------------------------------------------------
    void
    TVWrapper
    ::GetPolygoneInfo(MED::TPolygoneInfo& theInfo,
		      TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);

      if(theErr && *theErr < 0)
	return;

      MED::TMeshInfo& aMeshInfo = *theInfo.myMeshInfo;
      TInt aNbElem = theInfo.myElemNum.size();

      TErr aRet;
      aRet = MEDpolygoneConnLire(myFile->Id(), 
				 &aMeshInfo.myName[0],
				 (med_int*)&theInfo.myIndex[0],
				 aNbElem+1,
				 (med_int*)&theInfo.myConn[0],
				 med_entite_maillage(theInfo.myEntity),
				 med_connectivite(theInfo.myConnMode));

      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetPolygoneInfo - MEDpolygoneInfo(...)");

      if(theInfo.myIsElemNames){
	GetNames(theInfo,aNbElem,theInfo.myEntity,ePOLYGONE,&aRet);
	if(theErr) 
	  *theErr = aRet;
      }

      if(theInfo.myIsElemNum){
	GetNumeration(theInfo,aNbElem,theInfo.myEntity,ePOLYGONE,&aRet);
	if(theErr) 
	  *theErr = aRet;
      }

      GetFamilies(theInfo,aNbElem,theInfo.myEntity,ePOLYGONE,&aRet);
      if(theErr) 
	*theErr = aRet;
    }
    
    void
    TVWrapper
    ::SetPolygoneInfo(const MED::TPolygoneInfo& theInfo,
		      TErr* theErr)
    {
      SetPolygoneInfo(theInfo,eLECTURE_ECRITURE,theErr);
    }
    
    void 
    TVWrapper
    ::SetPolygoneInfo(const MED::TPolygoneInfo& theInfo,
		      EModeAcces theMode,
		      TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,theMode,theErr);
      
      if(theErr && *theErr < 0)
	return;

      MED::TPolygoneInfo& anInfo = const_cast<MED::TPolygoneInfo&>(theInfo);
      MED::TMeshInfo& aMeshInfo = *anInfo.myMeshInfo;

      TErr aRet = MEDpolygoneConnEcr(myFile->Id(),
				     &aMeshInfo.myName[0],
				     (med_int*)&anInfo.myIndex[0],
				     anInfo.myNbElem+1,
				     (med_int*)&anInfo.myConn[0],
				     med_entite_maillage(theInfo.myEntity),
				     med_connectivite(theInfo.myConnMode));
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"SetPolygoneInfo - MEDpolygoneConnEcr(...)");
      
      SetNames(anInfo,theInfo.myEntity,ePOLYGONE,&aRet);
      if(theErr) 
	*theErr = aRet;
      
      SetNumeration(anInfo,theInfo.myEntity,ePOLYGONE,&aRet);
      if(theErr) 
	*theErr = aRet;
      
      SetFamilies(anInfo,theInfo.myEntity,ePOLYGONE,&aRet);
      if(theErr) 
	*theErr = aRet;
    }

    TInt 
    TVWrapper
    ::GetNbPolygones(const MED::TMeshInfo& theMeshInfo, 
		     EEntiteMaillage theEntity, 
		     EGeometrieElement theGeom, 
		     EConnectivite theConnMode,
		     TErr* theErr)
    {
      return GetNbCells(theMeshInfo,theEntity,theGeom,theConnMode,theErr);
    }
    
    TInt 
    TVWrapper
    ::GetPolygoneConnSize(const MED::TMeshInfo& theMeshInfo, 
			  EEntiteMaillage theEntity, 
			  EGeometrieElement theGeom, 
			  EConnectivite theConnMode,
			  TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);

      if(theErr && *theErr < 0)
	return 0;

      MED::TMeshInfo& aMeshInfo = const_cast<MED::TMeshInfo&>(theMeshInfo);
      
      med_int taille = 0;

      TErr aRet;
      aRet = MEDpolygoneInfo(myFile->Id(), 
			     &aMeshInfo.myName[0], 
			     med_entite_maillage(theEntity), 
			     med_connectivite(theConnMode),
			     &taille);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetPolygoneInfo - MEDpolygoneInfo(...)");

      return TInt(taille);
    }

    //-----------------------------------------------------------------
    void 
    TVWrapper
    ::GetPolyedreInfo(TPolyedreInfo& theInfo,
		      TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);

      if(theErr && *theErr < 0)
	return;

      MED::TMeshInfo& aMeshInfo = *theInfo.myMeshInfo;
      TInt aNbElem = theInfo.myElemNum.size();

      TErr aRet;
      aRet = MEDpolyedreConnLire(myFile->Id(), 
				 &aMeshInfo.myName[0],
				 (med_int*)&theInfo.myIndex[0],
				 aNbElem + 1,
				 (med_int*)&theInfo.myFaces[0],
				 theInfo.myFaces.size(),
				 (med_int*)&theInfo.myConn[0],
				 med_connectivite(theInfo.myConnMode));

      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetPolygoneInfo - MEDpolyedreConnLire(...)");

      if(theInfo.myIsElemNames){
	GetNames(theInfo,aNbElem,theInfo.myEntity,ePOLYEDRE,&aRet);
	if(theErr)
	  *theErr = aRet;
      }

      if(theInfo.myIsElemNum){
	GetNumeration(theInfo,aNbElem,theInfo.myEntity,ePOLYEDRE,&aRet);
	if(theErr) 
	  *theErr = aRet;
      }

      GetFamilies(theInfo,aNbElem,theInfo.myEntity,ePOLYEDRE,&aRet);
      if(theErr) 
	*theErr = aRet;
    }

    void
    TVWrapper
    ::SetPolyedreInfo(const TPolyedreInfo& theInfo,
		      TErr* theErr)
    {
      SetPolyedreInfo(theInfo,eLECTURE_ECRITURE,theErr);
    }
    
    void 
    TVWrapper
    ::SetPolyedreInfo(const MED::TPolyedreInfo& theInfo,
		      EModeAcces theMode,
		      TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,theMode,theErr);
      
      if(theErr && *theErr < 0)
	return;

      MED::TPolyedreInfo& anInfo = const_cast<MED::TPolyedreInfo&>(theInfo);
      MED::TMeshInfo& aMeshInfo = *anInfo.myMeshInfo;

      TErr aRet;
      aRet = MEDpolyedreConnEcr(myFile->Id(),
				&aMeshInfo.myName[0],
				(med_int*)&anInfo.myIndex[0],
				anInfo.myNbElem+1,
				(med_int*)&anInfo.myFaces[0],
				anInfo.myFaces.size(),
				(med_int*)&anInfo.myConn[0],
				med_connectivite(theInfo.myConnMode));
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"SetPolyedreInfo - MEDpolyedreConnEcr(...)");
      
      if(theInfo.myIsElemNames){
	aRet  = MEDnomEcr(myFile->Id(),
			  &aMeshInfo.myName[0],
			  &anInfo.myElemNames[0],
			  anInfo.myElemNames.size(),
			  med_entite_maillage(theInfo.myEntity),
			  MED_POLYEDRE);
	if(theErr) 
	  *theErr = aRet;
	else if(aRet < 0)
	  EXCEPTION(runtime_error,"SetPolyedreInfo - MEDnomEcr(...)");
      }
      
      if(theInfo.myIsElemNum){
	aRet  = MEDnumEcr(myFile->Id(),
			  &aMeshInfo.myName[0],
			  (med_int*)&anInfo.myElemNum[0],
			  anInfo.myElemNum.size(),
			  med_entite_maillage(theInfo.myEntity),
			  MED_POLYEDRE);
	if(theErr) 
	  *theErr = aRet;
	else if(aRet < 0)
	  EXCEPTION(runtime_error,"SetPolyedreInfo - MEDnumEcr(...)");
      }
      
      
      aRet = MEDfamEcr(myFile->Id(),
		       &aMeshInfo.myName[0],
		       (med_int*)&anInfo.myFamNum[0],
		       anInfo.myFamNum.size(),
		       med_entite_maillage(theInfo.myEntity),
		       MED_POLYEDRE);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"SetPolyedreInfo - MEDfamEcr(...)");
    }

    TInt
    TVWrapper
    ::GetNbPolyedres(const MED::TMeshInfo& theMeshInfo, 
		     EEntiteMaillage theEntity, 
		     EGeometrieElement theGeom, 
		     EConnectivite theConnMode,
		     TErr* theErr)
    {
      return GetNbCells(theMeshInfo,theEntity,theGeom,theConnMode,theErr);
    }

    void
    TVWrapper
    ::GetPolyedreConnSize(const TMeshInfo& theMeshInfo,
			  TInt& theNbFaces,
			  TInt& theConnSize,
			  EConnectivite theConnMode,
			  TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);

      if(theErr && *theErr < 0) 
	EXCEPTION(runtime_error,"GetPolyedreInfo - (...)");

      MED::TMeshInfo& aMeshInfo = const_cast<MED::TMeshInfo&>(theMeshInfo);
      
      TErr aRet = MEDpolyedreInfo(myFile->Id(), 
				  &aMeshInfo.myName[0], 
				  med_connectivite(theConnMode),
				  (med_int*)&theNbFaces,
				  (med_int*)&theConnSize);

      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetPolygoneInfo - MEDpolyedreInfo(...)");

    }
    
    //-----------------------------------------------------------------
    TEntityInfo
    TVWrapper
    ::GetEntityInfo(const MED::TMeshInfo& theMeshInfo,
		    EConnectivite theConnMode,
		    TErr* theErr)
    {
      TEntityInfo anInfo;
      
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return anInfo;

      if(theMeshInfo.GetType() == eNON_STRUCTURE) {

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
      } else { // eSTRUCTURE
	EGrilleType aGrilleType;
	TInt aNbNodes = 1;
	TInt aNbElem  = 1;
	TInt aDim = theMeshInfo.GetDim();
	EGeometrieElement aGeom;
	EEntiteMaillage anEntity = eMAILLE;
	switch(aDim){
	case 1:
	  aGeom = eSEG2;
	  break;
	case 2:
	  aGeom = eQUAD4;
	  break;
	case 3:
	  aGeom = eHEXA8;
	  break;
	}

	GetGrilleType(theMeshInfo,aGrilleType);

	if (aGrilleType == eGRILLE_STANDARD){
	  TIntVector theStruct;
	  theStruct.resize(aDim);
	  GetGrilleStruct(theMeshInfo,theStruct,theErr);
	  for(med_int i=0;i<aDim;i++){
	    aNbNodes = aNbNodes * theStruct[i];
	    aNbElem =  aNbElem * (theStruct[i]-1);
	  }
	  anInfo[eNOEUD][ePOINT1] = aNbNodes;
	  anInfo[anEntity][aGeom] = aNbElem;
	  
	} else { // eGRILLE_CARTESIENNE and eGRILLE_POLAIRE
	  med_table quoi;
	  for(int aAxe=1;aAxe<=aDim;aAxe++)
	  {
	    switch(aAxe) {
	    case 1 :
	      quoi = MED_COOR_IND1;
	      break;
	    case 2 :
	      quoi = MED_COOR_IND2;
	      break;
	    case 3 :
	      quoi = MED_COOR_IND3;
	      break;
	    }
	    TInt nbn = GetNbNodes(theMeshInfo,(ETable)quoi);
	    aNbNodes = aNbNodes * nbn;
	    aNbElem =  aNbElem * (nbn-1);
	  }
	  anInfo[eNOEUD][ePOINT1] = aNbNodes;
	  anInfo[anEntity][aGeom] = aNbElem;
	}
      }
      return anInfo;
    }
    
    
    //-----------------------------------------------------------------
    TInt
    TVWrapper
    ::GetNbCells(const MED::TMeshInfo& theMeshInfo, 
		 EEntiteMaillage theEntity, 
		 EGeometrieElement theGeom, 
		 EConnectivite theConnMode,
		 TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
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
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);

      if(theErr && *theErr < 0)
	return;
      
      MED::TMeshInfo& aMeshInfo = *theInfo.myMeshInfo;
      TInt aNbElem = theInfo.myElemNum.size();

      char* anElemNames = theInfo.myIsElemNames? &theInfo.myElemNames[0]: NULL;
      med_int* anElemNum = theInfo.myIsElemNum? &theInfo.myElemNum[0]: NULL;

      TErr aRet;
      aRet = MEDelementsLire(myFile->Id(),
			     &aMeshInfo.myName[0],
			     aMeshInfo.myDim,
			     (med_int*)&theInfo.myConn[0],
			     med_mode_switch(theInfo.myModeSwitch),
			     anElemNames,
			     (med_booleen*)&theInfo.myIsElemNames,
			     anElemNum,
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

      char* anElemNames = theInfo.myIsElemNames? &anInfo.myElemNames[0]: NULL;
      med_int* anElemNum = theInfo.myIsElemNum? &anInfo.myElemNum[0]: NULL;

      TErr aRet;
      aRet = MEDelementsEcr(myFile->Id(),
			    &aMeshInfo.myName[0],
			    aMeshInfo.myDim,
			    (med_int*)&anInfo.myConn[0],
			    med_mode_switch(theInfo.myModeSwitch),
			    anElemNames,
			    med_booleen(theInfo.myIsElemNames),
			    anElemNum,
			    med_booleen(theInfo.myIsElemNum),
			    (med_int*)&anInfo.myFamNum[0],
			    anInfo.myNbElem,
			    med_entite_maillage(theInfo.myEntity),
			    med_geometrie_element(theInfo.myGeom),
			    med_connectivite(theInfo.myConnMode));
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"SetCellInfo - MEDelementsLire(...)");
    }
    

    void
    TVWrapper
    ::SetCellInfo(const MED::TCellInfo& theInfo,
		  TErr* theErr)
    {
      SetCellInfo(theInfo,eLECTURE_ECRITURE,theErr);
    }
    

    //-----------------------------------------------------------------
    TInt
    TVWrapper
    ::GetNbFields(TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      return MEDnChamp(myFile->Id(),0);
    }
    
    
    TInt
    TVWrapper
    ::GetNbComp(TInt theFieldId,
		TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
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
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TErr aRet;
      TString aName(256); // PAL13122, protect from memory pb with too long names
      aRet = MEDchampInfo(myFile->Id(),
			  theFieldId,
			  &aName[0],// PAL13122 &theInfo.myName[0],
			  (med_type_champ*)&theInfo.myType,
			  &theInfo.myCompNames[0],
			  &theInfo.myUnitNames[0],
			  theInfo.myNbComp);
      theInfo.SetName(aName); // PAL13122

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
	EXCEPTION(runtime_error,"SetFieldInfo - MEDchampEcr(...)");
    }
    
    
    void
    TVWrapper
    ::SetFieldInfo(const MED::TFieldInfo& theInfo,
		   TErr* theErr)
    {
      TErr aRet;
      SetFieldInfo(theInfo,eLECTURE_ECRITURE,&aRet);
      
      if(aRet < 0)
	SetFieldInfo(theInfo,eLECTURE_AJOUT,&aRet);

      if(theErr) 
	*theErr = aRet;
    }
    
    
    //-----------------------------------------------------------------
    TInt
    TVWrapper
    ::GetNbGauss(TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      return MEDnGauss(myFile->Id());
    }


    TGaussInfo::TInfo
    TVWrapper
    ::GetGaussPreInfo(TInt theId, 
		      TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);

      if(theErr && *theErr < 0)
	return TGaussInfo::TInfo();
      
      med_int aNbGaussPoints = med_int();
      TVector<char> aName(GetNOMLength<eV2_2>()+1);
      med_geometrie_element aGeom = MED_NONE;

      TErr aRet;
      aRet = MEDgaussInfo(myFile->Id(),
			  theId,
			  &aName[0],
			  &aGeom,
			  &aNbGaussPoints);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetGaussPreInfo - MEDgaussInfo(...)");
      
      return TGaussInfo::TInfo(TGaussInfo::TKey(EGeometrieElement(aGeom),&aName[0]),
			       TInt(aNbGaussPoints));
    }


    void
    TVWrapper
    ::GetGaussInfo(TInt theId, 
		   TGaussInfo& theInfo,
		   TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TErr aRet;
      aRet = MEDgaussLire(myFile->Id(),
			  &theInfo.myRefCoord[0],
			  &theInfo.myGaussCoord[0],
			  &theInfo.myWeight[0],
			  med_mode_switch(theInfo.myModeSwitch),
			  &theInfo.myName[0]);

      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetGaussInfo - MEDgaussLire(...)");
    }


    //-----------------------------------------------------------------
    TInt
    TVWrapper
    ::GetNbProfiles(TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      return MEDnProfil(myFile->Id());
    }


    TProfileInfo::TInfo
    TVWrapper
    ::GetProfilePreInfo(TInt theId, 
			TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);

      if(theErr && *theErr < 0)
	return TProfileInfo::TInfo();
      
      TInt aSize = -1;
      TVector<char> aName(GetNOMLength<eV2_2>()+1);

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
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TErr aRet;
      aRet = MEDprofilLire(myFile->Id(),
			   &theInfo.myElemNum[0],
			   &theInfo.myName[0]);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetProfileInfo - MEDprofilLire(...)");
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
      theEntity = EEntiteMaillage(-1);
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);

      if(theErr){
	if(theEntityInfo.empty())
	  *theErr = -1;
	if(*theErr < 0)
	  return -1;
      }else if(theEntityInfo.empty()) 
	EXCEPTION(runtime_error,"GetNbTimeStamps - There is no any Entity on the Mesh");
      
      bool anIsPerformAdditionalCheck = GetNbMeshes() > 1;

      theGeom2Size.clear();
      TInt aNbTimeStamps = 0;
      TIdt anId = myFile->Id();

      MED::TFieldInfo& anInfo = const_cast<MED::TFieldInfo&>(theInfo);
      MED::TMeshInfo& aMeshInfo = anInfo.myMeshInfo;

      TEntityInfo::const_iterator anIter = theEntityInfo.begin();
      for(; anIter != theEntityInfo.end(); anIter++){
	med_entite_maillage anEntity = med_entite_maillage(anIter->first);
	const TGeom2Size& aGeom2Size = anIter->second;
	TGeom2Size::const_iterator anGeomIter = aGeom2Size.begin();
	for(; anGeomIter != aGeom2Size.end(); anGeomIter++){
	  med_geometrie_element aGeom = med_geometrie_element(anGeomIter->first);
	  TInt aNbStamps = MEDnPasdetemps(anId,
					  &anInfo.myName[0],
					  anEntity,
					  aGeom);
	  bool anIsSatisfied = aNbStamps > 0;
	  if(anIsSatisfied){
	    INITMSG(MYDEBUG,
		    "GetNbTimeStamps aNbTimeStamps = "<<aNbStamps<<
		    "; aGeom = "<<aGeom<<"; anEntity = "<<anEntity<<"\n");
	    if(anIsPerformAdditionalCheck){
	      TInt iTimeStampEnd = aNbStamps;
	      for(TInt iTimeStamp = 1; iTimeStamp <= iTimeStampEnd; iTimeStamp++){
		TVector<char> aMeshName(GetNOMLength<eV2_2>()+1);
		TVector<char> aDtUnit(GetPNOMLength<eV2_2>()+1);
		med_int aNbGauss;
		med_int aNumDt;
		med_int aNumOrd;
		med_float aDt;
		med_booleen anIsLocal;
		med_int aNbRef;
		TErr aRet = MEDpasdetempsInfo(anId,
					      &anInfo.myName[0],
					      anEntity,
					      aGeom,
					      iTimeStamp, 
					      &aNbGauss,
					      &aNumDt,  
					      &aNumOrd,
					      &aDtUnit[0],
					      &aDt, 
					      &aMeshName[0],
					      &anIsLocal,
					      &aNbRef);

		anIsSatisfied = (aRet == 0 && (!strcmp(&aMeshName[0],&aMeshInfo.myName[0])));
		if(!anIsSatisfied){
		  INITMSG(MYDEBUG,
			  "GetNbTimeStamps aMeshName = '"<<&aMeshName[0]<<"' != "<<
			  "; aMeshInfo.myName = '"<<&aMeshInfo.myName[0]<<"'\n");
		  break;
		}
	      }
	    }
	  }
	  if(anIsSatisfied){
	    theGeom2Size[EGeometrieElement(aGeom)] = anGeomIter->second;
	    theEntity = EEntiteMaillage(anEntity);
	    aNbTimeStamps = aNbStamps;
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
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      const TGeom2Size& aGeom2Size = theInfo.myGeom2Size;
      
      if(theErr){
	if(aGeom2Size.empty())
	  *theErr = -1;
	if(*theErr < 0)
	  return;
      }else if(aGeom2Size.empty())
	EXCEPTION(runtime_error,"GetTimeStampInfo - There is no any cell");
      
      MED::TFieldInfo& aFieldInfo = *theInfo.myFieldInfo;
      MED::TMeshInfo& aMeshInfo = *aFieldInfo.myMeshInfo;
      
      TGeom2NbGauss& aGeom2NbGauss = theInfo.myGeom2NbGauss;

      TGeom2Size::const_iterator anIter = aGeom2Size.begin();
      for(; anIter != aGeom2Size.end(); anIter++){
	const EGeometrieElement& aGeom = anIter->first;
	med_int aNbGauss = -1;

	TErr aRet;
	aRet = MEDpasdetempsInfo(myFile->Id(),
				 &aFieldInfo.myName[0],
				 med_entite_maillage(theInfo.myEntity),
				 med_geometrie_element(aGeom),
				 theTimeStampId,
				 &aNbGauss,
				 (med_int*)&theInfo.myNumDt,
				 (med_int*)&theInfo.myNumOrd,
				 &theInfo.myUnitDt[0],
				 &theInfo.myDt,
				 &aMeshInfo.myName[0],
				 (med_booleen*)&aFieldInfo.myIsLocal,
				 (med_int*)&aFieldInfo.myNbRef);

	static TInt MAX_NB_GAUSS_POINTS = 32;
	if(aNbGauss <= 0 || aNbGauss > MAX_NB_GAUSS_POINTS)
	  aNbGauss = 1;

	aGeom2NbGauss[aGeom] = aNbGauss;

	if(theErr) 
	  *theErr = aRet;
	else if(aRet < 0)
	  EXCEPTION(runtime_error,"GetTimeStampInfo - MEDpasdetempsInfo(...)");
      }      
    }
    

    void 
    TVWrapper
    ::GetTimeStampVal(TTimeStampVal& theVal,
		      const TMKey2Profile& theMKey2Profile,
		      const TKey2Gauss& theKey2Gauss,
		      TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TIdt anId = myFile->Id();
      
      MED::TTimeStampInfo& aTimeStampInfo = *theVal.myTimeStampInfo;
      MED::TFieldInfo& aFieldInfo = *aTimeStampInfo.myFieldInfo;
      MED::TMeshInfo& aMeshInfo = *aFieldInfo.myMeshInfo;
      
      TGeom2Gauss& aGeom2Gauss = aTimeStampInfo.myGeom2Gauss;
      TVector<char> aGaussName(GetNOMLength<eV2_2>()+1);

      med_mode_profil aProfileMode = med_mode_profil(boost::get<0>(theMKey2Profile));
      MED::TKey2Profile aKey2Profile = boost::get<1>(theMKey2Profile);
      MED::TGeom2Profile& aGeom2Profile = theVal.myGeom2Profile;
      TVector<char> aProfileName(GetNOMLength<eV2_2>()+1);

      TGeom2Size& aGeom2Size = aTimeStampInfo.myGeom2Size;
      TGeom2Size::iterator anIter = aGeom2Size.begin();
      for(; anIter != aGeom2Size.end(); anIter++){
	EGeometrieElement aGeom = anIter->first;
	TInt aNbCells = anIter->second;

	TInt aNbMeshRef = MEDnChampRef(anId,
				       &aFieldInfo.myName[0],
				       med_entite_maillage(aTimeStampInfo.myEntity),
				       med_geometrie_element(aGeom),
				       aTimeStampInfo.myNumDt,
				       aTimeStampInfo.myNumOrd);
	if(aNbMeshRef < 1){
	  if(theErr){
	    *theErr = MED_FAUX;
	    return;
	  }
	  EXCEPTION(runtime_error,"GetTimeStampVal - MEDnChampRef(...) < 1");
	}
	
	TErr aRet;
	med_int aNbGauss = -1;
	aRet = MEDchampRefInfo(anId,
			       &aFieldInfo.myName[0],
			       med_entite_maillage(aTimeStampInfo.myEntity),
			       med_geometrie_element(aGeom),
			       aNbMeshRef,
			       aTimeStampInfo.myNumDt,
			       aTimeStampInfo.myNumOrd, 
			       &aMeshInfo.myName[0],
			       (med_booleen*)&aFieldInfo.myIsLocal,
			       &aNbGauss);

	if(aRet < 0){
	  if(theErr){
	    *theErr = MED_FAUX;
	    return;
	  }
	  EXCEPTION(runtime_error,"GetTimeStampVal - MEDchampRefInfo(...)");
	}
 
	TInt aNbVal = MEDnVal(anId,
			      &aFieldInfo.myName[0],
			      med_entite_maillage(aTimeStampInfo.myEntity),
			      med_geometrie_element(aGeom),
			      aTimeStampInfo.myNumDt,
			      aTimeStampInfo.myNumOrd,
			      &aMeshInfo.myName[0],
			      aProfileMode);
	if(aNbVal <= 0){
	  if(theErr){
	    *theErr = -1;
	    return;
	  }
	  EXCEPTION(runtime_error,"GetTimeStampInfo - MEDnVal(...) - aNbVal == "<<aNbVal<<" <= 0");
	}
	
	TMeshValue& aMeshValue = theVal.GetMeshValue(aGeom);
	TInt aNbElem = aNbVal / aNbGauss;
	aMeshValue.Init(aNbElem,
			aNbGauss,
			aFieldInfo.myNbComp);
	TValue& aValue = aMeshValue.myValue;
	TInt anEnd = aValue.size();

	switch(aFieldInfo.myType){
	case eFLOAT64: {
	  TVector<TFloat> anArray(anEnd);
	  aRet = MEDchampLire(anId,
			      &aMeshInfo.myName[0],
			      &aFieldInfo.myName[0],
			      (unsigned char*)&anArray[0],
			      med_mode_switch(theVal.myModeSwitch),
			      MED_ALL,
			      &aGaussName[0],
			      &aProfileName[0],
			      aProfileMode,
			      med_entite_maillage(aTimeStampInfo.myEntity),
			      med_geometrie_element(aGeom),
			      aTimeStampInfo.myNumDt,
			      aTimeStampInfo.myNumOrd);
	  if(aRet >= 0) 
	    for(TInt anId = 0; anId < anEnd; anId++)
	      aValue[anId] = anArray[anId];
	  break;
	}
	default: {
	  TVector<TInt> anArray(anEnd);
	  aRet = MEDchampLire(anId,
			      &aMeshInfo.myName[0],
			      &aFieldInfo.myName[0],
			      (unsigned char*)&anArray[0],
			      med_mode_switch(theVal.myModeSwitch),
			      MED_ALL,
			      &aGaussName[0],
			      &aProfileName[0],
			      aProfileMode,
			      med_entite_maillage(aTimeStampInfo.myEntity),
			      med_geometrie_element(aGeom),
			      aTimeStampInfo.myNumDt,
			      aTimeStampInfo.myNumOrd);
	  if(aRet >= 0) 
	    for(TInt anId = 0; anId < anEnd; anId++)
	      aValue[anId] = anArray[anId];
	  break;
	}}

	if(aRet < 0){
	  if(theErr){
	    *theErr = MED_FAUX;
	    return;
	  }
	  EXCEPTION(runtime_error,"GetValTimeStamp - MEDchampLire(...)");
	}

	MED::PGaussInfo aGaussInfo;
	TGaussInfo::TKey aKey(aGeom,&aGaussName[0]);
	if(strcmp(&aGaussName[0],"") != 0){
	  MED::TKey2Gauss::const_iterator anIter = theKey2Gauss.find(aKey);
	  if(anIter != theKey2Gauss.end()){
	    aGaussInfo = anIter->second;
	    aGeom2Gauss[aGeom] = aGaussInfo;
	  }
	}
	
	MED::PProfileInfo aProfileInfo;
	if(strcmp(&aProfileName[0],"") != 0){
	  MED::TKey2Profile::const_iterator anIter = aKey2Profile.find(&aProfileName[0]);
	  if(anIter != aKey2Profile.end()){
	    aProfileInfo = anIter->second;
	    aGeom2Profile[aGeom] = aProfileInfo;
	  }
	}

	if(aNbGauss > 1 && !aGaussInfo){
	  if(theErr){
	    *theErr = MED_FAUX;
	    return;
	  }
	  EXCEPTION(runtime_error,"GetValTimeStamp "<<
		    "- aNbGauss("<<aNbGauss<<") > 1 && !aGaussInfo"<<
		    "; aGaussName = '"<<&aGaussName[0]<<"'"<<
		    "; aGeom = "<<aGeom<<
		    "");
	}
	  
	if(aGaussInfo && aNbGauss != aGaussInfo->GetNbGauss()){
	  if(theErr){
	    *theErr = MED_FAUX;
	    return;
	  }
	  EXCEPTION(runtime_error,"GetValTimeStamp - aNbGauss != aGaussInfo->GetNbGauss()");
	}
	
	if(aProfileInfo && aProfileInfo->IsPresent()){
	  TInt aNbSubCells = aProfileInfo->GetSize();

	  TInt aSize = aNbSubCells*aFieldInfo.myNbComp*aNbGauss;
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
		      "; aGeom = "<<aGeom<<
		      "; aNbCells = "<<aNbCells<<
		      "; aNbSubCells = "<<aNbSubCells<<
		      "; aNbComp = "<<aFieldInfo.myNbComp<<
		      "; aNbGauss = "<<aNbGauss<<
		      "");
	  }
	}else{
	  if(anEnd != aValue.size()){
	    if(theErr){
	      *theErr = -1;
	      return;
	    }
	    EXCEPTION(runtime_error,
		      "GetTimeStampInfo - anEnd("<<anEnd<<
		      ") != aValue.size()("<<aValue.size()<<
		      "); aNbVal = "<<aNbVal<<
		      "; anEntity = "<<aTimeStampInfo.myEntity<<
		      "; aGeom = "<<aGeom<<
		      "; aNbCells = "<<aNbCells<<
		      "; aNbComp = "<<aFieldInfo.myNbComp<<
		      "; aNbGauss = "<<aNbGauss<<
		      "");
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

      MED::TTimeStampInfo& aTimeStampInfo = *aVal.myTimeStampInfo;
      MED::TFieldInfo& aFieldInfo = *aTimeStampInfo.myFieldInfo;
      MED::TMeshInfo& aMeshInfo = *aFieldInfo.myMeshInfo;
      MED::TGeom2Value& aGeom2Value = aVal.myGeom2Value;
      
      MED::TGeom2Gauss& aGeom2Gauss = aTimeStampInfo.myGeom2Gauss;
      MED::TGeom2Profile& aGeom2Profile = aVal.myGeom2Profile;

      TGeom2Value::iterator anIter = aGeom2Value.begin();
      for(; anIter != aGeom2Value.end(); anIter++){
	EGeometrieElement aGeom = anIter->first;
	TMeshValue& aMeshValue = anIter->second;

	TVector<char> aGaussName(GetNOMLength<eV2_2>()+1);
	MED::TGeom2Gauss::const_iterator aGaussIter = aGeom2Gauss.find(aGeom);
	if(aGaussIter != aGeom2Gauss.end()){
	  MED::PGaussInfo aGaussInfo = aGaussIter->second;
	  strcpy(&aGaussName[0],&aGaussInfo->myName[0]);
	}

	TVector<char> aProfileName(GetNOMLength<eV2_2>()+1);
	med_mode_profil aProfileMode = med_mode_profil(eNO_PFLMOD);
	MED::TGeom2Profile::const_iterator aProfileIter = aGeom2Profile.find(aGeom);
	if(aProfileIter != aGeom2Profile.end()){
	  MED::PProfileInfo aProfileInfo = aProfileIter->second;
	  aProfileMode = med_mode_profil(aProfileInfo->myMode);
	  strcpy(&aProfileName[0],&aProfileInfo->myName[0]);
	}

	med_int aNbVal = aMeshValue.myNbElem * aMeshValue.myNbGauss;
	TValue& aValue = aMeshValue.myValue;
	TInt anEnd = aValue.size();
	
	switch(aFieldInfo.myType){
	case eFLOAT64: {
	  TVector<TFloat>& anArray = aValue;
	  
	  aRet = MEDchampEcr(anId,
			     &aMeshInfo.myName[0],
			     &aFieldInfo.myName[0],
			     (unsigned char*)&anArray[0],
			     med_mode_switch(theVal.myModeSwitch),
			     aNbVal,
			     &aGaussName[0],
			     MED_ALL,
			     &aProfileName[0],
			     aProfileMode,
			     med_entite_maillage(aTimeStampInfo.myEntity),
			     med_geometrie_element(aGeom),
			     aTimeStampInfo.myNumDt,
			     &aTimeStampInfo.myUnitDt[0],
			     aTimeStampInfo.myDt,
			     aTimeStampInfo.myNumOrd);
	  break;
	}
	default: {
	  vector<TInt> anArray(anEnd);
	  for(TInt anID = 0; anID < anEnd; anID++)
	    anArray[anID] = TInt(aValue[anID]);
	  
	  aRet = MEDchampEcr(anId,
			     &aMeshInfo.myName[0],
			     &aFieldInfo.myName[0],
			     (unsigned char*)&anArray[0],
			     med_mode_switch(theVal.myModeSwitch),
			     aNbVal,
			     &aGaussName[0],
			     MED_ALL,
			     &aProfileName[0],
			     aProfileMode,
			     med_entite_maillage(aTimeStampInfo.myEntity),
			     med_geometrie_element(aGeom),
			     aTimeStampInfo.myNumDt,
			     &aTimeStampInfo.myUnitDt[0],
			     aTimeStampInfo.myDt,
			     aTimeStampInfo.myNumOrd);
	  break;
	}}
	
	if(aRet < 0){
	  if(theErr){
	    *theErr = MED_FAUX;
	    break;
	  }
	  EXCEPTION(runtime_error,"SetTimeStamp - MEDchampEcr(...)");
	}
	
      }
      
      INITMSG(MYDEBUG,"TVWrapper::SetMeshInfo - MED_MODE_ACCES = "<<theMode<<"; aRet = "<<aRet<<endl);
    }

    
    void 
    TVWrapper
    ::SetTimeStamp(const MED::TTimeStampVal& theVal,
		   TErr* theErr)
    {
      TErr aRet;
      SetTimeStamp(theVal,eLECTURE_ECRITURE,&aRet);
      
      if(aRet < 0)
	SetTimeStamp(theVal,eLECTURE_AJOUT,&aRet);

      if(theErr) 
	*theErr = aRet;
    }
    
    void 
    TVWrapper
    ::SetGrilleInfo(const MED::TGrilleInfo& theInfo,
		    TErr* theErr)
    {
      SetGrilleInfo(theInfo,eLECTURE_ECRITURE,theErr);
    }

    void 
    TVWrapper
    ::SetGrilleInfo(const MED::TGrilleInfo& theInfo,
		    EModeAcces theMode,
		    TErr* theErr)
    {
      if(theInfo.myMeshInfo->myType != eSTRUCTURE)
	return;
      TFileWrapper aFileWrapper(myFile,theMode,theErr);
      
      if(theErr && *theErr < 0)
	  return;

      MED::TGrilleInfo& anInfo = const_cast<MED::TGrilleInfo&>(theInfo);
      MED::TMeshInfo& aMeshInfo = *anInfo.myMeshInfo;

      TErr aRet = 0;
      aRet = MEDnatureGrilleEcr(myFile->Id(),
				&aMeshInfo.myName[0],
				(med_type_grille)anInfo.myGrilleType);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"SetGrilleInfo - MEDnatureGrilleEcr(...)");
      
      if(anInfo.myGrilleType == eGRILLE_STANDARD){
	med_int nnoeuds = (med_int)(anInfo.myCoord.size()/aMeshInfo.myDim);
	aRet = MEDcoordEcr(myFile->Id(),
			   &aMeshInfo.myName[0],
			   aMeshInfo.myDim,
			   &anInfo.myCoord[0],
			   med_mode_switch(anInfo.myModeSwitch),
			   nnoeuds,
			   (med_repere)theInfo.myGrilleType,
			   &anInfo.myCoordNames[0],
			   &anInfo.myCoordUnits[0]);

	if(aRet < 0)
	  EXCEPTION(runtime_error,"SetGrilleInfo - MEDcoordEcr(...)");

	aRet = MEDstructureCoordEcr(myFile->Id(),
				    &aMeshInfo.myName[0],
				    aMeshInfo.myDim,
				    &anInfo.myGrilleStructure[0]);
	if(aRet < 0)
	  EXCEPTION(runtime_error,"SetGrilleInfo - MEDstructureCoordEcr(...)");
	
      } else {
	for(int aAxe=0;aAxe<aMeshInfo.myDim;aAxe++){
	  aRet = MEDindicesCoordEcr(myFile->Id(),
				    &aMeshInfo.myName[0],
				    aMeshInfo.myDim,
				    &anInfo.GetIndexes(aAxe)[0],
				    anInfo.GetIndexes(aAxe).size(),
				    aAxe+1,
				    &anInfo.GetCoordName(aAxe)[0],
				    &anInfo.GetCoordUnit(aAxe)[0]);
	  if(aRet < 0)
	    EXCEPTION(runtime_error,"SetGrilleInfo - MEDindicesCoordEcr(...)");
	}
	
      }

      return;
    }

    void
    TVWrapper
    ::GetGrilleInfo(TGrilleInfo& theInfo,
		    TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);

      if(theErr && *theErr < 0)
	  return;
      
      MED::TMeshInfo& aMeshInfo = *theInfo.myMeshInfo;
      
      EMaillage type_maillage   = aMeshInfo.myType;
      
      GetGrilleType(aMeshInfo,theInfo.myGrilleType,theErr);
      EGrilleType       aGrtype = theInfo.myGrilleType;

      TErr aRet;
      if(type_maillage == eSTRUCTURE && aGrtype == eGRILLE_STANDARD){
	GetGrilleStruct(aMeshInfo,theInfo.myGrilleStructure,theErr);
	med_repere aRep;
	aRet = MEDcoordLire(myFile->Id(),
			    &aMeshInfo.myName[0],
			    aMeshInfo.myDim,
			    &theInfo.myCoord[0],
			    med_mode_switch(theInfo.myModeSwitch),
			    MED_ALL, // all coordinates must be return
			    NULL,
			    0,
			    &aRep,//(med_repere*)&theInfo.myGrilleType,
			    &theInfo.myCoordNames[0],
			    &theInfo.myCoordUnits[0]);

	if(theErr) 
	  *theErr = aRet;
	else if(aRet < 0)
	  EXCEPTION(runtime_error,"GetGrilleInfo - MEDcoordLire(...)");

	TInt theNbNodes = theInfo.GetNbNodes();//GetNbFamilies(aMeshInfo);
	
	aRet = MEDfamLire(myFile->Id(),
			  &aMeshInfo.myName[0],
			  (med_int*)&theInfo.myFamNumNode[0],
			  theNbNodes,
			  med_entite_maillage(eNOEUD),
			  med_geometrie_element(ePOINT1));

	if(theErr) 
	  *theErr = aRet;
	else if(aRet < 0)
	  EXCEPTION(runtime_error,"GetGrilleInfo - MEDfamLire(...) of NODES");

	//============================
      }

      if(type_maillage == eSTRUCTURE && aGrtype != eGRILLE_STANDARD){
	med_table quoi;
	for(int aAxe=1;aAxe<=aMeshInfo.myDim;aAxe++)
	  {
	    switch(aAxe) {
	    case 1 :
	      quoi = MED_COOR_IND1;
	      break;
	    case 2 :
	      quoi = MED_COOR_IND2;
	      break;
	    case 3 :
	      quoi = MED_COOR_IND3;
	      break;

	    default :
	      aRet = -1;
	    }
	    
	    if(theErr) 
	      *theErr = aRet;
	    else if(aRet < 0)
	      EXCEPTION(runtime_error,"GetGrilleInfo - Axe number out of range(...)");

	    TInt nind = GetNbNodes(aMeshInfo,(ETable)quoi);
	    if(nind < 0)
	      EXCEPTION(runtime_error,"GetGrilleInfo - Erreur a la lecture de la taille de l'indice");
	    
	    med_float* indices = (med_float*)&theInfo.myIndixes[aAxe-1][0];
	    
	    char comp[MED_TAILLE_PNOM+1];
	    char unit[MED_TAILLE_PNOM+1];
	    
	    aRet = MEDindicesCoordLire(myFile->Id(),
				       &aMeshInfo.myName[0],
				       aMeshInfo.myDim,
				       indices,
				       (med_int)nind,
				       (med_int)aAxe,
				       comp,
				       unit);
	    theInfo.SetCoordName(aAxe-1,comp);
	    theInfo.SetCoordUnit(aAxe-1,unit);
	    if(theErr) 
	      *theErr = aRet;
	    else if(aRet < 0)
	      EXCEPTION(runtime_error,"GetGrilleInfo - MEDindicesCoordLire(...)");
	  }
      }

      EGeometrieElement aGeom   = theInfo.GetGeom();
      EEntiteMaillage   aEntity = theInfo.GetEntity();
      TInt theNbCells           = theInfo.GetNbCells();
      
      theInfo.myFamNum.resize(theNbCells);
      
      aRet = MEDfamLire(myFile->Id(),
			&aMeshInfo.myName[0],
			(med_int*)&theInfo.myFamNum[0],
			theNbCells,
			med_entite_maillage(aEntity),
			med_geometrie_element(aGeom));
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0){
	//EXCEPTION(runtime_error,"GetGrilleInfo - MEDfamLire(...) of CELLS");
	aRet = 0;
	theInfo.myFamNum.resize(0);
      }
      
    }

    void
    TVWrapper
    ::GetGrilleType(const MED::TMeshInfo& theMeshInfo,
		    EGrilleType& type,
		    TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);

      if(theErr && *theErr < 0)
 	EXCEPTION(runtime_error," GetGrilleType - aFileWrapper (...)");

      MED::TMeshInfo& aMeshInfo = const_cast<MED::TMeshInfo&>(theMeshInfo);
      
      EMaillage type_maillage   = aMeshInfo.myType;
      
      TErr aRet;
      if(type_maillage == eSTRUCTURE){
	med_type_grille Mtype;
	aRet = MEDnatureGrilleLire(myFile->Id(),
				   &aMeshInfo.myName[0],
				   &Mtype);
	if(aRet < 0)
	  EXCEPTION(runtime_error,"GetGrilleInfo - MEDnatureGrilleLire(...)");

	type = (EGrilleType)Mtype;

      }
    }    
    
    void
    TVWrapper
    ::GetGrilleStruct(const MED::TMeshInfo& theMeshInfo,
		      TIntVector& theStruct,
		      TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	  return;
      
      TErr aRet;
      MED::TMeshInfo& aMeshInfo = const_cast<MED::TMeshInfo&>(theMeshInfo);

      med_int* anGrilleStruct = aMeshInfo.myDim > 0? (med_int*)&theStruct[0]: NULL;
	
      aRet = MEDstructureCoordLire(myFile->Id(),
				   &aMeshInfo.myName[0],
				   aMeshInfo.myDim,
				   anGrilleStruct);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(runtime_error,"GetGrilleInfo - MEDstructureCoordLire(...)");
    }

  }
}
