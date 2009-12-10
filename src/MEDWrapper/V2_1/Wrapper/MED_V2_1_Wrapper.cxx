//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : 
//  Author : 
//  Module : 
//  $Header$
//
#include "MED_V2_1_Wrapper.hxx"
#include "MED_Algorithm.hxx"
#include "MED_Utilities.hxx"

#include "med.hxx"
using namespace med_2_1;

#ifdef _DEBUG_
static int MYDEBUG = 0;
#else
// static int MYDEBUG = 0;
#endif



namespace MED
{

  template<>
  TInt
  GetDESCLength<eV2_1>()
  {
    return 200;
  }

  template<>
  TInt
  GetIDENTLength<eV2_1>()
  {
    return 8;
  }

  template<>
  TInt
  GetLNOMLength<eV2_1>()
  {
    return 80;
  }

  template<>
  TInt
  GetNOMLength<eV2_1>()
  {
    return 32;
  }

  template<>
  TInt
  GetPNOMLength<eV2_1>()
  {
    return 8;
  }

  template<>
  void
  GetVersionRelease<eV2_1>(TInt& majeur, TInt& mineur, TInt& release)
  {
    MEDversionDonner(&majeur, &mineur, &release);
  }

  template<>
  TInt
  GetNbConn<eV2_1>(EGeometrieElement typmai,
                   EEntiteMaillage typent,
                   TInt mdim)
  {
    TInt nsup = 0;

    if(typent == eMAILLE){
      TInt edim = typmai / 100;
      if(mdim  == 2 || mdim == 3)
        if(edim == 1)
          nsup = 1;
      
      if(mdim == 3)
        if (edim == 2)
          nsup = 1;
    }

    return nsup + typmai%100;
  }

  namespace V2_1
  {

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
        if(theErr){
          *theErr = TErr(myFid);
          INITMSG(MYDEBUG && myFid < 0,"TFile::Open - MED_MODE_ACCES = "<<theMode<<"; myFid = "<<myFid<<std::endl);
        }else if(myFid < 0)
          EXCEPTION(std::runtime_error, "TFile - MEDouvrir('"<<myFileName<<"',"<<theMode<<")");
      }

