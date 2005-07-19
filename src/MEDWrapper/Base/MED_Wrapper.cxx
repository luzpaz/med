//  Copyright (C) 2003  CEA/DEN, EDF R&D
//
//
//
//  File   : VISU_DatConvertor.cxx
//  Author : Alexey PETROV
//  Module : VISU

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
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  PMeshInfo
  TWrapper
  ::GetPMeshInfo(TInt theId,
		 TErr* theErr)
  {
    PMeshInfo anInfo = CrMeshInfo();
    GetMeshInfo(theId,*anInfo,theErr);
    return anInfo;
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  PFamilyInfo 
  TWrapper
  ::GetPFamilyInfo(const PMeshInfo& theMeshInfo, 
		   TInt theId,
		   TErr* theErr)
  {
    TInt aNbAttr = GetNbFamAttr(theId,*theMeshInfo);
    TInt aNbGroup = GetNbFamGroup(theId,*theMeshInfo);
    PFamilyInfo anInfo = CrFamilyInfo(theMeshInfo,aNbGroup,aNbAttr);
    GetFamilyInfo(theId,*anInfo,theErr);

#ifdef _DEBUG_
    string aName = anInfo->GetName();
    INITMSG(MYDEBUG,"GetPFamilyInfo - aFamilyName = '"<<aName<<
	    "'; andId = "<<anInfo->GetId()<<
	    "; aNbAttr = "<<aNbAttr<<
	    "; aNbGroup = "<<aNbGroup<<"\n");
    for(TInt iGroup = 0; iGroup < aNbGroup; iGroup++){
      aName = anInfo->GetGroupName(iGroup);
      INITMSG(MYDEBUG,"aGroupName = '"<<aName<<"'\n");
    }
#endif
    
    return anInfo;
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  PNodeInfo
  TWrapper
  ::GetPNodeInfo(const PMeshInfo& theMeshInfo,
		 TErr* theErr)
  {
    TInt aNbElems = GetNbNodes(*theMeshInfo);
    PNodeInfo anInfo = CrNodeInfo(theMeshInfo,aNbElems);
    GetNodeInfo(*anInfo,theErr);

#ifdef _DEBUG_
    TInt aDim = theMeshInfo->myDim;
    TInt aNbElem = anInfo->GetNbElem();
    INITMSG(MYDEBUG,"GetPNodeInfo: ");
    {
      INITMSG(MYDEBUG,"aCoords: "<<aNbElem<<": ");
      TNodeCoord& aCoord = anInfo->myCoord;
      for(TInt iElem = 0; iElem < aNbElem; iElem++){
	for(TInt iDim = 0, anId = iElem*aDim; iDim < aDim; iDim++, anId++){
	  ADDMSG(MYVALUEDEBUG,aCoord[anId]<<",");
	}
	ADDMSG(MYVALUEDEBUG," ");
      }
      ADDMSG(MYDEBUG,endl);
      
      BEGMSG(MYVALUEDEBUG,"GetFamNum: ");
      for(TInt iElem = 0; iElem < aNbElem; iElem++){
	ADDMSG(MYVALUEDEBUG,anInfo->GetFamNum(iElem)<<", ");
      }
      ADDMSG(MYVALUEDEBUG,endl);
      
      if(anInfo->IsElemNum()){
	BEGMSG(MYVALUEDEBUG,"GetElemNum: ");
	for(TInt iElem = 0; iElem < aNbElem; iElem++){
	  ADDMSG(MYVALUEDEBUG,anInfo->GetElemNum(iElem)<<", ");
	}
	ADDMSG(MYVALUEDEBUG,endl);
      }
    }
    ADDMSG(MYDEBUG,endl);
#endif
    
    return anInfo;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  PPolygoneInfo
  TWrapper
  ::GetPPolygoneInfo(const PMeshInfo& theMeshInfo,
		     EEntiteMaillage theEntity, 
		     EGeometrieElement theGeom, 
		     EConnectivite theConnMode)
  {
    TInt aNbElem = GetNbPolygones(theMeshInfo,theEntity,theGeom,theConnMode);
    TInt aConnSize = GetPolygoneConnSize(theMeshInfo,theEntity,theGeom,theConnMode);
    PPolygoneInfo anInfo = CrPolygoneInfo(theMeshInfo,theEntity,theGeom,aNbElem,aConnSize,theConnMode);
    GetPolygoneInfo(anInfo);

#ifdef _DEBUG_
    const TElemNum& aConn  = anInfo->GetConnectivite();
    const TElemNum& aIndex = anInfo->GetIndex();
    TInt aNbIndex = aIndex.size();
    TInt aIndex0 = aIndex[0];
    INITMSG(MYDEBUG,"theGeom = "<<theGeom<<"; aNbElem = "<<aNbIndex-1<<": ");
    for(TInt iElem = 1; iElem < aNbIndex; iElem++){
      for (TInt i = aIndex0; i < aIndex[iElem];i++)
	ADDMSG(MYVALUEDEBUG,aConn[i-1]<<",");
      ADDMSG(MYDEBUG," ");
      aIndex0 = aIndex[iElem];
    }
    ADDMSG(MYDEBUG,endl);
    BEGMSG(MYDEBUG,"Indexes: ");
    for(TInt iElem = 0; iElem < aIndex.size(); iElem++){
      ADDMSG(MYVALUEDEBUG,aIndex[iElem]<<",");
    }
    ADDMSG(MYDEBUG,endl);
#endif

    return anInfo;
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  PPolyedreInfo
  TWrapper
  ::GetPPolyedreInfo(const PMeshInfo& theMeshInfo,
		     EEntiteMaillage theEntity, 
		     EGeometrieElement theGeom, 
		     EConnectivite theConnMode)
  {
    TInt aNbElem  = GetNbPolyedres(theMeshInfo,theEntity,theGeom,theConnMode);
    TInt aNbFaces, aConnSize;
    GetPolyedreConnSize(theMeshInfo,aNbFaces,aConnSize,theConnMode);
    PPolyedreInfo anInfo = CrPolyedreInfo(theMeshInfo,theEntity,theGeom,aNbElem,aNbFaces,aConnSize,theConnMode);
    GetPolyedreInfo(anInfo);

#ifdef _DEBUG_
    const TElemNum& aConn = anInfo->GetConnectivite();
    const TElemNum& aFaces = anInfo->GetFaces();
    const TElemNum& aIndex = anInfo->GetIndex();
    
    TInt aNbIndex = aIndex.size();
    
    for (int aNp = 0; aNp < aNbIndex-1;aNp++){
      if (anInfo->IsElemNames())
	ADDMSG(MYDEBUG,anInfo->GetElemName(aNp)<<endl);
      else 
	ADDMSG(MYDEBUG,"POLYEDRE "<<aNp+1<<endl);
      
      for (int aNf = aIndex[aNp]-1;aNf < aIndex[aNp+1]-1;aNf++){
	ADDMSG(MYDEBUG,"Face "<<aNf-aIndex[aNp]+2<<": [");
	for (int aNc = aFaces[aNf]-1; aNc < aFaces[aNf+1]-1;aNc++){
	  ADDMSG(MYDEBUG," "<<aConn[aNc]);
	}
	ADDMSG(MYDEBUG," ]"<<endl;);
      }
    }
#endif

    return anInfo;
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  PCellInfo 
  TWrapper
  ::GetPCellInfo(const PMeshInfo& theMeshInfo,
		 EEntiteMaillage theEntity, 
		 EGeometrieElement theGeom, 
		 EConnectivite theConnMode,
		 TErr* theErr)
  {
    TInt aNbElem = GetNbCells(theMeshInfo,theEntity,theGeom,theConnMode);
    PCellInfo anInfo = CrCellInfo(theMeshInfo,theEntity,theGeom,aNbElem,theConnMode);
    GetCellInfo(anInfo,theErr);

#ifdef _DEBUG_
    TInt aConnDim = anInfo->GetConnDim();
    INITMSG(MYDEBUG,"GetPCellInfo - theEntity = "<<theEntity<<"; theGeom = "<<theGeom<<"; aConnDim: "<<aConnDim<<"\n");
    BEGMSG(MYDEBUG,"GetPCellInfo - aNbElem: "<<aNbElem<<": ");
    for(TInt iElem = 0; iElem < aNbElem; iElem++){
      for(TInt iConn = 0; iConn < aConnDim; iConn++){
	ADDMSG(MYVALUEDEBUG,anInfo->GetConn(iElem,iConn)<<",");
      }
      ADDMSG(MYVALUEDEBUG," ");
    }
    ADDMSG(MYDEBUG,endl);

    BEGMSG(MYVALUEDEBUG,"GetPCellInfo - GetFamNum: ");
    for(TInt iElem = 0; iElem < aNbElem; iElem++){
      ADDMSG(MYVALUEDEBUG,anInfo->GetFamNum(iElem)<<", ");
    }
    ADDMSG(MYVALUEDEBUG,endl);

    if(anInfo->IsElemNum()){
      BEGMSG(MYVALUEDEBUG,"GetPCellInfo - GetElemNum: ");
      for(TInt iElem = 0; iElem < aNbElem; iElem++){
	ADDMSG(MYVALUEDEBUG,anInfo->GetElemNum(iElem)<<", ");
      }
      ADDMSG(MYVALUEDEBUG,endl);
    }
    ADDMSG(MYDEBUG,endl);
#endif
    
    return anInfo;
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  PFieldInfo
  TWrapper
  ::GetPFieldInfo(const PMeshInfo& theMeshInfo, 
		  TInt theId,
		  TErr* theErr)
  {
    TInt aNbComp = GetNbComp(theId);
    PFieldInfo anInfo = CrFieldInfo(theMeshInfo,aNbComp);
    GetFieldInfo(theId,*anInfo,theErr);

#ifdef _DEBUG_
    INITMSG(MYDEBUG,
	    "GetPFieldInfo - aName = '"<<anInfo->GetName()<<"'"<<
	    "; aType = "<<anInfo->GetType()<<
	    "; aNbComp = "<<aNbComp<<
	    endl);
#endif
    
    return anInfo;
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  PTimeStampInfo
  TWrapper
  ::GetPTimeStampInfo(const PFieldInfo& theFieldInfo,
		      EEntiteMaillage theEntity,
		      const TGeom2Size& theGeom2Size,
		      TInt theId,
		      TErr* theErr)
  {
    PTimeStampInfo anInfo = CrTimeStampInfo(theFieldInfo,theEntity,theGeom2Size);
    GetTimeStampInfo(theId,*anInfo,theErr);

#ifdef _DEBUG_
    INITMSG(MYDEBUG,
	    "GetPTimeStampInfo - anEntity = "<<anInfo->GetEntity()<<
	    "; aNbGauss = "<<anInfo->GetNbGauss()<<
	    "; aNumDt = "<<anInfo->GetNumDt()<<"\n");
#endif

    return anInfo;
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  PTimeStampVal 
  TWrapper
  ::GetPTimeStampVal(const PTimeStampInfo& theTimeStampInfo,
		     const TMKey2Profile& theMKey2Profile,
		     const TKey2Gauss& theKey2Gauss,
		     TErr* theErr)
  {
    PTimeStampVal anInfo = CrTimeStampVal(theTimeStampInfo);
    GetTimeStampVal(*anInfo,theMKey2Profile,theKey2Gauss,theErr);

#ifdef _DEBUG_
    TInt aNbGauss = theTimeStampInfo->GetNbGauss();
    PFieldInfo aFieldInfo = theTimeStampInfo->GetFieldInfo();
    TInt aNbComp = aFieldInfo->GetNbComp();
    INITMSG(MYDEBUG,"GetPTimeStampVal\n");
    TMeshValue& aMeshValue = anInfo->myMeshValue;
    TMeshValue::const_iterator aMeshValueIter = aMeshValue.begin();
    for(; aMeshValueIter != aMeshValue.end(); aMeshValueIter++){
      const EGeometrieElement& aGeom = aMeshValueIter->first;
      const TValue& aValue = aMeshValueIter->second;
      TInt iElemEnd = aValue.size() / aNbComp / aNbGauss;
      INITMSG(MYDEBUG,"aGeom = "<<aGeom<<" - "<<iElemEnd<<": ");
      for(TInt iElem = 0, anId = 0; iElem < iElemEnd; iElem++){
	ADDMSG(MYVALUEDEBUG,"{");
	for(TInt iComp = 0; iComp < aNbComp; iComp++){
	  for(TInt iGauss = 0; iGauss < aNbGauss; iGauss++){
	    ADDMSG(MYVALUEDEBUG,aValue[anId++]<<" ");
	  }
	  ADDMSG(MYVALUEDEBUG,"| ");
	}
	ADDMSG(MYVALUEDEBUG,"} ");
      }
      ADDMSG(MYDEBUG,"\n");
    }
#endif

    return anInfo;
  }
}
