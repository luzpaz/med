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

#ifndef MED_Wrapper_HeaderFile
#define MED_Wrapper_HeaderFile

#include "MED_Structures.hxx"
#include "MED_Algorithm.hxx"

namespace MED{

  struct TWrapper{
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual 
    PMeshInfo 
    CrMeshInfo(TInt theDim = 0,
	       const std::string& theValue = "",
	       EMaillage theType = eNON_STRUCTURE,
	       const std::string& theDesc = "") = 0;

    virtual 
    PMeshInfo 
    CrMeshInfo(const PMeshInfo& theInfo) = 0;

    virtual 
    TInt 
    GetNbMeshes(TErr* theErr = NULL) = 0;
    
    virtual 
    void 
    GetMeshInfo(TInt theMeshId, TMeshInfo&,
		TErr* theErr = NULL) = 0;

    virtual 
    void
    SetMeshInfo(const TMeshInfo& theInfo,
		TErr* theErr = NULL) = 0;
    
    virtual 
    PMeshInfo
    GetPMeshInfo(TInt theId,
		 TErr* theErr = NULL);


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual 
    TInt
    GetNbFamilies(const TMeshInfo& theMeshInfo,
		  TErr* theErr = NULL) = 0;

    virtual 
    TInt
    GetNbFamAttr(TInt theFamId, 
		 const TMeshInfo& theInfo,
		 TErr* theErr = NULL) = 0;
    
    virtual
    TInt
    GetNbFamGroup(TInt theFamId, 
		  const TMeshInfo& theInfo,
		  TErr* theErr = NULL) = 0;
    
    virtual
    void
    GetFamilyInfo(TInt theFamId, 
		  TFamilyInfo& theInfo,
		  TErr* theErr = NULL) = 0;
    
    virtual
    void
    SetFamilyInfo(const TFamilyInfo& theInfo,
		  TErr* theErr = NULL) = 0;
    
    virtual 
    PFamilyInfo
    CrFamilyInfo(const PMeshInfo& theMeshInfo,
		 TInt theNbGroup = 0, 
		 TInt theNbAttr = 0,
		 TInt theId = 0,
		 const std::string& theValue = "") = 0;
    
    virtual 
    PFamilyInfo
    CrFamilyInfo(const PMeshInfo& theMeshInfo,
		 const std::string& theValue,
		 TInt theId,
		 const TStringSet& theGroupNames, 
		 const TStringVector& theAttrDescs = TStringVector(), 
		 const TIntVector& theAttrIds = TIntVector(), 
		 const TIntVector& theAttrVals = TIntVector()) = 0;
    
    virtual
    PFamilyInfo
    CrFamilyInfo(const PMeshInfo& theMeshInfo,
		 const PFamilyInfo& theInfo) = 0;
    
    PFamilyInfo
    GetPFamilyInfo(const PMeshInfo& theMeshInfo, 
		   TInt theId,
		   TErr* theErr = NULL);
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual
    void
    GetNames(TElemInfo& theInfo,
	     TInt nb,
	     EEntiteMaillage theEntity, 
	     EGeometrieElement theGeom,
	     TErr* theErr = NULL)
    {}

    virtual
    void
    GetNumeration(TElemInfo& theInfo,
		  TInt nb,
		  EEntiteMaillage theEntity, 
		  EGeometrieElement theGeom,
		  TErr* theErr = NULL)
    {}

    virtual
    void
    GetFamilies(TElemInfo& theInfo,
		TInt nb,
		EEntiteMaillage theEntity, 
		EGeometrieElement theGeom,
		TErr* theErr = NULL)
    {}

    virtual
    void
    SetNames(const TElemInfo& theInfo,
	     EEntiteMaillage theEntity, 
	     EGeometrieElement theGeom,
	     TErr* theErr = NULL)
    {}

    virtual
    void
    SetNumeration(const TElemInfo& theInfo,
		  EEntiteMaillage theEntity, 
		  EGeometrieElement theGeom,
		  TErr* theErr = NULL)
    {}

    virtual
    void
    SetFamilies(const TElemInfo& theInfo,
		EEntiteMaillage theEntity, 
		EGeometrieElement theGeom,
		TErr* theErr = NULL)
    {}

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual
    TInt
    GetNbNodes(const TMeshInfo& theMeshInfo,
	       TErr* theErr = NULL) = 0;
    
    virtual
    void
    GetNodeInfo(TNodeInfo& theInfo,
		TErr* theErr = NULL) = 0;

