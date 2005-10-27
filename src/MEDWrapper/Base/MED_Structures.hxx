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

namespace MED
{

  //---------------------------------------------------------------
  //! Defines a type for managing sequence of strings
  typedef TVector<char> TString; 

  //! Extract a substring from the sequence of the strings
  std::string GetString(TInt theId, TInt theStep, 
			const TString& theString);

  //! Set a substring in the sequence of the strings
  void SetString(TInt theId, TInt theStep, 
		 TString& theString, 
		 const std::string& theValue);

  //---------------------------------------------------------------
  //! Define a parent class for all MEDWrapper classes
  struct TBase
  {
    virtual ~TBase() {} 
  };


  //---------------------------------------------------------------
  //! Define a parent class for all named MED entities
  struct TNameInfo: virtual TBase
  {
    TString myName;
    virtual std::string GetName() const = 0;
    virtual void SetName(const std::string& theValue) = 0;
  };


  //---------------------------------------------------------------
  //! Define a parent class for all MED entities that contains a sequence of numbers
  struct TModeSwitchInfo: virtual TBase
  {
    TModeSwitchInfo():
      myModeSwitch(eFULL_INTERLACE)
    {}

    TModeSwitchInfo(EModeSwitch theModeSwitch):
      myModeSwitch(theModeSwitch)
    {}

    EModeSwitch myModeSwitch;
    EModeSwitch GetModeSwitch() const { return myModeSwitch;}
  };


  //---------------------------------------------------------------
  //! Define a base class which represents MED Mesh entity
  struct TMeshInfo: virtual TNameInfo
  {
    TInt myDim; //!< Dimension of the mesh (0, 1, 2 or 3)
    TInt GetDim() const { return myDim;}

    EMaillage myType; //!< Type of the mesh
    EMaillage GetType() const { return myType;}

    TString myDesc; //!< Description of the mesh
    virtual std::string GetDesc() const = 0;
    virtual void SetDesc(const std::string& theValue) = 0;
  };
  

  //---------------------------------------------------------------
  typedef TVector<TInt> TIntVector;
  typedef TSlice<TIntVector> TIntVecSlice;
  typedef TCSlice<TIntVector> TCIntVecSlice;

  typedef TIntVector TFamAttr;

  //! Define a base class which represents MED Family entity
  struct TFamilyInfo: virtual TNameInfo
  {
    PMeshInfo myMeshInfo; //!< A reference to correspondig MED Mesh
    const PMeshInfo& GetMeshInfo() const { return myMeshInfo;}

    TInt myId; //!< An unique index of the MED Family
    TInt GetId() const { return myId;}
    void SetId(TInt theId) { myId = theId;}

    TInt myNbGroup; //!< Defines number MED Groups connected to
    TInt GetNbGroup() const { return myNbGroup;}

    //! Contains sequence of the names for the MED Groups connected to
    TString myGroupNames; 
    virtual std::string GetGroupName(TInt theId) const = 0;
    virtual void SetGroupName(TInt theId, const std::string& theValue) = 0;

    TInt myNbAttr; //!< Defines number of the MED Family attributes 
    TInt GetNbAttr() const { return myNbAttr;}

    //! Defines sequence of the indexes of the MED Family attributes
    TFamAttr myAttrId; 
    TInt GetAttrId(TInt theId) const;
    void SetAttrId(TInt theId, TInt theVal);

    //! Defines sequence of the values of the MED Family attributes
    TFamAttr myAttrVal;
    TInt GetAttrVal(TInt theId) const;
    void SetAttrVal(TInt theId, TInt theVal);

    //! Defines sequence of the names of the MED Family attributes
    TString myAttrDesc;
    virtual std::string GetAttrDesc(TInt theId) const = 0;
    virtual void SetAttrDesc(TInt theId, const std::string& theValue) = 0;
  };


  //---------------------------------------------------------------
  typedef TIntVector TElemNum;
  
  //! Define a parent class for all MED entities that describes mesh entites such as nodes and cells.
  struct TElemInfo: virtual TBase
  {
    PMeshInfo myMeshInfo; //!< A reference to correspondig MED Mesh
    const PMeshInfo& GetMeshInfo() const { return myMeshInfo;}

    TInt myNbElem; //<! Number of corresponding mesh entities
    TInt GetNbElem() const { return myNbElem;}
    
    //! Defines sequence MED Family indexes for corresponding mesh entites
    TElemNum myFamNum; 
    TInt GetFamNum(TInt theId) const;
    void SetFamNum(TInt theId, TInt theVal);

    //! Defines if the mesh elements are indexed
    EBooleen myIsElemNum;
    EBooleen IsElemNum() const { return myIsElemNum;}

