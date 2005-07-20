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

#ifndef MED_Structures_HeaderFile
#define MED_Structures_HeaderFile

#include "MED_Common.hxx"

namespace MED{

  //---------------------------------------------------------------
  typedef std::vector<char> TString;

  std::string GetString(TInt theId, TInt theStep, 
			const TString& theString);

  void SetString(TInt theId, TInt theStep, 
		 TString& theString, 
		 const std::string& theValue);

  //---------------------------------------------------------------
  struct TBase
  {
    virtual ~TBase() {} 
  };


  //---------------------------------------------------------------
  struct TNameInfo: virtual TBase
  {
    TString myName;
    virtual std::string GetName() const = 0;
    virtual void SetName(const std::string& theValue) = 0;
  };


  //---------------------------------------------------------------
  struct TModeSwitchInfo: virtual TBase
  {
    EModeSwitch myModeSwitch;
    EModeSwitch GetModeSwitch() const { return myModeSwitch;}
  };


  //---------------------------------------------------------------
  struct TMeshInfo: virtual TNameInfo
  {
    TInt myDim;
    TInt GetDim() const { return myDim;}

    EMaillage myType;
    EMaillage GetType() const { return myType;}

    TString myDesc;
    virtual std::string GetDesc() const = 0;
    virtual void SetDesc(const std::string& theValue) = 0;
  };
  

  //---------------------------------------------------------------
  typedef std::vector<TInt> TIntVector;
  typedef TSlice<TIntVector> TIntVecSlice;
  typedef TCSlice<TIntVector> TCIntVecSlice;

  typedef TIntVector TFamAttr;

  struct TFamilyInfo: virtual TNameInfo
  {
    PMeshInfo myMeshInfo;
    const PMeshInfo& GetMeshInfo() const { return myMeshInfo;}

    TInt myId;
    TInt GetId() const { return myId;}
    void SetId(TInt theId) { myId = theId;}

    TInt myNbGroup;
    TInt GetNbGroup() const { return myNbGroup;}

    TString myGroupNames;
    virtual std::string GetGroupName(TInt theId) const = 0;
    virtual void SetGroupName(TInt theId, const std::string& theValue) = 0;

    TInt myNbAttr;
    TInt GetNbAttr() const { return myNbAttr;}

    TFamAttr myAttrId;
    TInt GetAttrId(TInt theId) const;
    void SetAttrId(TInt theId, TInt theVal);

    TFamAttr myAttrVal;
    TInt GetAttrVal(TInt theId) const;
    void SetAttrVal(TInt theId, TInt theVal);

    TString myAttrDesc;
    virtual std::string GetAttrDesc(TInt theId) const = 0;
    virtual void SetAttrDesc(TInt theId, const std::string& theValue) = 0;
  };


  //---------------------------------------------------------------
  typedef TIntVector TElemNum;
  
  struct TElemInfo: virtual TBase
  {
    PMeshInfo myMeshInfo;
    const PMeshInfo& GetMeshInfo() const { return myMeshInfo;}

    TInt myNbElem;
    TInt GetNbElem() const { return myNbElem;}
    
    TElemNum myFamNum;
    TInt GetFamNum(TInt theId) const;
    void SetFamNum(TInt theId, TInt theVal);

    EBooleen myIsElemNum;
    EBooleen IsElemNum() const { return myIsElemNum;}

    TElemNum myElemNum;
    TInt GetElemNum(TInt theId) const;
    void SetElemNum(TInt theId, TInt theVal);

    EBooleen myIsElemNames;
    EBooleen IsElemNames() const { return myIsElemNames;}

    TString myElemNames;
    virtual std::string GetElemName(TInt theId) const = 0;
    virtual void SetElemName(TInt theId, const std::string& theValue) = 0;
  };


  //---------------------------------------------------------------
  typedef std::vector<TFloat> TFloatVector;
  typedef TSlice<TFloatVector> TFloatVecSlice;
  typedef TCSlice<TFloatVector> TCFloatVecSlice;

  typedef TFloatVector TNodeCoord;
  typedef TFloatVecSlice TCoordSlice;
  typedef TCFloatVecSlice TCCoordSlice;