    virtual 
    void
    SetNodeInfo(const TNodeInfo& theInfo,
		TErr* theErr = NULL) = 0;
    
    virtual 
    PNodeInfo
    CrNodeInfo(const PMeshInfo& theMeshInfo, 
	       TInt theNbElem,
	       EModeSwitch theMode = eFULL_INTERLACE,
	       ERepere theSystem = eCART, 
	       EBooleen theIsElemNum = eVRAI,
	       EBooleen theIsElemNames = eVRAI) = 0;

    virtual 
    PNodeInfo
    CrNodeInfo(const PMeshInfo& theMeshInfo, 
	       const TFloatVector& theNodeCoords,
	       EModeSwitch theMode = eFULL_INTERLACE,
	       ERepere theSystem = eCART, 
	       const TStringVector& theCoordNames = TStringVector(),
	       const TStringVector& theCoordUnits = TStringVector(),
	       const TIntVector& theFamilyNums = TIntVector(),
	       const TIntVector& theElemNums = TIntVector(),
	       const TStringVector& theElemNames = TStringVector()) = 0;
    
    virtual
    PNodeInfo
    CrNodeInfo(const PMeshInfo& theMeshInfo,
	       const PNodeInfo& theInfo) = 0;
    
    PNodeInfo
    GetPNodeInfo(const PMeshInfo& theMeshInfo,
		 TErr* theErr = NULL);
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual
    void
    GetPolygoneInfo(TPolygoneInfo& theInfo,
		    TErr* theErr = NULL) 
    {}

    virtual
    void
    SetPolygoneInfo(const TPolygoneInfo& theInfo,
		    TErr* theErr = NULL) 
    {}

    virtual
    TInt
    GetNbPolygones(const TMeshInfo& theMeshInfo,
		   EEntiteMaillage theEntity,
		   EGeometrieElement theGeom,
		   EConnectivite theConnMode = eNOD,
		   TErr* theErr = NULL) 
    { 
      return 0;
    }

    virtual
    TInt
    GetPolygoneConnSize(const TMeshInfo& theMeshInfo,
			EEntiteMaillage theEntity,
			EGeometrieElement theGeom,
			EConnectivite theConnMode = eNOD,
			TErr* theErr = NULL)
    { 
      return 0;
    }
    
    virtual 
    PPolygoneInfo
    CrPolygoneInfo(const PMeshInfo& theMeshInfo, 
		   EEntiteMaillage theEntity, 
		   EGeometrieElement theGeom,
		   TInt theNbElem,
		   TInt theConnSize,
		   EConnectivite theConnMode = eNOD,
		   EBooleen theIsElemNum = eVRAI,
		   EBooleen theIsElemNames = eVRAI)
    {
      return PPolygoneInfo();
    }
    
    virtual
    PPolygoneInfo 
    CrPolygoneInfo(const PMeshInfo& theMeshInfo, 
		   EEntiteMaillage theEntity, 
		   EGeometrieElement theGeom,
		   const TIntVector& theIndexes,
		   const TIntVector& theConnectivities,
		   EConnectivite theConnMode = eNOD,
		   const TIntVector& theFamilyNums = TIntVector(),
		   const TIntVector& theElemNums = TIntVector(),
		   const TStringVector& theElemNames = TStringVector())
    {
      return PPolygoneInfo();
    }

    virtual
    PPolygoneInfo
    CrPolygoneInfo(const PMeshInfo& theMeshInfo,
		   const PPolygoneInfo& theInfo)
    {
      return PPolygoneInfo();
    }
    
    PPolygoneInfo
    GetPPolygoneInfo(const PMeshInfo& theMeshInfo,
		     EEntiteMaillage theEntity, 
		     EGeometrieElement theGeom, 
		     EConnectivite theConnMode = eNOD);
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual 
    void
    GetPolyedreInfo(TPolyedreInfo& theInfo,
		    TErr* theErr = NULL) 
    {}

    virtual
    void
    SetPolyedreInfo(const TPolyedreInfo& theInfo,
		    TErr* theErr = NULL)
    {}
    
    virtual
    TInt
    GetNbPolyedres(const TMeshInfo& theMeshInfo,
		   EEntiteMaillage theEntity,
		   EGeometrieElement theGeom,
		   EConnectivite theConnMode = eNOD,
		   TErr* theErr = NULL)
    { 
      return 0;
    }

    virtual 
    void
    GetPolyedreConnSize(const TMeshInfo& theMeshInfo,
			TInt& theNbFaces,
			TInt& theConnSize,
			EConnectivite theConnMode = eNOD,
			TErr* theErr = NULL)
    {
      theNbFaces = theConnSize = 0;
    }
    