    //! Contains sequence of the indexes for the mesh elements
    TElemNum myElemNum;
    TInt GetElemNum(TInt theId) const;
    void SetElemNum(TInt theId, TInt theVal);

    //! Defines if the mesh elements are named
    EBooleen myIsElemNames;
    EBooleen IsElemNames() const { return myIsElemNames;}

    //! Contains sequence of the names for the mesh elements
    TString myElemNames;
    virtual std::string GetElemName(TInt theId) const = 0;
    virtual void SetElemName(TInt theId, const std::string& theValue) = 0;
  };


  //---------------------------------------------------------------
  typedef TVector<TFloat> TFloatVector;
  typedef TSlice<TFloatVector> TFloatVecSlice;
  typedef TCSlice<TFloatVector> TCFloatVecSlice;

  typedef TFloatVector TNodeCoord;
  typedef TFloatVecSlice TCoordSlice;
  typedef TCFloatVecSlice TCCoordSlice;

  //! Define a base class which represents MED Nodes entity
  struct TNodeInfo: 
    virtual TElemInfo,
    virtual TModeSwitchInfo 
  {
    TNodeCoord myCoord; //!< Contains all nodal coordinates

    //! Gives coordinates for mesh node by its number
    TCCoordSlice GetCoordSlice(TInt theId) const;
    //! Gives coordinates for mesh node by its number
    TCoordSlice GetCoordSlice(TInt theId);

    ERepere mySystem; //!< Defines, which coordinate system is used
    ERepere GetSystem() const { return mySystem;}
    void SetSystem(ERepere theSystem) { mySystem = theSystem;}

    TString myCoordNames; //!< Contains names for the coordinate dimensions
    virtual std::string GetCoordName(TInt theId) const = 0;
    virtual void SetCoordName(TInt theId, const std::string& theValue) = 0;

    TString myCoordUnits; //!< Contains units for the coordinate dimensions
    virtual std::string GetCoordUnit(TInt theId) const = 0;
    virtual void SetCoordUnit(TInt theId, const std::string& theValue) = 0;
  };


  //---------------------------------------------------------------
  typedef TIntVecSlice TConnSlice;
  typedef TCIntVecSlice TCConnSlice;

  //! Define a base class which represents MED Cells entity
  struct TCellInfo: 
    virtual TElemInfo,
    virtual TModeSwitchInfo 
  {
    EEntiteMaillage myEntity; //!< Defines the MED Entity where the mesh cells belongs to
    EEntiteMaillage GetEntity() const { return myEntity;}

    EGeometrieElement myGeom; //!< Defines the MED Geometric type of the instance
    EGeometrieElement GetGeom() const { return myGeom;}

    EConnectivite myConnMode; //!< Defines connectivity mode
    EConnectivite GetConnMode() const { return myConnMode;}

    virtual TInt GetConnDim() const = 0; //!< Gives step in the connectivity sequence

    TElemNum myConn; //!< Defines sequence which describe connectivity for ech of mesh cell

    //! Gives connectivities for mesh cell by its number
    TCConnSlice GetConnSlice(TInt theElemId) const;
    //! Gives connectivities for mesh cell by its number
    TConnSlice GetConnSlice(TInt theElemId);
  };

  //---------------------------------------------------------------
  //! Define a base class which represents MED Polygon entity
  struct TPolygoneInfo: 
    virtual TElemInfo
  {
    //! Defines the MED Entity where the polygons belongs to
    EEntiteMaillage myEntity; // MED_FACE|MED_MAILLE
    EEntiteMaillage GetEntity() const { return myEntity;}

    //! Defines the MED Geometric type of the instance
    EGeometrieElement myGeom; // ePOLYGONE
    EGeometrieElement GetGeom() const { return ePOLYGONE;}

    //! Defines connectivity mode
    EConnectivite myConnMode; // eNOD|eDESC(eDESC not used)
    EConnectivite GetConnMode() const { return myConnMode;}

    TElemNum myConn; //!< Table de connectivities
    TElemNum myIndex; //!< Table de indexes

    //! Gives number of the connectivities for the defined polygon
    TInt GetNbConn(TInt theElemId) const;

    //! Gives connectivities for polygon by its number
    TCConnSlice GetConnSlice(TInt theElemId) const;
    TConnSlice GetConnSlice(TInt theElemId);
  };

  //---------------------------------------------------------------
  typedef TVector<TCConnSlice> TCConnSliceArr;
  typedef TVector<TConnSlice> TConnSliceArr;