  struct TNodeInfo: 
    virtual TElemInfo,
    virtual TModeSwitchInfo 
  {
    TNodeCoord myCoord;

    TCCoordSlice GetCoordSlice(TInt theId) const;
    TCoordSlice GetCoordSlice(TInt theId);

    TFloat GetNodeCoord(TInt theId, TInt theComp) const;
    void SetNodeCoord(TInt theId, TInt theComp, TFloat theVal);

    ERepere mySystem;
    ERepere GetSystem() const { return mySystem;}
    void SetSystem(ERepere theSystem) { mySystem = theSystem;}

    TString myCoordNames;
    virtual std::string GetCoordName(TInt theId) const = 0;
    virtual void SetCoordName(TInt theId, const std::string& theValue) = 0;

    TString myCoordUnits;
    virtual std::string GetCoordUnit(TInt theId) const = 0;
    virtual void SetCoordUnit(TInt theId, const std::string& theValue) = 0;
  };


  //---------------------------------------------------------------
  typedef TIntVecSlice TConnSlice;
  typedef TCIntVecSlice TCConnSlice;

  struct TCellInfo: 
    virtual TElemInfo,
    virtual TModeSwitchInfo 
  {
    EEntiteMaillage myEntity;
    EEntiteMaillage GetEntity() const { return myEntity;}

    EGeometrieElement myGeom;
    EGeometrieElement GetGeom() const { return myGeom;}

    EConnectivite myConnMode;
    EConnectivite GetConnMode() const { return myConnMode;}

    virtual TInt GetConnDim() const = 0;

    TElemNum myConn;
    TCConnSlice GetConnSlice(TInt theElemId) const;
    TConnSlice GetConnSlice(TInt theElemId);

    TInt GetConn(TInt theElemId, TInt theConnId) const;
    void SetConn(TInt theElemId, TInt theConnId, TInt theVal);
  };

  //---------------------------------------------------------------
  struct TPolygoneInfo: 
    virtual TElemInfo
  {
    EEntiteMaillage myEntity; // MED_FACE|MED_MAILLE
    EEntiteMaillage GetEntity() const { return myEntity;}

    EGeometrieElement myGeom; // ePOLYGONE
    EGeometrieElement GetGeom() const { return ePOLYGONE;}

    EConnectivite myConnMode; // eNOD|eDESC(eDESC not used)
    EConnectivite GetConnMode() const { return myConnMode;}

    TInt myConnSize;
    TInt GetConnSize() const { return myConnSize;}

    TElemNum myConn; // Table de connectivities
    const TElemNum& GetConnectivite() const { return myConn;}

    TElemNum myIndex; // Table de indexes
    const TElemNum& GetIndex() const { return myIndex;}
    TInt GetNbConn(TInt theElemId) const;
  };

  //---------------------------------------------------------------
  struct TPolyedreInfo: 
    virtual TElemInfo
  {
    EEntiteMaillage myEntity; // MED_FACE|MED_MAILLE
    EEntiteMaillage GetEntity() const { return myEntity;}

    EGeometrieElement myGeom; // ePOLYEDRE
    EGeometrieElement GetGeom() const { return ePOLYEDRE;}

    EConnectivite myConnMode; // eNOD|eDESC(eDESC not used)
    EConnectivite GetConnMode() const { return myConnMode;}

    TInt myConnSize;
    TInt GetConnSize() const { return myConnSize;}

    TElemNum myConn; // Table de connectivities
    TElemNum GetConnectivite() const { return myConn;}
    
    TInt myNbFaces;
    TInt GetNbFaces() const { return myNbFaces;}
    
    TElemNum myFaces; // Table de faces indexes
    const TElemNum& GetFaces() const { return myFaces;}
    
    TElemNum myIndex; // Table de indexes
    const TElemNum& GetIndex() const { return myIndex;}
    TInt GetNbConn(TInt theElemId) const;
  };

  //---------------------------------------------------------------
  struct TFieldInfo: 
    virtual TNameInfo
  {
    PMeshInfo myMeshInfo;
    const PMeshInfo& GetMeshInfo() const { return myMeshInfo;}

    ETypeChamp myType;
    ETypeChamp GetType() const { return myType;}

    TInt myNbComp;
    TInt GetNbComp() const { return myNbComp;}

    EBooleen myIsLocal;
    EBooleen GetIsLocal() const { return myIsLocal;}

    TInt myNbRef;
    TInt GetNbRef() const { return myNbRef;}

    TString myCompNames; 
    virtual std::string GetCompName(TInt theId) const = 0;
    virtual void SetCompName(TInt theId, const std::string& theValue) = 0;

    TString myUnitNames; 
    virtual std::string GetUnitName(TInt theId) const = 0;
    virtual void SetUnitName(TInt theId, const std::string& theValue) = 0;
  };