    virtual
    PPolyedreInfo
    CrPolyedreInfo(const PMeshInfo& theMeshInfo, 
		   EEntiteMaillage theEntity, 
		   EGeometrieElement theGeom,
		   TInt theNbElem,
		   TInt theNbFaces,
		   TInt theConnSize,
		   EConnectivite theConnMode = eNOD,
		   EBooleen theIsElemNum = eVRAI,
		   EBooleen theIsElemNames = eVRAI)
    {
      return PPolyedreInfo();
    }
    
    virtual
    PPolyedreInfo
    CrPolyedreInfo(const PMeshInfo& theMeshInfo, 
		   EEntiteMaillage theEntity, 
		   EGeometrieElement theGeom,
		   const TIntVector& theIndexes,
		   const TIntVector& theFaces,
		   const TIntVector& theConnectivities,
		   EConnectivite theConnMode = eNOD,
		   const TIntVector& theFamilyNums = TIntVector(),
		   const TIntVector& theElemNums = TIntVector(),
		   const TStringVector& theElemNames = TStringVector())
    {
      return PPolyedreInfo();
    }

    virtual
    PPolyedreInfo
    CrPolyedreInfo(const PMeshInfo& theMeshInfo,
		   const PPolyedreInfo& theInfo)
    {
      return PPolyedreInfo();
    }
    
    PPolyedreInfo
    GetPPolyedreInfo(const PMeshInfo& theMeshInfo,
		     EEntiteMaillage theEntity, 
		     EGeometrieElement theGeom, 
		     EConnectivite theConnMode = eNOD);
    
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual
    TEntityInfo
    GetEntityInfo(const TMeshInfo& theMeshInfo,
		  EConnectivite theConnMode = eNOD,
		  TErr* theErr = NULL) = 0;
    
    virtual
    TInt
    GetNbCells(const TMeshInfo& theMeshInfo, 
	       EEntiteMaillage theEntity, 
	       EGeometrieElement theGeom, 
	       EConnectivite theConnMode = eNOD,
	       TErr* theErr = NULL) = 0;
    
    virtual
    void
    GetCellInfo(TCellInfo& theInfo,
		TErr* theErr = NULL) = 0;
    
    virtual 
    void
    SetCellInfo(const TCellInfo& theInfo,
		TErr* theErr = NULL) = 0;
    
    virtual
    PCellInfo
    CrCellInfo(const PMeshInfo& theMeshInfo, 
	       EEntiteMaillage theEntity, 
	       EGeometrieElement theGeom,
	       TInt theNbElem,
	       EConnectivite theConnMode = eNOD,
	       EBooleen theIsElemNum = eVRAI,
	       EBooleen theIsElemNames = eVRAI,
	       EModeSwitch theMode = eFULL_INTERLACE) = 0;
    
    virtual
    PCellInfo
    CrCellInfo(const PMeshInfo& theMeshInfo, 
	       EEntiteMaillage theEntity, 
	       EGeometrieElement theGeom,
	       const TIntVector& theConnectivities,
	       EConnectivite theConnMode = eNOD,
	       const TIntVector& theFamilyNums = TIntVector(),
	       const TIntVector& theElemNums = TIntVector(),
	       const TStringVector& theElemNames = TStringVector(),
	       EModeSwitch theMode = eFULL_INTERLACE) = 0;
    
    virtual
    PCellInfo
    CrCellInfo(const PMeshInfo& theMeshInfo,
	       const PCellInfo& theInfo) = 0;
    
    PCellInfo
    GetPCellInfo(const PMeshInfo& theMeshInfo,
		 EEntiteMaillage theEntity, 
		 EGeometrieElement theGeom, 
		 EConnectivite theConnMode = eNOD,
		 TErr* theErr = NULL);


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual 
    TInt
    GetNbFields(TErr* theErr = NULL) = 0;
    
    virtual
    TInt
    GetNbComp(TInt theFieldId,
	      TErr* theErr = NULL) = 0;
    
    virtual 
    void
    GetFieldInfo(TInt theFieldId, 
		 TFieldInfo& theInfo,
		 TErr* theErr = NULL) = 0;

    virtual 
    void
    SetFieldInfo(const TFieldInfo& theInfo,
		 TErr* theErr = NULL) = 0;
    