  //! Define a base class which represents MED Polyedre entity
  struct TPolyedreInfo: 
    virtual TElemInfo
  {
    //! Defines the MED Entity where the polyedres belongs to
    EEntiteMaillage myEntity; // MED_FACE|MED_MAILLE
    EEntiteMaillage GetEntity() const { return myEntity;}

    //! Defines the MED Geometric type of the instance
    EGeometrieElement myGeom; // ePOLYEDRE
    EGeometrieElement GetGeom() const { return ePOLYEDRE;}

    //! Defines connectivity mode
    EConnectivite myConnMode; // eNOD|eDESC(eDESC not used)
    EConnectivite GetConnMode() const { return myConnMode;}

    TElemNum myConn; //!< Table de connectivities
    TElemNum myFaces; //!< Table de faces indexes
    TElemNum myIndex; //!< Table de indexes

    //! Gives number of the faces for the defined polyedre
    TInt GetNbFaces(TInt theElemId) const;
    //! Gives number of the nodes for the defined polyedre
    TInt GetNbNodes(TInt theElemId) const;

    //! Gives sequence of the face connectivities for polyedre by its number
    TCConnSliceArr GetConnSliceArr(TInt theElemId) const;
    //! Gives sequence of the face connectivities for polyedre by its number
    TConnSliceArr GetConnSliceArr(TInt theElemId);
  };

  //---------------------------------------------------------------
  //! Define a base class which represents MED Field entity
  struct TFieldInfo: 
    virtual TNameInfo
  {
    PMeshInfo myMeshInfo; //!< A reference to correspondig MED Mesh
    const PMeshInfo& GetMeshInfo() const { return myMeshInfo;}

    ETypeChamp myType; //!< Defines type of the MED Field
    ETypeChamp GetType() const { return myType;}

    TInt myNbComp; //!< Defines number of components stored in the field
    TInt GetNbComp() const { return myNbComp;}

    EBooleen myIsLocal; //!< Defines if the MED Field is local
    EBooleen GetIsLocal() const { return myIsLocal;}

    TInt myNbRef; //!< Defines number of refereces of the field
    TInt GetNbRef() const { return myNbRef;}

    TString myCompNames; //!< Contains names for each of MED Field components
    virtual std::string GetCompName(TInt theId) const = 0;
    virtual void SetCompName(TInt theId, const std::string& theValue) = 0;

    TString myUnitNames; //!< Contains units for each of MED Field components
    virtual std::string GetUnitName(TInt theId) const = 0;
    virtual void SetUnitName(TInt theId, const std::string& theValue) = 0;
  };


  //---------------------------------------------------------------
  //! Get dimension of the Gauss coordinates for the defined type of mesh cell
  TInt
  GetDimGaussCoord(EGeometrieElement theGeom);

  //! Get number of referenced nodes for the defined type of mesh cell
  TInt
  GetNbRefCoord(EGeometrieElement theGeom);

  typedef TFloatVector TWeight;

  //! The class represents MED Gauss entity
  struct TGaussInfo: 
    virtual TNameInfo,
    virtual TModeSwitchInfo 
  {
    typedef boost::tuple<EGeometrieElement,std::string> TKey;
    typedef boost::tuple<TKey,TInt> TInfo;
    struct TLess
    {
      bool
      operator()(const TKey& theLeft, const TKey& theRight) const;
    };

    //! Defines, which geometrical type the MED Gauss entity belongs to
    EGeometrieElement myGeom; 
    EGeometrieElement GetGeom() const { return myGeom;}

    //! Contains coordinates for the refereced nodes
    TNodeCoord myRefCoord; 

    //! Gives coordinates for the referenced node by its number
    TCCoordSlice GetRefCoordSlice(TInt theId) const;
    //! Gives coordinates for the referenced node by its number
    TCoordSlice GetRefCoordSlice(TInt theId);

    //! Contains coordinates for the Gauss points
    TNodeCoord myGaussCoord;

    //! Gives coordinates for the Gauss points by its number
    TCCoordSlice GetGaussCoordSlice(TInt theId) const;
    //! Gives coordinates for the Gauss points by its number
    TCoordSlice GetGaussCoordSlice(TInt theId);

    //! Contains wheights for the Gauss points
    TWeight myWeight;

    //! Gives number of the referenced nodes
    TInt GetNbRef() const { return GetNbRefCoord(GetGeom());}

    //! Gives dimension of the referenced nodes
    TInt GetDim() const { return GetDimGaussCoord(GetGeom());}

    //! Gives number of the Gauss Points
    TInt GetNbGauss() const { return myGaussCoord.size()/GetDim();}
  };


  //---------------------------------------------------------------
  typedef std::map<EGeometrieElement,PGaussInfo> TGeom2Gauss;