      const TIdt& Id() const 
      { 
        if(myFid < 0)
          EXCEPTION(std::runtime_error, "TFile - GetFid() < 0");
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
      
      TValueHolder<TString, char> aMeshName(theInfo.myName);
      TValueHolder<TInt, med_int> aDim(theInfo.myDim);

      TErr aRet = MEDmaaInfo(myFile->Id(),
                             theMeshId,
                             &aMeshName,
                             &aDim);
      if(theErr) 
        *theErr = aRet;
      else if(aRet < 0)
        EXCEPTION(std::runtime_error,"GetMeshInfo - MEDmaaInfo(...)");
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
      TValueHolder<TString, char> aMeshName(anInfo.myName);
      TValueHolder<TInt, med_int> aDim(anInfo.myDim);
      
      TErr aRet = MEDmaaCr(myFile->Id(),
                           &aMeshName,
                           aDim);
      
      if(theErr) 
        *theErr = aRet;
      else if(aRet < 0)
        EXCEPTION(std::runtime_error,"SetMeshInfo - MEDmaaCr(...)");
    }
    
    
    void
    TVWrapper
    ::SetMeshInfo(const TMeshInfo& theInfo,
                  TErr* theErr)
    {
      TErr aRet;
      SetMeshInfo(theInfo,eECRI,&aRet);
      
      if(aRet < 0)
        SetMeshInfo(theInfo,eREMP,&aRet);

      if(theErr) 
        *theErr = aRet;
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
      TValueHolder<TString, char> aMeshName(anInfo.myName);

      return MEDnFam(myFile->Id(),
                     &aMeshName,
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
      TValueHolder<TString, char> aMeshName(anInfo.myName);

      return MEDnFam(myFile->Id(),
                     &aMeshName,
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
      TValueHolder<TString, char> aMeshName(anInfo.myName);

      return MEDnFam(myFile->Id(),
                     &aMeshName,
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
        EXCEPTION(std::runtime_error,"GetFamilyInfo - MEDfamInfo - "<<
                  "&aMeshInfo.myName[0] = '"<<&aMeshName<<"'; "<<
                  "theFamId = "<<theFamId<<"; "<<
                  "&theInfo.myName[0] = '"<<&aFamilyName<<"'; "<<
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
      
      INITMSG(MYDEBUG && aRet,"TVWrapper::SetFamilyInfo - MED_MODE_ACCES = "<<theMode<<"; aRet = "<<aRet<<std::endl);
      
      if(theErr) 
        *theErr = aRet;
      else if(aRet < 0)
        EXCEPTION(std::runtime_error,"SetFamilyInfo - MEDfamCr(...)");
    }
    
    
    void 
    TVWrapper
    ::SetFamilyInfo(const TFamilyInfo& theInfo,
                    TErr* theErr)
    {
      TErr aRet;
      SetFamilyInfo(theInfo,eECRI,&aRet);
      
      if(aRet < 0)
        SetFamilyInfo(theInfo,eREMP,&aRet);

      if(theErr) 
        *theErr = aRet;
    }
    
    
    TInt
    TVWrapper
    ::GetNbNodes(const TMeshInfo& theMeshInfo,
                 TErr* theErr)
    {
      MSG(MYDEBUG,"TVWrapper::GetNbNodes");
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
        return -1;
      
      TMeshInfo& aMeshInfo = const_cast<TMeshInfo&>(theMeshInfo);
      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      
      TInt aRet = MEDnEntMaa(myFile->Id(),
                             &aMeshName,
                             MED_COOR,
                             MED_NOEUD,
                             med_geometrie_element(0),
                             med_connectivite(0));
      return aRet;
    }
    
    
    void 
    TVWrapper
    ::GetNodeInfo(TNodeInfo& theInfo,
                  TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
        return;
      
      TMeshInfo& aMeshInfo = *theInfo.myMeshInfo;

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
                               aNbElem,
                               MED_REMP);
      if(theErr) 
        *theErr = aRet;
      else if(aRet < 0)
        EXCEPTION(std::runtime_error,"SetNodeInfo - MEDnoeudsEcr(...)");
    }
    
    
    void 
    TVWrapper
    ::SetNodeInfo(const MED::TNodeInfo& theInfo,
                  TErr* theErr)
    {
      TErr aRet;
      SetNodeInfo(theInfo,eECRI,&aRet);
      
      if(aRet < 0)
        SetNodeInfo(theInfo,eREMP,&aRet);

      if(theErr) 
        *theErr = aRet;
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
      TValueHolder<TString, char> aMeshName(aMeshInfo.myName);
      
      return MEDnEntMaa(myFile->Id(),
                        &aMeshName,
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
      TInt aNbElem = theInfo.myElemNum->size();

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
                            aConnMode,
                            MED_REMP);
      
      if(theErr) 
        *theErr = aRet;
      else if(aRet < 0)
        EXCEPTION(std::runtime_error,"GetCellInfo - MEDelementsLire(...)");
    }
    

    void
    TVWrapper
    ::SetCellInfo(const MED::TCellInfo& theInfo,
                  TErr* theErr)
    {
      TErr aRet;
      SetCellInfo(theInfo,eECRI,&aRet);
      
      if(aRet < 0)
        SetCellInfo(theInfo,eREMP,&aRet);

      if(theErr) 
        *theErr = aRet;
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
        EXCEPTION(std::runtime_error,"SetFieldInfo - MEDchampCr(...)");
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
          SetFieldInfo(theInfo,eREMP,&aRet);

        if(theErr) 
          *theErr = aRet;

      }catch(const std::exception& theExc){
        EXCEPTION(std::runtime_error,"SetFieldInfo(...)"<<std::endl<<
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
      
      med_int aSize = -1;
      TVector<char> aName(GetNOMLength<eV2_1>()+1);

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
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr && *theErr < 0)
        return;
      
      TValueHolder<TElemNum, med_int> anElemNum(theInfo.myElemNum);
      TValueHolder<TString, char> aProfileName(theInfo.myName);

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
                     TErr* theErr)
    {
      TErr aRet;
      SetProfileInfo(theInfo,eECRI,&aRet);
      
      if(aRet < 0)
        SetProfileInfo(theInfo,eREMP,&aRet);

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
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
      if(theErr){
        if(theEntityInfo.empty())
          *theErr = -1;
        if(*theErr < 0)
          return -1;
      }else if(theEntityInfo.empty()) 
        EXCEPTION(std::runtime_error,"GetNbTimeStamps - There is no any Entity on the Mesh");

      bool anIsPerformAdditionalCheck = GetNbMeshes() > 1;
#ifdef _DEBUG_
      static bool anIsCheckOnlyFirstTimeStamp = false;
#else
      static bool anIsCheckOnlyFirstTimeStamp = true;
#endif

      theGeom2Size.clear();
      TInt aNbTimeStamps = 0;
      TIdt anId = myFile->Id();

      MED::TFieldInfo& anInfo = const_cast<MED::TFieldInfo&>(theInfo);
      TValueHolder<TString, char> aFieldName(anInfo.myName);
      MED::TMeshInfo& aMeshInfo = anInfo.myMeshInfo;

      TEntityInfo::const_iterator anIter = theEntityInfo.begin();
      for(; anIter != theEntityInfo.end(); anIter++){
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
              TInt iTimeStampEnd = anIsCheckOnlyFirstTimeStamp? 1: aNbStamps;
              for(TInt iTimeStamp = 1; iTimeStamp <= iTimeStampEnd; iTimeStamp++){
                TVector<char> aMeshName(GetNOMLength<eV2_1>()+1);
                TVector<char> aDtUnit(GetPNOMLength<eV2_1>()+1);
                med_int aNbGauss;
                med_int aNumDt;
                med_int aNumOrd;
                med_float aDt;
                TErr aRet = MEDpasdetempsInfo(anId,
                                              &aFieldName,
                                              anEntity,
                                              aGeom,
                                              iTimeStamp, 
                                              &aMeshName[0],
                                              &aNbGauss,
                                              &aNumDt,  
                                              &aDtUnit[0],
                                              &aDt, 
                                              &aNumOrd);
                
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
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
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
                                 &aMeshName,
                                 &aNbGauss,
                                 &aNumDt,
                                 &anUnitDt,
                                 &aDt,
                                 &aNumOrd);
        
        
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
    

    void 
    TVWrapper
    ::GetTimeStampValue(const PTimeStampValueBase& theTimeStampValue,
                        const TMKey2Profile& theMKey2Profile,
                        const TKey2Gauss& theKey2Gauss,
                        TErr* theErr)
    {
      TFileWrapper aFileWrapper(myFile,eLECT,theErr);
      
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

      MED::PMeshInfo aMeshInfo = aFieldInfo->myMeshInfo;
      TValueHolder<TString, char> aMeshName(aMeshInfo->myName);
      
      MED::TKey2Profile aKey2Profile = boost::get<1>(theMKey2Profile);
      TVector<char> aProfileName(GetNOMLength<eV2_1>()+1);

      TGeom2Size& aGeom2Size = aTimeStampInfo->myGeom2Size;
      TGeom2Size::iterator anIter = aGeom2Size.begin();
      for(; anIter != aGeom2Size.end(); anIter++){
        EGeometrieElement aGeom = anIter->first;
        TInt aNbElem = anIter->second;

        TInt aNbVal = MEDnVal(anId,
                              &aFieldName,
                              anEntity,
                              med_geometrie_element(aGeom),
                              aNumDt,
                              aNumOrd);
        if(aNbVal <= 0){
          if(theErr){
            *theErr = -1;
            return;
          }
          EXCEPTION(std::runtime_error,"GetTimeStampValue - MEDnVal(...) - aNbVal == "<<aNbVal<<" <= 0");
        }
        
        TInt aNbGauss = aTimeStampInfo->GetNbGauss(aGeom);
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

        TErr aRet = MEDchampLire(anId,
                                 &aMeshName,
                                 &aFieldName,
                                 theTimeStampValue->GetValuePtr(aGeom),
                                 aModeSwitch,
                                 MED_ALL,
                                 &aProfileName[0],
                                 anEntity,
                                 med_geometrie_element(aGeom),
                                 aNumDt,
                                 aNumOrd);
        if(aRet < 0){
          if(theErr){
            *theErr = aRet;
            return;
          }
          EXCEPTION(std::runtime_error,"GetTimeStampValue - MEDchampLire(...)");
        }

        MED::PProfileInfo aProfileInfo;
        if(strcmp(&aProfileName[0],"") != 0){
          MED::TKey2Profile::const_iterator anIter = aKey2Profile.find(&aProfileName[0]);
          if(anIter != aKey2Profile.end()){
            aProfileInfo = anIter->second;
            aGeom2Profile[aGeom] = aProfileInfo;
          }
        }
          
        if(aProfileInfo && aProfileInfo->IsPresent()){
          TInt aNbSubElem = aProfileInfo->GetSize();
          TInt aProfileSize = aNbSubElem*aNbComp*aNbGauss;
          if(aProfileSize > aValueSize){
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
    }
    
    
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

      MED::PFieldInfo aFieldInfo = aTimeStampInfo->myFieldInfo;
      TValueHolder<TString, char> aFieldName(aFieldInfo->myName);

      MED::PMeshInfo aMeshInfo = aFieldInfo->myMeshInfo;
      TValueHolder<TString, char> aMeshName(aMeshInfo->myName);
      
      const TGeomSet& aGeomSet = theTimeStampValue->myGeomSet;
      TGeomSet::const_iterator anIter = aGeomSet.begin();
      for(; anIter != aGeomSet.end(); anIter++){
        EGeometrieElement aGeom = *anIter;

        TVector<char> aProfileName(GetNOMLength<eV2_1>()+1);
        MED::TGeom2Profile::iterator aProfileIter = aGeom2Profile.find(aGeom);
        if(aProfileIter != aGeom2Profile.end()){
          MED::TProfileInfo& aProfileInfo = aProfileIter->second;
          aProfileName = aProfileInfo.myName;
        }

        med_int aNbGauss = aTimeStampInfo->GetNbGauss(aGeom);
        med_int aNbVal = theTimeStampValue->GetNbVal(aGeom);
        
        aRet = MEDchampEcr(anId,
                           &aMeshName,
                           &aFieldName,
                           theTimeStampValue->GetValuePtr(aGeom),
                           aModeSwitch,
                           aNbVal,
                           aNbGauss,
                           MED_ALL,
                           &aProfileName[0],
                           MED_ECRI, 
                           anEntity,
                           med_geometrie_element(aGeom),
                           aNumDt,
                           &anUnitDt,
                           aDt,
                           aNumOrd);
        if(aRet < 0){
          if(theErr){
            *theErr = aRet;
            break;
          }
          EXCEPTION(std::runtime_error,"SetTimeStampValue - MEDchampEcr(...)");
        }
        
      }
      
      INITMSG(MYDEBUG,"TVWrapper::SetTimeStampValue - MED_MODE_ACCES = "<<theMode<<"; aRet = "<<aRet<<std::endl);
    }

    
    void
    TVWrapper
    ::SetTimeStampValue(const PTimeStampValueBase& theTimeStampValue,
                        TErr* theErr)
    {
      TErr aRet;
      SetTimeStampValue(theTimeStampValue,eECRI,&aRet);
      
      if(aRet < 0)
        SetTimeStampValue(theTimeStampValue,eREMP,&aRet);

      if(theErr) 
        *theErr = aRet;
    }
    
  }
}