    virtual 
    PFieldInfo
    CrFieldInfo(const PMeshInfo& theMeshInfo, 
		TInt theNbComp = 0,
		ETypeChamp theType = eFLOAT64,
		const std::string& theValue = "",
		EBooleen theIsLocal = eVRAI,
		TInt theNbRef = 1) = 0;

    virtual 
    PFieldInfo
    CrFieldInfo(const PMeshInfo& theMeshInfo,
		const PFieldInfo& theInfo) = 0;
    
    PFieldInfo
    GetPFieldInfo(const PMeshInfo& theMeshInfo, 
		  TInt theId,
		  TErr* theErr = NULL);


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual 
    TInt
    GetNbGauss(TErr* theErr = NULL)
    { 
      return TInt();
    }

    virtual 
    TGaussInfo::TInfo
    GetGaussPreInfo(TInt theId, 
		    TErr* theErr = NULL)
    {
      return TGaussInfo::TInfo();
    }
    
    virtual 
    void
    GetGaussInfo(TInt theId, 
		 TGaussInfo& theInfo,
		 TErr* theErr = NULL)
    {}

    virtual
    PGaussInfo
    CrGaussInfo(const TGaussInfo::TInfo& theInfo,
		EModeSwitch theMode = eFULL_INTERLACE) = 0;


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual 
    TInt
    GetNbTimeStamps(const TFieldInfo& theInfo, 
		    const TEntityInfo& theEntityInfo,
		    EEntiteMaillage& theEntity,
		    TGeom2Size& theGeom2Size,
		    TErr* theErr = NULL) = 0;
    
    virtual 
    void
    GetTimeStampInfo(TInt theTimeStampId, 
		     TTimeStampInfo& theInfo,
		     TErr* theErr = NULL) = 0;

    virtual 
    PTimeStampInfo
    CrTimeStampInfo(const PFieldInfo& theFieldInfo, 
		    EEntiteMaillage theEntity,
		    const TGeom2Size& theGeom2Size,
		    TInt theNbGauss = 0,
		    TInt theNumDt = 0,
		    TInt theNumOrd = 0,
		    TFloat theDt = 0,
		    const std::string& theUnitDt = "",
		    const TGeom2Gauss& theGeom2Gauss = TGeom2Gauss()) = 0;

    virtual 
    PTimeStampInfo 
    CrTimeStampInfo(const PFieldInfo& theFieldInfo,
		    const PTimeStampInfo& theInfo) = 0;
    
    PTimeStampInfo
    GetPTimeStampInfo(const PFieldInfo& theFieldInfo,
		      EEntiteMaillage theEntity,
		      const TGeom2Size& theGeom2Size,
		      TInt theId,
		      TErr* theErr = NULL);
    

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual 
    TInt
    GetNbProfiles(TErr* theErr = NULL) = 0;

    virtual 
    TProfileInfo::TInfo
    GetProfilePreInfo(TInt theId, 
		      TErr* theErr = NULL) = 0;
    virtual 
    void
    GetProfileInfo(TInt theId, 
		   TProfileInfo& theInfo,
		   TErr* theErr = NULL) = 0;
    virtual
    PProfileInfo
    CrProfileInfo(const TProfileInfo::TInfo& theInfo,
		  EModeProfil theMode = eCOMPACT) = 0;

    PProfileInfo
    GetPProfileInfo(TInt theId,
		    EModeProfil theMode = eCOMPACT,
		    TErr* theErr = NULL);


    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    virtual 
    void
    GetTimeStampVal(TTimeStampVal& theVal,
		    const TMKey2Profile& theMKey2Profile,
		    const TKey2Gauss& theKey2Gauss,
		    TErr* theErr = NULL) = 0;
    
    virtual 
    void
    SetTimeStamp(const TTimeStampVal& theTimeStampVal,
		 TErr* theErr = NULL) = 0;
    
    virtual
    PTimeStampVal
    CrTimeStampVal(const PTimeStampInfo& theTimeStampInfo,
		   const TGeom2Profile& theGeom2Profile = TGeom2Profile(),
		   EModeSwitch theMode = eFULL_INTERLACE) = 0;

    virtual 
    PTimeStampVal
    CrTimeStampVal(const PTimeStampInfo& theTimeStampInfo,
		   const PTimeStampVal& theInfo) = 0;
    
    PTimeStampVal
    GetPTimeStampVal(const PTimeStampInfo& theTimeStampInfo,
		     const TMKey2Profile& theMKey2Profile,
		     const TKey2Gauss& theKey2Gauss,
		     TErr* theErr = NULL);

  };

}

#endif