  //! Define a base class which represents MED TimeStamp
  struct TTimeStampInfo: 
    virtual TBase
  {
    PFieldInfo myFieldInfo; //!< A reference to correspondig MED Field
    const PFieldInfo& GetFieldInfo() const { return myFieldInfo;}

    //! Defines the MED Entity where the MED TimeStamp belongs to
    EEntiteMaillage myEntity;
    EEntiteMaillage GetEntity() const { return myEntity;}

    //! Keeps map of number of cells per geometric type where the MED TimeStamp belongs to
    TGeom2Size myGeom2Size;
    const TGeom2Size& GetGeom2Size() const { return myGeom2Size;}

    TInt myNbGauss, myNumDt, myNumOrd;
    TInt GetNbGauss() const { return myNbGauss;} //!< Gives number of the Gauss Points for the MED TimeStamp
    TInt GetNumDt() const { return myNumDt;} //!< Defines number in time for the MED TimeStamp
    TInt GetNumOrd() const { return myNumOrd;} //!< Defines number for the MED TimeStamp

    TFloat myDt;
    TFloat GetDt() const { return myDt;} //!< Defines time for the MED TimeStamp

    //! Keeps map of MED Gauss entityes per geometric type
    TGeom2Gauss myGeom2Gauss;
    const TGeom2Gauss& GetGeom2Gauss() const { return myGeom2Gauss;}

    TString myUnitDt; //!< Defines unit for the time for the MED TimeStamp
    virtual std::string GetUnitDt() const = 0;
    virtual void SetUnitDt(const std::string& theValue) = 0;
  };
  

  //---------------------------------------------------------------
  //! The class represents MED Profile entity
  struct TProfileInfo: 
    virtual TNameInfo
  {
    typedef std::string TKey;
    typedef boost::tuple<TKey,TInt> TInfo;

    EModeProfil myMode; //!< Keeps mode for the MED Profile
    EModeProfil GetMode() const { return myMode;}
    void SetMode(EModeProfil theMode) { myMode = theMode;}

    TElemNum myElemNum; //!< Keeps sequence of cell by its number which belong to the profile
    TInt GetElemNum(TInt theId) const;
    void SetElemNum(TInt theId, TInt theVal);

    bool IsPresent() const { return GetName() != "";}
    TInt GetSize() const { return myElemNum.size();}
  };


  //---------------------------------------------------------------
  typedef TFloatVector TValue;
  typedef TSlice<TValue> TValueSlice;
  typedef TCSlice<TValue> TCValueSlice;

  typedef TVector<TCValueSlice> TCValueSliceArr;
  typedef TVector<TValueSlice> TValueSliceArr;

  //! The class is a helper one. It provide safe and flexible way to get access to values for a MED TimeStamp
  struct TMeshValue:
    virtual TModeSwitchInfo 
  {
    TValue myValue;

    TInt myNbElem;
    TInt myNbComp;
    TInt myNbGauss;
    TInt myStep;

    //! Initialize the class
    void
    Init(TInt theNbElem,
	 TInt theNbGauss,
	 TInt theNbComp,
	 EModeSwitch theMode = eFULL_INTERLACE);

    //! Iteration through Gauss Points by their components
    TCValueSliceArr
    GetGaussValueSliceArr(TInt theElemId) const;

    //! Iteration through Gauss Points by their components
    TValueSliceArr 
    GetGaussValueSliceArr(TInt theElemId);

    //! Iteration through components by corresponding Gauss Points
    TCValueSliceArr
    GetCompValueSliceArr(TInt theElemId) const;

    //! Iteration through components by corresponding Gauss Points
    TValueSliceArr 
    GetCompValueSliceArr(TInt theElemId);
  };


  //---------------------------------------------------------------
  typedef std::map<EGeometrieElement,TMeshValue> TGeom2Value;
  typedef std::map<EGeometrieElement,PProfileInfo> TGeom2Profile;

  //! The class implements a container for MED TimeStamp values
  struct TTimeStampVal: 
    virtual TModeSwitchInfo 
  {
    PTimeStampInfo myTimeStampInfo; //!< A reference to correspondig MED TimeStamp
    const PTimeStampInfo& GetTimeStampInfo() const { return myTimeStampInfo;}

    //! Keeps map of MED Profiles per geometric type
    TGeom2Profile myGeom2Profile;
    const TGeom2Profile& GetGeom2Profile() const { return myGeom2Profile;}

    //! Keeps map of MED TimeStamp values per geometric type
    TGeom2Value myGeom2Value;
    const TMeshValue& GetMeshValue(EGeometrieElement theGeom) const;
    TMeshValue& GetMeshValue(EGeometrieElement theGeom);
  };

}

#endif