  //---------------------------------------------------------------
  TInt
  GetDimGaussCoord(EGeometrieElement theGeom);

  TInt
  GetNbRefCoord(EGeometrieElement theGeom);

  typedef TFloatVector TWeight;

  struct TGaussInfo: 
    virtual TNameInfo,
    virtual TModeSwitchInfo 
  {
    typedef boost::tuple<std::string,EGeometrieElement> TKey;
    typedef boost::tuple<TKey,TInt> TInfo;
    struct TLess
    {
      bool
      operator()(const TKey& theLeft, const TKey& theRight) const
      {
	std::string aLStr = boost::get<0>(theLeft);
	std::string aRStr = boost::get<0>(theRight);
	if(aLStr != aRStr)
	  return aLStr < aRStr;

	EGeometrieElement aLGeom = boost::get<1>(theLeft);
	EGeometrieElement aRGeom = boost::get<1>(theRight);
	return aLGeom < aRGeom;
      }
    };

    EGeometrieElement myGeom;
    EGeometrieElement GetGeom() const { return myGeom;}

    TNodeCoord myRefCoord;
    TCCoordSlice GetRefCoordSlice(TInt theId) const;
    TCoordSlice GetRefCoordSlice(TInt theId);

    TNodeCoord myGaussCoord;
    TCCoordSlice GetGaussCoordSlice(TInt theId) const;
    TCoordSlice GetGaussCoordSlice(TInt theId);

    TWeight myWeight;

    TInt GetNbRef() const { return GetNbRefCoord(GetGeom());}
    TInt GetDim() const { return GetDimGaussCoord(GetGeom());}
    TInt GetNbGauss() const { return myGaussCoord.size()/GetDim();}
  };


  //---------------------------------------------------------------
  typedef std::map<EGeometrieElement,PGaussInfo> TGeom2Gauss;

  struct TTimeStampInfo: 
    virtual TBase
  {
    PFieldInfo myFieldInfo;
    const PFieldInfo& GetFieldInfo() const { return myFieldInfo;}

    EEntiteMaillage myEntity;
    EEntiteMaillage GetEntity() const { return myEntity;}

    TGeom2Size myGeom2Size;
    const TGeom2Size& GetGeom2Size() const { return myGeom2Size;}

    TInt myNbGauss, myNumDt, myNumOrd;
    TInt GetNbGauss() const { return myNbGauss;}
    TInt GetNumDt() const { return myNumDt;}
    TInt GetNumOrd() const { return myNumOrd;}

    TFloat myDt;
    TFloat GetDt() const { return myDt;}

    TGeom2Gauss myGeom2Gauss;
    const TGeom2Gauss& GetGeom2Gauss() const { return myGeom2Gauss;}

    TString myUnitDt;
    virtual std::string GetUnitDt() const = 0;
    virtual void SetUnitDt(const std::string& theValue) = 0;
  };
  

  //---------------------------------------------------------------
  struct TProfileInfo: 
    virtual TNameInfo
  {
    typedef std::string TKey;
    typedef boost::tuple<TKey,TInt> TInfo;

    EModeProfil myMode;
    EModeProfil GetMode() const { return myMode;}
    void SetMode(EModeProfil theMode) { myMode = theMode;}

    TElemNum myElemNum;
    TInt GetElemNum(TInt theId) const;
    void SetElemNum(TInt theId, TInt theVal);

    bool IsPresent() const { return GetName() != "";}
    TInt GetSize() const { return myElemNum.size();}
  };


  //---------------------------------------------------------------
  typedef TFloatVector TValue;
  typedef std::map<EGeometrieElement,TValue> TMeshValue;
  typedef std::map<EGeometrieElement,PProfileInfo> TGeom2Profile;

  struct TTimeStampVal: 
    virtual TModeSwitchInfo 
  {
    PTimeStampInfo myTimeStampInfo;
    const PTimeStampInfo& GetTimeStampInfo() const { return myTimeStampInfo;}

    TGeom2Profile myGeom2Profile;
    const TGeom2Profile& GetGeom2Profile() const { return myGeom2Profile;}

    TMeshValue myMeshValue;
    TFloat GetVal(EGeometrieElement theGeom, TInt theId, 
		  TInt theComp, TInt theGauss = 0) const;

    void SetVal(EGeometrieElement theGeom, TInt theId, 
		TInt theComp, TFloat theVal, TInt theGauss = 0);
  };

}

#endif
