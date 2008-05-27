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



//#include <med.h>
//using namespace med_2_2;

extern "C"
{
#include <med.h>

med_err
MEDgaussInfo(med_idt fid, int indice, char * locname, med_geometrie_element * type_geo,
	     med_int * ngauss );

med_int
MEDnMaa(med_idt fid);

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
  void
  GetVersionRelease<eV2_2>(TInt& majeur, TInt& mineur, TInt& release)
  {
    MEDversionDonner(&majeur, &mineur, &release);
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
      
      ~TFile()
      { 
	Close();
      }
      
      void
      Open(EModeAcces theMode, TErr* theErr = NULL)
      {
	if(myCount++ == 0){
	  char* aFileName = const_cast<char*>(myFileName.c_str());
	  myFid = MEDouvrir(aFileName,med_mode_acces(theMode));
	}
	if(theErr)
	  *theErr = TErr(myFid);
	else if(myFid < 0)
	  EXCEPTION(std::runtime_error,"TFile - MEDouvrir('"<<myFileName<<"',"<<theMode<<")");
      }

      const TIdt& Id() const 
      { 
	if(myFid < 0)
	  EXCEPTION(std::runtime_error,"TFile - GetFid() < 0");
	return myFid;
      }

      void Close()
      { 
	if(--myCount == 0)
	  MEDfermer(myFid);
      }

    protected:
      std::string myFileName;
      TInt myCount;
      TIdt myFid;
    };


    //---------------------------------------------------------------
    class TFileWrapper
    {
      PFile myFile;

    public:
      TFileWrapper(const PFile& theFile, EModeAcces theMode, TErr* theErr = NULL): 
	myFile(theFile)
      {
	myFile->Open(theMode,theErr);
      }
      
      ~TFileWrapper()
      {
	myFile->Close();
      }
    };


    //---------------------------------------------------------------
    TVWrapper::TVWrapper(const std::string& theFileName): 
      myFile(new TFile(theFileName))
    {}
    
    
    //----------------------------------------------------------------------------
    TInt
    TVWrapper
    ::GetNbMeshes(TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      return MEDnMaa(myFile->Id());
    }
    
    
    //----------------------------------------------------------------------------
    void
    TVWrapper
    ::GetMeshInfo(TInt theMeshId, 
		  MED::TMeshInfo& theInfo,
		  TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TValueHolder<TString, char> aMeshName(theInfo.myName);
      TValueHolder<TInt, med_int> aDim(theInfo.myDim);
      TValueHolder<EMaillage, med_maillage> aType(theInfo.myType);

      TErr aRet = MEDmaaInfo(myFile->Id(),
			     theMeshId,
			     &aMeshName,
			     &aDim,
			     &aType,
			     &theInfo.myDesc[0]);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"GetMeshInfo - MEDmaaInfo(...)");
    }
    
    
    //----------------------------------------------------------------------------
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
      
      TValueHolder<TString, char> aMeshName(anInfo.myName);
      TValueHolder<TInt, med_int> aDim(anInfo.myDim);
      TValueHolder<EMaillage, med_maillage> aType(anInfo.myType);
      TValueHolder<TString, char> aDesc(anInfo.myDesc);

      TErr aRet = MEDmaaCr(myFile->Id(),
			   &aMeshName,
			   aDim,
			   aType,
			   &aDesc);
      if(aRet == 0){
	aRet = MEDunvCr(myFile->Id(),
			&aMeshName);
      }
      
      INITMSG(MYDEBUG,"TVWrapper::SetMeshInfo - MED_MODE_ACCES = "<<theMode<<"; aRet = "<<aRet<<std::endl);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"SetMeshInfo - MEDmaaCr(...)");
    }
    
    
    //----------------------------------------------------------------------------
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
    
    
    //----------------------------------------------------------------------------
    TInt
    TVWrapper
    ::GetNbFamilies(const MED::TMeshInfo& theInfo,
		    TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      MED::TMeshInfo& anInfo = const_cast<MED::TMeshInfo&>(theInfo);
      TValueHolder<TString, char> aName(anInfo.myName);
      return MEDnFam(myFile->Id(),
		     &aName);
    }
    
    
    //----------------------------------------------------------------------------
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

      TValueHolder<TString, char> aName(anInfo.myName);

      return MEDnAttribut(myFile->Id(),
			  &aName,
			  theFamId);
    }
    
    
    //----------------------------------------------------------------------------
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

      TValueHolder<TString, char> aName(anInfo.myName);

      return MEDnGroupe(myFile->Id(),
			&aName,
			theFamId);
    }
    
    
    //----------------------------------------------------------------------------
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
      
      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<TString, char> aFamilyName(theInfo.myName);
      TValueHolder<TInt, med_int> aFamilyId(theInfo.myId);
      TValueHolder<TFamAttr, med_int> anAttrId(theInfo.myAttrId);
      TValueHolder<TFamAttr, med_int> anAttrVal(theInfo.myAttrVal);
      TValueHolder<TInt, med_int> aNbAttr(theInfo.myNbAttr);
      TValueHolder<TString, char> anAttrDesc(theInfo.myAttrDesc);
      TValueHolder<TInt, med_int> aNbGroup(theInfo.myNbGroup);
      TValueHolder<TString, char> aGroupNames(theInfo.myGroupNames);

      TErr aRet = MEDfamInfo(myFile->Id(),
			     &aMeshName,
			     theFamId,
			     &aFamilyName,
			     &aFamilyId,
			     &anAttrId,
			     &anAttrVal,
			     &anAttrDesc,
			     &aNbAttr,
			     &aGroupNames,
			     &aNbGroup);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"GetFamilyInfo - MEDfamInfo(...) - "<<
		  " aMeshInfo.myName = '"<<&aMeshName<<
		  "'; theFamId = "<<theFamId<<
		  "; theInfo.myNbGroup = "<<aNbGroup()<<
		  "; theInfo.myNbAttr = "<<aNbAttr());
    }
    
    
    //----------------------------------------------------------------------------
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
      
      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<TString, char> aFamilyName(anInfo.myName);
      TValueHolder<TInt, med_int> aFamilyId(anInfo.myId);
      TValueHolder<TFamAttr, med_int> anAttrId(anInfo.myAttrId);
      TValueHolder<TFamAttr, med_int> anAttrVal(anInfo.myAttrVal);
      TValueHolder<TInt, med_int> aNbAttr(anInfo.myNbAttr);
      TValueHolder<TString, char> anAttrDesc(anInfo.myAttrDesc);
      TValueHolder<TInt, med_int> aNbGroup(anInfo.myNbGroup);
      TValueHolder<TString, char> aGroupNames(anInfo.myGroupNames);

      TErr aRet = MEDfamCr(myFile->Id(),
			   &aMeshName,
			   &aFamilyName,
			   aFamilyId,
			   &anAttrId,
			   &anAttrVal,
			   &anAttrDesc,
			   aNbAttr,
			   &aGroupNames,
			   aNbGroup);
      
      INITMSG(MYDEBUG,"TVWrapper::GetFamilyInfo - MED_MODE_ACCES = "<<theMode<<"; aRet = "<<aRet<<std::endl);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"SetFamilyInfo - MEDfamCr(...)");
    }
    
    
    //----------------------------------------------------------------------------
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
    
    //----------------------------------------------------------------------------
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

      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<TString, char> anElemNames(theInfo.myElemNames);
      TValueHolder<EEntiteMaillage, med_entite_maillage> anEntity(theEntity);
      TValueHolder<EGeometrieElement, med_geometrie_element> aGeom(theGeom);
      
      TErr aRet = MEDnomLire(myFile->Id(),
			     &aMeshName,
			     &anElemNames,
			     theNb,
			     anEntity,
			     aGeom);

      theInfo.myIsElemNames = aRet != 0? eFAUX : eVRAI ;

      if(theErr)
        *theErr = aRet;
    }

    //----------------------------------------------------------------------------
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
      
      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<TElemNum, med_int> anElemNum(theInfo.myElemNum);
      TValueHolder<EEntiteMaillage, med_entite_maillage> anEntity(theEntity);
      TValueHolder<EGeometrieElement, med_geometrie_element> aGeom(theGeom);
      
      TErr aRet = MEDnumLire(myFile->Id(),
			     &aMeshName,
			     &anElemNum,
			     theNb,
			     anEntity,
			     aGeom);

      theInfo.myIsElemNum = aRet != 0? eFAUX : eVRAI;

      if(theErr)
        *theErr = aRet;
    }

    //----------------------------------------------------------------------------
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

      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<TElemNum, med_int> aFamNum(theInfo.myFamNum);
      TValueHolder<EEntiteMaillage, med_entite_maillage> anEntity(theEntity);
      TValueHolder<EGeometrieElement, med_geometrie_element> aGeom(theGeom);
      
      TErr aRet = MEDfamLire(myFile->Id(),
			     &aMeshName,
			     &aFamNum,
			     theNb,
			     anEntity,
			     aGeom);

      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"GetFamilies - MEDfamLire(...)");
    }


    //----------------------------------------------------------------------------
    void
    TVWrapper
    ::SetNames(const TElemInfo& theInfo,
	       EEntiteMaillage theEntity, 
	       EGeometrieElement theGeom,
	       TErr* theErr)
    { 
      SetNames(theInfo,eLECTURE_ECRITURE,theEntity,theGeom,theErr);
    }


    //----------------------------------------------------------------------------
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
	TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
	TValueHolder<TString, char> anElemNames(anInfo.myElemNames);
	TValueHolder<EEntiteMaillage, med_entite_maillage> anEntity(theEntity);
	TValueHolder<EGeometrieElement, med_geometrie_element> aGeom(theGeom);
      
	aRet  = MEDnomEcr(myFile->Id(),
			  &aMeshName,
			  &anElemNames,
			  (TInt)anInfo.myElemNames->size(),
			  anEntity,
			  aGeom);
	if(theErr) 
	  *theErr = aRet;
	else if(aRet < 0)
	  EXCEPTION(std::runtime_error,"SetNames - MEDnomEcr(...)");
      }
    }


    //----------------------------------------------------------------------------
    void
    TVWrapper
    ::SetNumeration(const TElemInfo& theInfo,
		    EEntiteMaillage theEntity, 
		    EGeometrieElement theGeom,
		    TErr* theErr)
    { 
      SetNumeration(theInfo,eLECTURE_ECRITURE,theEntity,theGeom,theErr);
    }


    //----------------------------------------------------------------------------
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
	TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
	TValueHolder<TElemNum, med_int> anElemNum(anInfo.myElemNum);
	TValueHolder<EEntiteMaillage, med_entite_maillage> anEntity(theEntity);
	TValueHolder<EGeometrieElement, med_geometrie_element> aGeom(theGeom);
      
	aRet  = MEDnumEcr(myFile->Id(),
			  &aMeshName,
			  &anElemNum,
			  (TInt)anInfo.myElemNum->size(),
			  anEntity,
			  aGeom);
	if(theErr) 
	  *theErr = aRet;
	else if(aRet < 0)
	  EXCEPTION(std::runtime_error,"SetNumeration - MEDnumEcr(...)");
      }
    }

    //----------------------------------------------------------------------------
    void
    TVWrapper
    ::SetFamilies(const TElemInfo& theInfo,
		  EEntiteMaillage theEntity, 
		  EGeometrieElement theGeom,
		  TErr* theErr)
    { 
      SetFamilies(theInfo,eLECTURE_ECRITURE,theEntity,theGeom,theErr);
    }

    //----------------------------------------------------------------------------
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

      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<TElemNum, med_int> aFamNum(anInfo.myFamNum);
      TValueHolder<EEntiteMaillage, med_entite_maillage> anEntity(theEntity);
      TValueHolder<EGeometrieElement, med_geometrie_element> aGeom(theGeom);
      
      TErr aRet = MEDfamEcr(myFile->Id(),
			    &aMeshName,
			    &aFamNum,
			    (TInt)anInfo.myFamNum->size(),
			    anEntity,
			    aGeom);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"SetFamilies - MEDfamEcr(...)");
    }
    
    //----------------------------------------------------------------------------
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
      
      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<ETable, med_table> aTable(theTable);

      return MEDnEntMaa(myFile->Id(),
			&aMeshName,
			aTable,
			MED_NOEUD,
			med_geometrie_element(0),
			med_connectivite(0));
    }
    
    
    //----------------------------------------------------------------------------
    void
    TVWrapper
    ::GetNodeInfo(MED::TNodeInfo& theInfo,
		  TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      MED::TMeshInfo& aMeshInfo = *theInfo.myMeshInfo;

      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<TInt, med_int> aDim(aMeshInfo.myDim);
      TValueHolder<TNodeCoord, med_float> aCoord(theInfo.myCoord);
      TValueHolder<EModeSwitch, med_mode_switch> aModeSwitch(theInfo.myModeSwitch);
      TValueHolder<ERepere, med_repere> aSystem(theInfo.mySystem);
      TValueHolder<TString, char> aCoordNames(theInfo.myCoordNames);
      TValueHolder<TString, char> aCoordUnits(theInfo.myCoordUnits);
      TValueHolder<TString, char> anElemNames(theInfo.myElemNames);
      TValueHolder<EBooleen, med_booleen> anIsElemNames(theInfo.myIsElemNames);
      TValueHolder<TElemNum, med_int> anElemNum(theInfo.myElemNum);
      TValueHolder<EBooleen, med_booleen> anIsElemNum(theInfo.myIsElemNum);
      TValueHolder<TElemNum, med_int> aFamNum(theInfo.myFamNum);
      TValueHolder<TInt, med_int> aNbElem(theInfo.myNbElem);

      TErr aRet = MEDnoeudsLire(myFile->Id(),
				&aMeshName,
				aDim,
				&aCoord,
				aModeSwitch,
				&aSystem,
				&aCoordNames,
				&aCoordUnits,
				&anElemNames,
				&anIsElemNames,
				&anElemNum,
				&anIsElemNum,
				&aFamNum,
				aNbElem);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"GetNodeInfo - MEDnoeudsLire(...)");
    }
    
    
    //----------------------------------------------------------------------------
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
      
      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<TInt, med_int> aDim(aMeshInfo.myDim);
      TValueHolder<TNodeCoord, med_float> aCoord(anInfo.myCoord);
      TValueHolder<EModeSwitch, med_mode_switch> aModeSwitch(anInfo.myModeSwitch);
      TValueHolder<ERepere, med_repere> aSystem(anInfo.mySystem);
      TValueHolder<TString, char> aCoordNames(anInfo.myCoordNames);
      TValueHolder<TString, char> aCoordUnits(anInfo.myCoordUnits);
      TValueHolder<TString, char> anElemNames(anInfo.myElemNames);
      TValueHolder<EBooleen, med_booleen> anIsElemNames(anInfo.myIsElemNames);
      TValueHolder<TElemNum, med_int> anElemNum(anInfo.myElemNum);
      TValueHolder<EBooleen, med_booleen> anIsElemNum(anInfo.myIsElemNum);
      TValueHolder<TElemNum, med_int> aFamNum(anInfo.myFamNum);
      TValueHolder<TInt, med_int> aNbElem(anInfo.myNbElem);

      TErr aRet = MEDnoeudsEcr(myFile->Id(),
			       &aMeshName,
			       aDim,
			       &aCoord,
			       aModeSwitch,
			       aSystem,
			       &aCoordNames,
			       &aCoordUnits,
			       &anElemNames,
			       anIsElemNames,
			       &anElemNum,
			       anIsElemNum,
			       &aFamNum,
			       aNbElem);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"SetNodeInfo - MEDnoeudsEcr(...)");
    }
    
    
    //----------------------------------------------------------------------------
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

      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<TElemNum, med_int> anIndex(theInfo.myIndex);
      TInt aNbElem = (TInt)theInfo.myElemNum->size();
      TValueHolder<TElemNum, med_int> aConn(theInfo.myConn);
      TValueHolder<EEntiteMaillage, med_entite_maillage> anEntity(theInfo.myEntity);
      TValueHolder<EConnectivite, med_connectivite> aConnMode(theInfo.myConnMode);

      TErr aRet;
      aRet = MEDpolygoneConnLire(myFile->Id(), 
				 &aMeshName,
				 &anIndex,
				 aNbElem+1,
				 &aConn,
				 anEntity,
				 aConnMode);

      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"GetPolygoneInfo - MEDpolygoneInfo(...)");

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
    
    //----------------------------------------------------------------------------
    void
    TVWrapper
    ::SetPolygoneInfo(const MED::TPolygoneInfo& theInfo,
		      TErr* theErr)
    {
      SetPolygoneInfo(theInfo,eLECTURE_ECRITURE,theErr);
    }
    
    //----------------------------------------------------------------------------
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

      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<TElemNum, med_int> anIndex(anInfo.myIndex);
      TValueHolder<TElemNum, med_int> aConn(anInfo.myConn);
      TValueHolder<EEntiteMaillage, med_entite_maillage> anEntity(anInfo.myEntity);
      TValueHolder<EConnectivite, med_connectivite> aConnMode(anInfo.myConnMode);

      TErr aRet = MEDpolygoneConnEcr(myFile->Id(),
				     &aMeshName,
				     &anIndex,
				     anInfo.myNbElem + 1,
				     &aConn,
				     anEntity,
				     aConnMode);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"SetPolygoneInfo - MEDpolygoneConnEcr(...)");
      
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

    //----------------------------------------------------------------------------
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
    
    //----------------------------------------------------------------------------
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
      
      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      med_int aTaille = 0;

      TErr aRet;
      aRet = MEDpolygoneInfo(myFile->Id(), 
			     &aMeshName, 
			     med_entite_maillage(theEntity), 
			     med_connectivite(theConnMode),
			     &aTaille);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"GetPolygoneInfo - MEDpolygoneInfo(...)");

      return TInt(aTaille);
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

      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TInt aNbElem = (TInt)theInfo.myElemNum->size();
      TValueHolder<TElemNum, med_int> anIndex(theInfo.myIndex);
      TValueHolder<TElemNum, med_int> aFaces(theInfo.myFaces);
      TValueHolder<TElemNum, med_int> aConn(theInfo.myConn);
      TValueHolder<EConnectivite, med_connectivite> aConnMode(theInfo.myConnMode);

      TErr aRet;
      aRet = MEDpolyedreConnLire(myFile->Id(), 
				 &aMeshName,
				 &anIndex,
				 aNbElem + 1,
				 &aFaces,
				 (TInt)theInfo.myFaces->size(),
				 &aConn,
				 aConnMode);

      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"GetPolygoneInfo - MEDpolyedreConnLire(...)");

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

    //----------------------------------------------------------------------------
    void
    TVWrapper
    ::SetPolyedreInfo(const TPolyedreInfo& theInfo,
		      TErr* theErr)
    {
      SetPolyedreInfo(theInfo,eLECTURE_ECRITURE,theErr);
    }
    
    //----------------------------------------------------------------------------
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

      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<TElemNum, med_int> anIndex(anInfo.myIndex);
      TValueHolder<TElemNum, med_int> aFaces(anInfo.myFaces);
      TValueHolder<TElemNum, med_int> aConn(anInfo.myConn);
      TValueHolder<EConnectivite, med_connectivite> aConnMode(anInfo.myConnMode);

      TErr aRet;
      aRet = MEDpolyedreConnEcr(myFile->Id(),
				&aMeshName,
				&anIndex,
				anInfo.myNbElem + 1,
				&aFaces,
				(TInt)anInfo.myFaces->size(),
				&aConn,
				aConnMode);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"SetPolyedreInfo - MEDpolyedreConnEcr(...)");
      
      TValueHolder<EEntiteMaillage, med_entite_maillage> anEntity(anInfo.myEntity);

      if(theInfo.myIsElemNames){
	TValueHolder<TString, char> anElemNames(anInfo.myElemNames);
	aRet  = MEDnomEcr(myFile->Id(),
			  &aMeshName,
			  &anElemNames,
			  (TInt)anInfo.myElemNames->size(),
			  anEntity,
			  MED_POLYEDRE);
	if(theErr) 
	  *theErr = aRet;
	else if(aRet < 0)
	  EXCEPTION(std::runtime_error,"SetPolyedreInfo - MEDnomEcr(...)");
      }
      
      if(theInfo.myIsElemNum){
	TValueHolder<TElemNum, med_int> anElemNum(anInfo.myElemNum);
	aRet  = MEDnumEcr(myFile->Id(),
			  &aMeshName,
			  &anElemNum,
			  (TInt)anInfo.myElemNum->size(),
			  anEntity,
			  MED_POLYEDRE);
	if(theErr) 
	  *theErr = aRet;
	else if(aRet < 0)
	  EXCEPTION(std::runtime_error,"SetPolyedreInfo - MEDnumEcr(...)");
      }
      
      
      TValueHolder<TElemNum, med_int> aFamNum(anInfo.myFamNum);
      aRet = MEDfamEcr(myFile->Id(),
		       &aMeshName,
		       &aFamNum,
		       (TInt)anInfo.myFamNum->size(),
		       anEntity,
		       MED_POLYEDRE);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"SetPolyedreInfo - MEDfamEcr(...)");
    }

    //----------------------------------------------------------------------------
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

    //----------------------------------------------------------------------------
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
	EXCEPTION(std::runtime_error,"GetPolyedreConnSize - (...)");

      MED::TMeshInfo& aMeshInfo = const_cast<MED::TMeshInfo&>(theMeshInfo);
      
      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<EConnectivite, med_connectivite> aConnMode(theConnMode);
      TValueHolder<TInt, med_int> aNbFaces(theNbFaces);
      TValueHolder<TInt, med_int> aConnSize(theConnSize);

      TErr aRet = MEDpolyedreInfo(myFile->Id(), 
				  &aMeshName, 
				  aConnMode,
				  &aNbFaces,
				  &aConnSize);

      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"GetPolygoneInfo - MEDpolyedreInfo(...)");

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

	GetGrilleType(theMeshInfo, aGrilleType);

	if(aGrilleType == eGRILLE_STANDARD){
	  TIntVector aStruct(aDim);
	  GetGrilleStruct(theMeshInfo, aStruct, theErr);
	  for(med_int i = 0; i < aDim; i++){
	    aNbNodes = aNbNodes * aStruct[i];
	    aNbElem = aNbElem * (aStruct[i] - 1);
	  }
	}else{ // eGRILLE_CARTESIENNE and eGRILLE_POLAIRE
	  ETable aTable;
	  for(med_int anAxis = 1; anAxis <= aDim; anAxis++){
	    switch(anAxis){
	    case 1 :
	      aTable = eCOOR_IND1;
	      break;
	    case 2 :
	      aTable = eCOOR_IND2;
	      break;
	    case 3 :
	      aTable = eCOOR_IND3;
	      break;
	    }
	    TInt aNbNodes = GetNbNodes(theMeshInfo, aTable);
	    aNbNodes = aNbNodes * aNbNodes;
	    aNbElem =  aNbElem * (aNbNodes - 1);
	  }
	}
	anInfo[eNOEUD][ePOINT1] = aNbNodes;
	anInfo[anEntity][aGeom] = aNbElem;
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
      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);

      return MEDnEntMaa(myFile->Id(),
			&aMeshName,
			MED_CONN,
			med_entite_maillage(theEntity),
			med_geometrie_element(theGeom),
			med_connectivite(theConnMode)); 
    }
    
    
    //----------------------------------------------------------------------------
    void
    TVWrapper
    ::GetCellInfo(MED::TCellInfo& theInfo,
		  TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);

      if(theErr && *theErr < 0)
	return;
      
      MED::TMeshInfo& aMeshInfo = *theInfo.myMeshInfo;
      TInt aNbElem = (TInt)theInfo.myElemNum->size();

      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<TInt, med_int> aDim(aMeshInfo.myDim);
      TValueHolder<TElemNum, med_int> aConn(theInfo.myConn);
      TValueHolder<EModeSwitch, med_mode_switch> aModeSwitch(theInfo.myModeSwitch);
      TValueHolder<TString, char> anElemNames(theInfo.myElemNames);
      TValueHolder<EBooleen, med_booleen> anIsElemNames(theInfo.myIsElemNames);
      TValueHolder<TElemNum, med_int> anElemNum(theInfo.myElemNum);
      TValueHolder<EBooleen, med_booleen> anIsElemNum(theInfo.myIsElemNum);
      TValueHolder<TElemNum, med_int> aFamNum(theInfo.myFamNum);
      TValueHolder<EEntiteMaillage, med_entite_maillage> anEntity(theInfo.myEntity);
      TValueHolder<EGeometrieElement, med_geometrie_element> aGeom(theInfo.myGeom);
      TValueHolder<EConnectivite, med_connectivite> aConnMode(theInfo.myConnMode);

      TErr aRet;
      aRet = MEDelementsLire(myFile->Id(),
			     &aMeshName,
			     aDim,
			     &aConn,
			     aModeSwitch,
			     &anElemNames,
			     &anIsElemNames,
			     &anElemNum,
			     &anIsElemNum,
			     &aFamNum,
			     aNbElem,
			     anEntity,
			     aGeom,
			     aConnMode);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"GetCellInfo - MEDelementsLire(...)");
    }
    
    
    //----------------------------------------------------------------------------
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

      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<TInt, med_int> aDim(aMeshInfo.myDim);
      TValueHolder<TElemNum, med_int> aConn(anInfo.myConn);
      TValueHolder<EModeSwitch, med_mode_switch> aModeSwitch(anInfo.myModeSwitch);
      TValueHolder<TString, char> anElemNames(anInfo.myElemNames);
      TValueHolder<EBooleen, med_booleen> anIsElemNames(anInfo.myIsElemNames);
      TValueHolder<TElemNum, med_int> anElemNum(anInfo.myElemNum);
      TValueHolder<EBooleen, med_booleen> anIsElemNum(anInfo.myIsElemNum);
      TValueHolder<TElemNum, med_int> aFamNum(anInfo.myFamNum);
      TValueHolder<EEntiteMaillage, med_entite_maillage> anEntity(anInfo.myEntity);
      TValueHolder<EGeometrieElement, med_geometrie_element> aGeom(anInfo.myGeom);
      TValueHolder<EConnectivite, med_connectivite> aConnMode(anInfo.myConnMode);

      TErr aRet;
      aRet = MEDelementsEcr(myFile->Id(),
			    &aMeshName,
			    aDim,
			    &aConn,
			    aModeSwitch,
			    &anElemNames,
			    anIsElemNames,
			    &anElemNum,
			    anIsElemNum,
			    &aFamNum,
			    anInfo.myNbElem,
			    anEntity,
			    aGeom,
			    aConnMode);
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"SetCellInfo - MEDelementsLire(...)");
    }
    

    //----------------------------------------------------------------------------
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
    
    
    //----------------------------------------------------------------------------
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
    
    
    //----------------------------------------------------------------------------
    void
    TVWrapper
    ::GetFieldInfo(TInt theFieldId, 
		   MED::TFieldInfo& theInfo,
		   TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TString aFieldName(256); // Protect from memory problems with too long names
      TValueHolder<ETypeChamp, med_type_champ> aType(theInfo.myType);
      TValueHolder<TString, char> aCompNames(theInfo.myCompNames);
      TValueHolder<TString, char> anUnitNames(theInfo.myUnitNames);

      TErr aRet;
      aRet = MEDchampInfo(myFile->Id(),
			  theFieldId,
			  &aFieldName[0],
			  &aType,
			  &aCompNames,
			  &anUnitNames,
			  theInfo.myNbComp);

      theInfo.SetName(aFieldName);

      if(theErr)
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"GetFieldInfo - MEDchampInfo(...)");
    }
    
    
    //----------------------------------------------------------------------------
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
      
      TValueHolder<TString, char> aFieldName(anInfo.myName);
      TValueHolder<ETypeChamp, med_type_champ> aType(anInfo.myType);
      TValueHolder<TString, char> aCompNames(anInfo.myCompNames);
      TValueHolder<TString, char> anUnitNames(anInfo.myUnitNames);

      TErr aRet;
      aRet = MEDchampCr(myFile->Id(),
			&aFieldName,
			aType,
			&aCompNames,
			&anUnitNames,
			anInfo.myNbComp);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"SetFieldInfo - MEDchampEcr(...)");
    }
    
    
    //----------------------------------------------------------------------------
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
    
    
    //----------------------------------------------------------------------------
    TInt
    TVWrapper
    ::GetNbGauss(TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return -1;
      
      return MEDnGauss(myFile->Id());
    }


    //----------------------------------------------------------------------------
    TGaussInfo::TInfo
    TVWrapper
    ::GetGaussPreInfo(TInt theId, 
		      TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);

      if(theErr && *theErr < 0)
	return TGaussInfo::TInfo( TGaussInfo::TKey(ePOINT1,""),0 );
      
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
	EXCEPTION(std::runtime_error,"GetGaussPreInfo - MEDgaussInfo(...)");
      
      return TGaussInfo::TInfo(TGaussInfo::TKey(EGeometrieElement(aGeom),&aName[0]),
			       TInt(aNbGaussPoints));
    }


    //----------------------------------------------------------------------------
    void
    TVWrapper
    ::GetGaussInfo(TInt theId, 
		   TGaussInfo& theInfo,
		   TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TValueHolder<TNodeCoord, med_float> aRefCoord(theInfo.myRefCoord);
      TValueHolder<TNodeCoord, med_float> aGaussCoord(theInfo.myGaussCoord);
      TValueHolder<TWeight, med_float> aWeight(theInfo.myWeight);
      TValueHolder<EModeSwitch, med_mode_switch> aModeSwitch(theInfo.myModeSwitch);
      TValueHolder<TString, char> aGaussName(theInfo.myName);

      TErr aRet;
      aRet = MEDgaussLire(myFile->Id(),
			  &aRefCoord,
			  &aGaussCoord,
			  &aWeight,
			  aModeSwitch,
			  &aGaussName);

      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"GetGaussInfo - MEDgaussLire(...)");
    }


    //----------------------------------------------------------------------------
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
      
      med_int aSize = -1;
      TVector<char> aName(GetNOMLength<eV2_2>()+1);

      TErr aRet;
      aRet = MEDprofilInfo(myFile->Id(),
			   theId,
			   &aName[0],
			   &aSize);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"GetProfilePreInfo - MEDprofilInfo(...)");
      
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
      
      TProfileInfo& anInfo = const_cast<TProfileInfo&>(theInfo);
      TValueHolder<TElemNum, med_int> anElemNum(anInfo.myElemNum);
      TValueHolder<TString, char>     aProfileName(anInfo.myName);

      TErr aRet;
      aRet = MEDprofilLire(myFile->Id(),
			   &anElemNum,
			   &aProfileName);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"GetProfileInfo - MEDprofilLire(...)");
    }

    void
    TVWrapper
    ::SetProfileInfo(const TProfileInfo& theInfo,
                     EModeAcces          theMode,
		     TErr*               theErr)
    {
      TFileWrapper aFileWrapper(myFile,theMode,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TProfileInfo& anInfo = const_cast<TProfileInfo&>(theInfo);
      TValueHolder<TElemNum, med_int> anElemNum(anInfo.myElemNum);
      TValueHolder<TString, char>     aProfileName(anInfo.myName);

      TErr aRet;
      aRet = MEDprofilEcr(myFile->Id(),      // descripteur du fichier.
                          &anElemNum,        // tableau de valeurs du profil.
                          theInfo.GetSize(), // taille du profil.
                          &aProfileName);    // nom profil.
      if(theErr)
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"SetProfileInfo - MEDprofilEcr(...)");
    }

    void
    TVWrapper
    ::SetProfileInfo(const TProfileInfo& theInfo,
		     TErr*               theErr)
    {
      TErr aRet;
      SetProfileInfo(theInfo,eLECTURE_ECRITURE,&aRet);
      
      if(aRet < 0)
	SetProfileInfo(theInfo,eLECTURE_AJOUT,&aRet);

      if(aRet < 0)
	SetProfileInfo(theInfo,eCREATION,&aRet);

      if(theErr)
        *theErr = aRet;
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
	EXCEPTION(std::runtime_error,"GetNbTimeStamps - There is no any Entity on the Mesh");
      
      bool anIsPerformAdditionalCheck = GetNbMeshes() > 1;

      theGeom2Size.clear();
      TInt aNbTimeStamps = 0;
      TIdt anId = myFile->Id();

      MED::TFieldInfo& anInfo = const_cast<MED::TFieldInfo&>(theInfo);
      TValueHolder<TString, char> aFieldName(anInfo.myName);
      MED::TMeshInfo& aMeshInfo = anInfo.myMeshInfo;

      MED::TEntityInfo localEntityInfo = theEntityInfo;
      TEntityInfo::iterator anLocalIter = localEntityInfo.find(eMAILLE);
      if(anLocalIter != localEntityInfo.end()){
        localEntityInfo[eNOEUD_ELEMENT] = anLocalIter->second;
      }
        
      TEntityInfo::const_iterator anIter = localEntityInfo.begin();
      for(; anIter != localEntityInfo.end(); anIter++){
	med_entite_maillage anEntity = med_entite_maillage(anIter->first);
	const TGeom2Size& aGeom2Size = anIter->second;
	TGeom2Size::const_iterator anGeomIter = aGeom2Size.begin();
	for(; anGeomIter != aGeom2Size.end(); anGeomIter++){
	  med_geometrie_element aGeom = med_geometrie_element(anGeomIter->first);
	  TInt aNbStamps = MEDnPasdetemps(anId,
					  &aFieldName,
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
					      &aFieldName,
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
    
    
    //----------------------------------------------------------------------------
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
	EXCEPTION(std::runtime_error,"GetTimeStampInfo - There is no any cell");
      
      MED::TFieldInfo& aFieldInfo = *theInfo.myFieldInfo;
      MED::TMeshInfo& aMeshInfo = *aFieldInfo.myMeshInfo;
      
      TValueHolder<TString, char> aFieldName(aFieldInfo.myName);
      TValueHolder<EEntiteMaillage, med_entite_maillage> anEntity(theInfo.myEntity);
      TValueHolder<TInt, med_int> aNumDt(theInfo.myNumDt);
      TValueHolder<TInt, med_int> aNumOrd(theInfo.myNumOrd);
      TValueHolder<TString, char> anUnitDt(theInfo.myUnitDt);
      TValueHolder<TFloat, med_float> aDt(theInfo.myDt);
      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<EBooleen, med_booleen> anIsLocal(aFieldInfo.myIsLocal);
      TValueHolder<TInt, med_int> aNbRef(aFieldInfo.myNbRef);

      TGeom2NbGauss& aGeom2NbGauss = theInfo.myGeom2NbGauss;

      TGeom2Size::const_iterator anIter = aGeom2Size.begin();
      for(; anIter != aGeom2Size.end(); anIter++){
	const EGeometrieElement& aGeom = anIter->first;
	med_int aNbGauss = -1;

	TErr aRet;
	aRet = MEDpasdetempsInfo(myFile->Id(),
				 &aFieldName,
				 anEntity,
				 med_geometrie_element(aGeom),
				 theTimeStampId,
				 &aNbGauss,
				 &aNumDt,
				 &aNumOrd,
				 &anUnitDt,
				 &aDt,
				 &aMeshName,
				 &anIsLocal,
				 &aNbRef);

	static TInt MAX_NB_GAUSS_POINTS = 32;
	if(aNbGauss <= 0 || aNbGauss > MAX_NB_GAUSS_POINTS)
	  aNbGauss = 1;

	aGeom2NbGauss[aGeom] = aNbGauss;

	if(theErr) 
	  *theErr = aRet;
	else if(aRet < 0)
	  EXCEPTION(std::runtime_error,"GetTimeStampInfo - MEDpasdetempsInfo(...)");
      }      
    }
    

    //----------------------------------------------------------------------------
    void 
    TVWrapper
    ::GetTimeStampValue(const PTimeStampValueBase& theTimeStampValue,
			const TMKey2Profile& theMKey2Profile,
			const TKey2Gauss& theKey2Gauss,
			TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TIdt anId = myFile->Id();
      
      TValueHolder<EModeSwitch, med_mode_switch> aModeSwitch(theTimeStampValue->myModeSwitch);
      MED::TGeom2Profile& aGeom2Profile = theTimeStampValue->myGeom2Profile;

      MED::PTimeStampInfo aTimeStampInfo = theTimeStampValue->myTimeStampInfo;
      TValueHolder<EEntiteMaillage, med_entite_maillage> anEntity(aTimeStampInfo->myEntity);
      TValueHolder<TInt, med_int> aNumDt(aTimeStampInfo->myNumDt);
      TValueHolder<TInt, med_int> aNumOrd(aTimeStampInfo->myNumOrd);

      MED::PFieldInfo aFieldInfo = aTimeStampInfo->myFieldInfo;
      TValueHolder<TString, char> aFieldName(aFieldInfo->myName);
      TValueHolder<EBooleen, med_booleen> anIsLocal(aFieldInfo->myIsLocal);

      MED::PMeshInfo aMeshInfo = aFieldInfo->myMeshInfo;
      TValueHolder<TString, char> aMeshName(aMeshInfo->myName);
      
      TGeom2Gauss& aGeom2Gauss = aTimeStampInfo->myGeom2Gauss;
      TVector<char> aGaussName(GetNOMLength<eV2_2>()+1);

      med_mode_profil aProfileMode = med_mode_profil(boost::get<0>(theMKey2Profile));
      MED::TKey2Profile aKey2Profile = boost::get<1>(theMKey2Profile);
      TVector<char> aProfileName(GetNOMLength<eV2_2>()+1);

      TGeom2Size& aGeom2Size = aTimeStampInfo->myGeom2Size;
      TGeom2Size::iterator anIter = aGeom2Size.begin();
      for(; anIter != aGeom2Size.end(); anIter++){
	EGeometrieElement aGeom = anIter->first;
	TInt aNbElem = anIter->second;

	TInt aNbMeshRef = MEDnChampRef(anId,
				       &aFieldName,
				       anEntity,
				       med_geometrie_element(aGeom),
				       aNumDt,
				       aNumOrd);
	if(aNbMeshRef < 1){
	  if(theErr){
	    *theErr = MED_FAUX;
	    return;
	  }
	  EXCEPTION(std::runtime_error,"GetTimeStampValue - MEDnChampRef(...) < 1");
	}
	
	TErr aRet;
	med_int aNbGauss = -1;
	aRet = MEDchampRefInfo(anId,
			       &aFieldName,
			       anEntity,
			       med_geometrie_element(aGeom),
			       aNbMeshRef,
			       aNumDt,
			       aNumOrd, 
			       &aMeshName,
			       &anIsLocal,
			       &aNbGauss);

	if(aRet < 0){
	  if(theErr){
	    *theErr = MED_FAUX;
	    return;
	  }
	  EXCEPTION(std::runtime_error,"GetTimeStampValue - MEDchampRefInfo(...)");
	}
 
	TInt aNbVal = MEDnVal(anId,
			      &aFieldName,
			      anEntity,
			      med_geometrie_element(aGeom),
			      aNumDt,
			      aNumOrd,
			      &aMeshName,
			      aProfileMode);
	if(aNbVal <= 0){
	  if(theErr){
	    *theErr = -1;
	    return;
	  }
	  EXCEPTION(std::runtime_error,"GetTimeStampValue - MEDnVal(...) - aNbVal == "<<aNbVal<<" <= 0");
	}
	
	TInt aNbComp = aFieldInfo->myNbComp;
	TInt aNbValue = aNbVal / aNbGauss;
	theTimeStampValue->AllocateValue(aGeom,
					 aNbValue,
					 aNbGauss,
					 aNbComp);
	TInt aValueSize = theTimeStampValue->GetValueSize(aGeom);

	INITMSG(MYDEBUG,
		"TVWrapper::GetTimeStampValue - aGeom = "<<aGeom<<
		"; aNbVal = "<<aNbVal<<
		"; aNbValue = "<<aNbValue<<
		"; aNbGauss = "<<aNbGauss<<
		"; aNbComp = "<<aNbComp<<
		std::endl);
	
	aRet = MEDchampLire(anId,
			    &aMeshName,
			    &aFieldName,
			    theTimeStampValue->GetValuePtr(aGeom),
			    aModeSwitch,
			    MED_ALL,
			    &aGaussName[0],
			    &aProfileName[0],
			    aProfileMode,
			    anEntity,
			    med_geometrie_element(aGeom),
			    aNumDt,
			    aNumOrd);
	if(aRet < 0){
	  if(theErr){
	    *theErr = MED_FAUX;
	    return;
	  }
	  EXCEPTION(std::runtime_error,"GetTimeStampValue - MEDchampLire(...)");
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

	if(aGaussInfo && aNbGauss != aGaussInfo->GetNbGauss()){
	  if(theErr){
	    *theErr = MED_FAUX;
	    return;
	  }
	  EXCEPTION(std::runtime_error,"GetTimeStampValue - aNbGauss != aGaussInfo->GetNbGauss()");
	}
	
	if(aProfileInfo && aProfileInfo->IsPresent()){
	  TInt aNbSubElem = aProfileInfo->GetSize();
	  TInt aProfileSize = aNbSubElem*aNbComp*aNbGauss;
	  if(aProfileSize != aValueSize){
	    if(theErr){
	      *theErr = -1;
	      return;
	    }
	    EXCEPTION(std::runtime_error,
		      "GetTimeStampValue - aProfileSize("<<aProfileSize<<
		      ") != aValueSize("<<aValueSize<<
		      "); aNbVal = "<<aNbVal<<
		      "; anEntity = "<<anEntity<<
		      "; aGeom = "<<aGeom<<
		      "; aNbElem = "<<aNbElem<<
		      "; aNbSubElem = "<<aNbSubElem<<
		      "; aNbComp = "<<aNbComp<<
		      "; aNbGauss = "<<aNbGauss<<
		      "");
	  }
	}else{
	  if(aNbElem != aNbValue){
	    if(theErr){
	      *theErr = -1;
	      return;
	    }
	    EXCEPTION(std::runtime_error,
		      "GetTimeStampValue - aNbElem("<<aNbElem<<
		      ") != aNbValue("<<aNbValue<<
		      "); aNbVal = "<<aNbVal<<
		      "; anEntity = "<<anEntity<<
		      "; aGeom = "<<aGeom<<
		      "; aNbElem = "<<aNbElem<<
		      "; aNbComp = "<<aNbComp<<
		      "; aNbGauss = "<<aNbGauss<<
		      "");
	  }
	}
      }
    }
    
    
    //----------------------------------------------------------------------------
    void
    TVWrapper
    ::SetTimeStampValue(const MED::PTimeStampValueBase& theTimeStampValue,
			EModeAcces theMode,
			TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,theMode,theErr);
      
      if(theErr && *theErr < 0)
	return;
      
      TErr aRet;
      TIdt anId = myFile->Id();
      
      TValueHolder<EModeSwitch, med_mode_switch> aModeSwitch(theTimeStampValue->myModeSwitch);
      MED::TGeom2Profile& aGeom2Profile = theTimeStampValue->myGeom2Profile;

      MED::PTimeStampInfo aTimeStampInfo = theTimeStampValue->myTimeStampInfo;
      TValueHolder<EEntiteMaillage, med_entite_maillage> anEntity(aTimeStampInfo->myEntity);
      TValueHolder<TInt, med_int> aNumDt(aTimeStampInfo->myNumDt);
      TValueHolder<TInt, med_int> aNumOrd(aTimeStampInfo->myNumOrd);
      TValueHolder<TString, char> anUnitDt(aTimeStampInfo->myUnitDt);
      TValueHolder<TFloat, med_float> aDt(aTimeStampInfo->myDt);
      MED::TGeom2Gauss& aGeom2Gauss = aTimeStampInfo->myGeom2Gauss;

      MED::PFieldInfo aFieldInfo = aTimeStampInfo->myFieldInfo;
      TValueHolder<TString, char> aFieldName(aFieldInfo->myName);

      MED::PMeshInfo aMeshInfo = aFieldInfo->myMeshInfo;
      TValueHolder<TString, char> aMeshName(aMeshInfo->myName);
      
      const TGeomSet& aGeomSet = theTimeStampValue->myGeomSet;
      TGeomSet::const_iterator anIter = aGeomSet.begin();
      for(; anIter != aGeomSet.end(); anIter++){
	EGeometrieElement aGeom = *anIter;

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

	med_int aNbVal = theTimeStampValue->GetNbVal(aGeom);

	aRet = MEDchampEcr(anId,
			   &aMeshName,
			   &aFieldName,
			   theTimeStampValue->GetValuePtr(aGeom),
			   aModeSwitch,
			   aNbVal,
			   &aGaussName[0],
			   MED_ALL,
			   &aProfileName[0],
			   aProfileMode,
			   anEntity,
			   med_geometrie_element(aGeom),
			   aNumDt,
			   &anUnitDt,
			   aDt,
			   aNumOrd);	
	if(aRet < 0){
	  if(theErr){
	    *theErr = MED_FAUX;
	    break;
	  }
	  EXCEPTION(std::runtime_error,"SetTimeStampValue - MEDchampEcr(...)");
	}
	
      }
      
      INITMSG(MYDEBUG,"TVWrapper::SetTimeStampValue - MED_MODE_ACCES = "<<theMode<<"; aRet = "<<aRet<<std::endl);
    }

    
    //----------------------------------------------------------------------------
    void 
    TVWrapper
    ::SetTimeStampValue(const PTimeStampValueBase& theTimeStampValue,
			TErr* theErr)
    {
      TErr aRet;
      SetTimeStampValue(theTimeStampValue,eLECTURE_ECRITURE,&aRet);
      
      if(aRet < 0)
	SetTimeStampValue(theTimeStampValue,eLECTURE_AJOUT,&aRet);

      if(theErr) 
	*theErr = aRet;
    }
    
    //----------------------------------------------------------------------------
    void 
    TVWrapper
    ::SetGrilleInfo(const MED::TGrilleInfo& theInfo,
		    TErr* theErr)
    {
      SetGrilleInfo(theInfo,eLECTURE_ECRITURE,theErr);
    }

    //----------------------------------------------------------------------------
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
      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<TInt, med_int> aDim(aMeshInfo.myDim);

      TValueHolder<EGrilleType, med_type_grille> aGrilleType(anInfo.myGrilleType);

      TErr aRet = 0;
      aRet = MEDnatureGrilleEcr(myFile->Id(),
				&aMeshName,
				aGrilleType);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"SetGrilleInfo - MEDnatureGrilleEcr(...)");
      
      if(anInfo.myGrilleType == eGRILLE_STANDARD){
	TValueHolder<TNodeCoord, med_float> aCoord(anInfo.myCoord);
	TValueHolder<EModeSwitch, med_mode_switch> aModeSwitch(anInfo.myModeSwitch);
	TValueHolder<TString, char> aCoordNames(anInfo.myCoordNames);
	TValueHolder<TString, char> aCoordUnits(anInfo.myCoordUnits);
	med_int aNbNoeuds = med_int(anInfo.myCoord.size() / aMeshInfo.myDim);
	med_repere aRepere = MED_CART;

	aRet = MEDcoordEcr(myFile->Id(),
			   &aMeshName,
			   aDim,
			   &aCoord,
			   aModeSwitch,
			   aNbNoeuds,
			   aRepere,
			   &aCoordNames,
			   &aCoordUnits);

	if(aRet < 0)
	  EXCEPTION(std::runtime_error,"SetGrilleInfo - MEDcoordEcr(...)");

	TValueHolder<TIntVector, med_int> aGrilleStructure(anInfo.myGrilleStructure);
	aRet = MEDstructureCoordEcr(myFile->Id(),
				    &aMeshName,
				    aDim,
				    &aGrilleStructure);
	if(aRet < 0)
	  EXCEPTION(std::runtime_error,"SetGrilleInfo - MEDstructureCoordEcr(...)");
	
      } else {
	for(med_int aAxis = 0; aAxis < aMeshInfo.myDim; aAxis++){
	  aRet = MEDindicesCoordEcr(myFile->Id(),
				    &aMeshName,
				    aDim,
				    &anInfo.GetIndexes(aAxis)[0],
				    anInfo.GetIndexes(aAxis).size(),
				    aAxis + 1,
				    &anInfo.GetCoordName(aAxis)[0],
				    &anInfo.GetCoordUnit(aAxis)[0]);
	  if(aRet < 0)
	    EXCEPTION(std::runtime_error,"SetGrilleInfo - MEDindicesCoordEcr(...)");
	}
	
      }

      return;
    }

    //----------------------------------------------------------------------------
    void
    TVWrapper
    ::GetGrilleInfo(TGrilleInfo& theInfo,
		    TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);

      if(theErr && *theErr < 0)
	  return;
      
      MED::TMeshInfo& aMeshInfo = *theInfo.myMeshInfo;
      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<TInt, med_int> aDim(aMeshInfo.myDim);
      EMaillage aMaillageType = aMeshInfo.myType;
      
      GetGrilleType(aMeshInfo, theInfo.myGrilleType, theErr);
      EGrilleType aGrilleType = theInfo.myGrilleType;

      TErr aRet = 0;
      if(aMaillageType == eSTRUCTURE && aGrilleType == eGRILLE_STANDARD){
	GetGrilleStruct(aMeshInfo, theInfo.myGrilleStructure, theErr);

	TValueHolder<TNodeCoord, med_float> aCoord(theInfo.myCoord);
	TValueHolder<EModeSwitch, med_mode_switch> aModeSwitch(theInfo.myModeSwitch);
	TValueHolder<TString, char> aCoordNames(theInfo.myCoordNames);
	TValueHolder<TString, char> aCoordUnits(theInfo.myCoordUnits);
	med_repere aRepere;

	aRet = MEDcoordLire(myFile->Id(),
			    &aMeshName,
			    aDim,
			    &aCoord,
			    aModeSwitch,
			    MED_ALL, // all coordinates must be return
			    NULL,
			    0,
			    &aRepere,
			    &aCoordNames,
			    &aCoordUnits);

	if(theErr) 
	  *theErr = aRet;
	else if(aRet < 0)
	  EXCEPTION(std::runtime_error,"GetGrilleInfo - MEDcoordLire(...)");

	TInt aNbNodes = theInfo.GetNbNodes();//GetNbFamilies(aMeshInfo);
	TValueHolder<TElemNum, med_int> aFamNumNode(theInfo.myFamNumNode);
	
	aRet = MEDfamLire(myFile->Id(),
			  &aMeshName,
			  &aFamNumNode,
			  aNbNodes,
			  med_entite_maillage(eNOEUD),
			  med_geometrie_element(ePOINT1));

	if(theErr) 
	  *theErr = aRet;
	else if(aRet < 0)
	  EXCEPTION(std::runtime_error,"GetGrilleInfo - MEDfamLire(...) of NODES");

	//============================
      }

      if(aMaillageType == eSTRUCTURE && aGrilleType != eGRILLE_STANDARD){
	ETable aTable;
	for(med_int anAxis = 1; anAxis <= aMeshInfo.myDim; anAxis++){
	  switch(anAxis){
	  case 1 :
	    aTable = eCOOR_IND1;
	    break;
	  case 2 :
	    aTable = eCOOR_IND2;
	    break;
	  case 3 :
	    aTable = eCOOR_IND3;
	    break;
	  default :
	    aRet = -1;
	  }
	    
	  if(theErr) 
	    *theErr = aRet;
	  else if(aRet < 0)
	    EXCEPTION(std::runtime_error,"GetGrilleInfo - anAxis number out of range(...)");
	  
	  TInt aNbIndexes = GetNbNodes(aMeshInfo,aTable);
	  if(aNbIndexes < 0)
	    EXCEPTION(std::runtime_error,"GetGrilleInfo - Erreur a la lecture de la taille de l'indice");
	    
	  TValueHolder<TFloatVector, med_float> anIndexes(theInfo.GetIndexes(anAxis-1));
	  char aCompNames[MED_TAILLE_PNOM+1];
	  char anUnitNames[MED_TAILLE_PNOM+1];
	    
	  aRet = MEDindicesCoordLire(myFile->Id(),
				     &aMeshName,
				     aDim,
				     &anIndexes,
				     aNbIndexes,
				     anAxis,
				     aCompNames,
				     anUnitNames);

	  theInfo.SetCoordName(anAxis-1, aCompNames);
	  theInfo.SetCoordUnit(anAxis-1, anUnitNames);

	  if(theErr) 
	    *theErr = aRet;
	  else if(aRet < 0)
	    EXCEPTION(std::runtime_error,"GetGrilleInfo - MEDindicesCoordLire(...)");
	}
      }

      EGeometrieElement aGeom = theInfo.GetGeom();
      EEntiteMaillage aEntity = theInfo.GetEntity();
      TInt aNbCells = theInfo.GetNbCells();
      
      theInfo.myFamNum.resize(aNbCells);
      TValueHolder<TElemNum, med_int> aFamNum(theInfo.myFamNum);
      
      aRet = MEDfamLire(myFile->Id(),
			&aMeshName,
			&aFamNum,
			aNbCells,
			med_entite_maillage(aEntity),
			med_geometrie_element(aGeom));
      
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0){
	//EXCEPTION(std::runtime_error,"GetGrilleInfo - MEDfamLire(...) of CELLS");
	aRet = 0;
	theInfo.myFamNum.resize(0);
      }
      
    }

    void
    TVWrapper
    ::GetGrilleType(const MED::TMeshInfo& theMeshInfo,
		    EGrilleType& theGrilleType,
		    TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECTURE,theErr);

      if(theErr && *theErr < 0)
 	EXCEPTION(std::runtime_error," GetGrilleType - aFileWrapper (...)");

      MED::TMeshInfo& aMeshInfo = const_cast<MED::TMeshInfo&>(theMeshInfo);
      
      if(aMeshInfo.myType == eSTRUCTURE){
	TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
	TValueHolder<EGrilleType, med_type_grille> aGrilleType(theGrilleType);

	TErr aRet = MEDnatureGrilleLire(myFile->Id(),
					&aMeshName,
					&aGrilleType);
	if(aRet < 0)
	  EXCEPTION(std::runtime_error,"GetGrilleInfo - MEDnatureGrilleLire(...)");
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

      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      TValueHolder<TInt, med_int> aDim(aMeshInfo.myDim);
      TValueHolder<TIntVector, med_int> aGrilleStructure(theStruct);
	
      aRet = MEDstructureCoordLire(myFile->Id(),
				   &aMeshName,
				   aDim,
				   &aGrilleStructure);
      if(theErr) 
	*theErr = aRet;
      else if(aRet < 0)
	EXCEPTION(std::runtime_error,"GetGrilleInfo - MEDstructureCoordLire(...)");
    }

  }

  bool getMEDVersion( const std::string& fname, int& major, int& minor, int& release )
  {
    med_idt f = MEDouvrir( (char*)fname.c_str(), MED_LECTURE );
    if( f<0 )
      return false;

    med_int aMajor, aMinor, aRelease;
    med_err aRet = MEDversionLire( f, &aMajor, &aMinor, &aRelease );
    major = aMajor;
    minor = aMinor;
    release = aRelease;
    MEDfermer( f );
    if( aRet<0 )
      return false;
    return true;
  }
}
