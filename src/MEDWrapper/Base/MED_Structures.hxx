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
    TString myName; //!< Keeps its name
    virtual std::string GetName() const = 0; //!< Gets its name
    virtual void SetName(const std::string& theValue) = 0; //!< Set a new name
  };


  //---------------------------------------------------------------
  //! Define a parent class for all MED entities that contains a sequence of numbers
  /*!
    It defines through corresponding enumeration (EModeSwitch) how the sequence 
    should be interpreted in C or Fortran mode (eFULL_INTERLACE or eNON_INTERLACE).
  */
  struct TModeSwitchInfo: virtual TBase
  {
    //! To construct instance of the class by default
    TModeSwitchInfo():
      myModeSwitch(eFULL_INTERLACE)
    {}

    //! To construct instance of the class
    TModeSwitchInfo(EModeSwitch theModeSwitch):
      myModeSwitch(theModeSwitch)
    {}

    EModeSwitch myModeSwitch; //!< Keeps the 
    EModeSwitch GetModeSwitch() const { return myModeSwitch;}
  };


  //---------------------------------------------------------------
  //! Define a base class which represents MED Mesh entity
  struct TMeshInfo: virtual TNameInfo
  {
    TInt myDim; //!< Dimension of the mesh (0, 1, 2 or 3)
    TInt GetDim() const { return myDim;} //!< Gets dimension of the mesh

    EMaillage myType; //!< Type of the mesh
    EMaillage GetType() const { return myType;} //!< Gets type of the mesh

    TString myDesc; //!< Description of the mesh
    virtual std::string GetDesc() const = 0; //!< Get description for the mesh
    virtual void SetDesc(const std::string& theValue) = 0; //!< Sets description for the mesh
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
    //! Get a reference to corresponding MED Mesh
    const PMeshInfo& GetMeshInfo() const { return myMeshInfo;} 

    TInt myId; //!< An unique index of the MED FAMILY
    TInt GetId() const { return myId;} //!< Gets number of the MED FAMILY
    void SetId(TInt theId) { myId = theId;} //! Define number of the MED FAMILY

    TInt myNbGroup; //!< Defines number MED Groups connected to
    //! Gets number of MED GROUPS the MED FAMILY is bound to
    TInt GetNbGroup() const { return myNbGroup;} 

    //! Contains sequence of the names for the MED Groups connected to
    TString myGroupNames; 
    //! Gets name of a bound MED GROUP by its number
    virtual std::string GetGroupName(TInt theId) const = 0;
    //! Sets name of the defined MED GROUP by its number
    virtual void SetGroupName(TInt theId, const std::string& theValue) = 0;

    TInt myNbAttr; //!< Defines number of the MED Family attributes 
    //! Gets number of attached attributes for the MED FAMILY
    TInt GetNbAttr() const { return myNbAttr;} 

    //! Defines sequence of the indexes of the MED Family attributes
    TFamAttr myAttrId; 
    //! Get MED FAMILY attribute by its number
    TInt GetAttrId(TInt theId) const;
    //! Set MED FAMILY attribute by its number
    void SetAttrId(TInt theId, TInt theVal);

    //! Defines sequence of the values of the MED Family attributes
    TFamAttr myAttrVal;
    //! Get MED FAMILY attribute by its number
    TInt GetAttrVal(TInt theId) const;
    //! Set MED FAMILY attribute by its number
    void SetAttrVal(TInt theId, TInt theVal);

    //! Defines sequence of the names of the MED Family attributes
    TString myAttrDesc;
    //! Get value of the MED FAMILY attribute by its number
    virtual std::string GetAttrDesc(TInt theId) const = 0;
    //! Set value of the MED FAMILY attribute by its number
    virtual void SetAttrDesc(TInt theId, const std::string& theValue) = 0;
  };


  //---------------------------------------------------------------
  typedef TIntVector TElemNum;
  
  //! Define a parent class for all MED entities that describes mesh entites such as nodes and cells.
  struct TElemInfo: virtual TBase
  {
    PMeshInfo myMeshInfo; //!< A reference to correspondig MED Mesh
    //! Get a reference to corresponding MED Mesh
    const PMeshInfo& GetMeshInfo() const { return myMeshInfo;}

    TInt myNbElem; //<! Number of corresponding mesh entities
    TInt GetNbElem() const { return myNbElem;} //! Get number of mesh elements
    
    //! Defines sequence MED Family indexes for corresponding mesh entites
    TElemNum myFamNum; 
    //! Get number of a MED FAMILY by order number of the mesh element
    TInt GetFamNum(TInt theId) const;
    //! Set number of a MED FAMILY for the mesh element with the  order number
    void SetFamNum(TInt theId, TInt theVal);

    //! Defines if the mesh elements are indexed
    EBooleen myIsElemNum;
    //! Let know if the mesh elements are indexed
    EBooleen IsElemNum() const { return myIsElemNum;}

    //! Contains sequence of the indexes for the mesh elements
    TElemNum myElemNum;
    //! Get a reference number of the mesh element by its order number
    TInt GetElemNum(TInt theId) const;
    //! Set a reference number for the mesh element by its order number
    void SetElemNum(TInt theId, TInt theVal);

    //! Defines if the mesh elements are named
    EBooleen myIsElemNames;
    //! Let know if the mesh elements havew names
    EBooleen IsElemNames() const { return myIsElemNames;}

    //! Contains sequence of the names for the mesh elements
    TString myElemNames;
    //! Get name of the mesh element by its order number
    virtual std::string GetElemName(TInt theId) const = 0;
    //! Set name of the mesh element by its order number
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

    //! Gives coordinates for mesh node by its number (const version)
    TCCoordSlice GetCoordSlice(TInt theId) const;
    //! Gives coordinates for mesh node by its number
    TCoordSlice GetCoordSlice(TInt theId);

    ERepere mySystem; //!< Defines, which coordinate system is used
    //! Get which coordinate system is used for the node describing
    ERepere GetSystem() const { return mySystem;}
    //! Set coordinate system to be used for the node describing
    void SetSystem(ERepere theSystem) { mySystem = theSystem;}

    TString myCoordNames; //!< Contains names for the coordinate dimensions
    //! Get name of the coordinate dimension by its order number
    virtual std::string GetCoordName(TInt theId) const = 0;
    //! Set name of the coordinate dimension by its order number
    virtual void SetCoordName(TInt theId, const std::string& theValue) = 0;

    TString myCoordUnits; //!< Contains units for the coordinate dimensions
    //! Get name of unit for the coordinate dimension by its order number
    virtual std::string GetCoordUnit(TInt theId) const = 0;
    //! Set name of unit for the coordinate dimension by its order number
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
    //! Let known what MED ENTITY the cells belong to
    EEntiteMaillage GetEntity() const { return myEntity;}

    EGeometrieElement myGeom; //!< Defines the MED Geometric type of the instance
    //! Let known what MED geometrical type the cells belong to
    EGeometrieElement GetGeom() const { return myGeom;}

    EConnectivite myConnMode; //!< Defines connectivity mode
    //! Let known in what connectivity the cells are writen
    EConnectivite GetConnMode() const { return myConnMode;}

    virtual TInt GetConnDim() const = 0; //!< Gives step in the connectivity sequence

    TElemNum myConn; //!< Defines sequence which describe connectivity for ech of mesh cell

    //! Gives connectivities for mesh cell by its number (const version)
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
    //! Let known what MED ENTITY the MED Polygons belong to
    EEntiteMaillage GetEntity() const { return myEntity;}

    //! Defines the MED Geometric type of the instance
    EGeometrieElement myGeom; // ePOLYGONE
    //! Let known what MED geometrical type the MED Polygons belong to
    EGeometrieElement GetGeom() const { return ePOLYGONE;}

    //! Defines connectivity mode
    EConnectivite myConnMode; // eNOD|eDESC(eDESC not used)
    //! Let known in what connectivity the cells are writen
    EConnectivite GetConnMode() const { return myConnMode;}

    TElemNum myConn; //!< Table de connectivities
    TElemNum myIndex; //!< Table de indexes

    //! Gives number of the connectivities for the defined polygon
    TInt GetNbConn(TInt theElemId) const;

    //! Gives connectivities for polygon by its number (const version)
    TCConnSlice GetConnSlice(TInt theElemId) const;
    //! Gives connectivities for polygon by its number
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
    //! Let known what MED ENTITY the MED Polyedres belong to
    EEntiteMaillage GetEntity() const { return myEntity;}

    //! Defines the MED Geometric type of the instance
    EGeometrieElement myGeom; // ePOLYEDRE
    //! Let known what MED geometrical type the MED Polyedres belong to
    EGeometrieElement GetGeom() const { return ePOLYEDRE;}

    //! Defines connectivity mode
    EConnectivite myConnMode; // eNOD|eDESC(eDESC not used)
    //! Let known in what connectivity the cells are writen
    EConnectivite GetConnMode() const { return myConnMode;}

    TElemNum myConn; //!< Table de connectivities
    TElemNum myFaces; //!< Table de faces indexes
    TElemNum myIndex; //!< Table de indexes

    //! Gives number of the faces for the defined polyedre (const version)
    TInt GetNbFaces(TInt theElemId) const;
    //! Gives number of the nodes for the defined polyedre
    TInt GetNbNodes(TInt theElemId) const;

    //! Gives sequence of the face connectivities for polyedre by its number (const version)
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
    //! Get a reference to corresponding MED Mesh
    const PMeshInfo& GetMeshInfo() const { return myMeshInfo;}

    ETypeChamp myType; //!< Defines type of the MED Field
    //! Let known what type of the MED FIELD is used
    ETypeChamp GetType() const { return myType;}

    TInt myNbComp; //!< Defines number of components stored in the field
    //! Get number of components for the MED FIELD
    TInt GetNbComp() const { return myNbComp;}

    EBooleen myIsLocal; //!< Defines if the MED Field is local
    //! Let known is the MED FIELD is local or not
    EBooleen GetIsLocal() const { return myIsLocal;}

    TInt myNbRef; //!< Defines number of refereces of the field
    //! Let known number of references for the MED FIELD
    TInt GetNbRef() const { return myNbRef;}

    TString myCompNames; //!< Contains names for each of MED Field components
    //! Get name of the component by its order number
    virtual std::string GetCompName(TInt theId) const = 0;
    //! Set name for the component by its order number
    virtual void SetCompName(TInt theId, const std::string& theValue) = 0;

    TString myUnitNames; //!< Contains units for each of MED Field components
    //! Get unit of the component by its order number
    virtual std::string GetUnitName(TInt theId) const = 0;
    //! Set unit for the component by its order number
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

      bool
      operator()(const TGaussInfo& theLeft, const TGaussInfo& theRight) const;
    };

    //! Defines, which geometrical type the MED Gauss entity belongs to
    EGeometrieElement myGeom; 
    //! Let known what MED geometrical type the MED GAUSS entity belong to
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
  typedef std::map<EGeometrieElement,TInt> TGeom2NbGauss;

  //! Define a base class which represents MED TimeStamp
  struct TTimeStampInfo: 
    virtual TBase
  {
    PFieldInfo myFieldInfo; //!< A reference to correspondig MED Field
    //! Get a reference to corresponding MED Field
    const PFieldInfo& GetFieldInfo() const { return myFieldInfo;}

    //! Defines the MED Entity where the MED TimeStamp belongs to
    EEntiteMaillage myEntity;
    //! Let known to what MED Entity the MED TimeStamp belong to
    EEntiteMaillage GetEntity() const { return myEntity;}

    //! Keeps map of number of cells per geometric type where the MED TimeStamp belongs to
    TGeom2Size myGeom2Size;
    //! Get map of number of cells per geometric type where the MED TimeStamp belongs to
    const TGeom2Size& GetGeom2Size() const { return myGeom2Size;}

    TGeom2NbGauss myGeom2NbGauss; //!< Keeps number of the Gauss Points for the MED TimeStamp
    TInt GetNbGauss(EGeometrieElement theGeom) const; //!< Gives number of the Gauss Points for the MED TimeStamp

    TInt myNumDt; //!< Keeps number in time for the MED TimeStamp
    TInt GetNumDt() const { return myNumDt;} //!< Defines number in time for the MED TimeStamp

    TInt myNumOrd; //!< Keeps number for the MED TimeStamp
    TInt GetNumOrd() const { return myNumOrd;} //!< Defines number for the MED TimeStamp

    TFloat myDt; //!< Keeps time for the MED TimeStamp
    TFloat GetDt() const { return myDt;} //!< Defines time for the MED TimeStamp

    //! Keeps map of MED Gauss entityes per geometric type
    TGeom2Gauss myGeom2Gauss;
    //! Gets a map of MED Gauss entityes per geometric type
    const TGeom2Gauss& GetGeom2Gauss() const { return myGeom2Gauss;}

    TString myUnitDt; //!< Defines unit for the time for the MED TimeStamp
    //! Get unit of time for the MED TimeStamp
    virtual std::string GetUnitDt() const = 0;
    //! Set unit of time for the MED TimeStamp
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
    //! Let known what mode of MED Profile is used
    EModeProfil GetMode() const { return myMode;}
    //! Set mode for the MED Profile
    void SetMode(EModeProfil theMode) { myMode = theMode;}

    TElemNum myElemNum; //!< Keeps sequence of cell by its number which belong to the profile
    //! Get number of mesh elelemts by its order number
    TInt GetElemNum(TInt theId) const;
    //! Set number of mesh elelemts by its order number
    void SetElemNum(TInt theId, TInt theVal);

    //! Let known is the MED Profile defined
    bool IsPresent() const { return GetName() != "";}

    //! Let known size of the MED Profile
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
    //!< Get a reference to correspondig MED TimeStamp
    const PTimeStampInfo& GetTimeStampInfo() const { return myTimeStampInfo;}

    //! Keeps map of MED Profiles per geometric type
    TGeom2Profile myGeom2Profile;
    //! Gets a map of MED Profiles per geometric type
    const TGeom2Profile& GetGeom2Profile() const { return myGeom2Profile;}

    TGeom2Value myGeom2Value;

    //! Keeps map of MED TimeStamp values per geometric type (const version)
    const TMeshValue& GetMeshValue(EGeometrieElement theGeom) const;

    //! Keeps map of MED TimeStamp values per geometric type
    TMeshValue& GetMeshValue(EGeometrieElement theGeom);
  };


  typedef std::map<TInt,TFloatVector> TIndexes;
  typedef std::map<TInt,TString> TNames;
  
  //! Define a base class which represents MED Grille 
  struct TGrilleInfo:
    virtual TModeSwitchInfo
  {

    PMeshInfo myMeshInfo;
    const PMeshInfo& GetMeshInfo() const { return myMeshInfo;} 

    TNodeCoord myCoord; //!< Contains all nodal coordinates, now used only for eGRILLE_STANDARD
    //! Gives coordinates for mesh nodes (const version)
    const TNodeCoord& GetNodeCoord() const;
    TNodeCoord GetNodeCoord();
    //! Gives coordinates for mesh node by its number, array index from 0
    TNodeCoord GetCoord(TInt theId);
    //! Gives ids of nodes for mesh cell by its number, array index from 0
    TIntVector GetConn(TInt theId);

    EGrilleType myGrilleType; //!< Defines grille type (eGRILLE_CARTESIENNE,eGRILLE_POLAIRE,eGRILLE_STANDARD)
    //!Gets grille type (const version)
    const EGrilleType& GetGrilleType() const;
    //!Gets grille type
    EGrilleType GetGrilleType();
    //!Sets grille type
    void SetGrilleType(EGrilleType theGrilleType);


    
    TString myCoordNames; //!< Contains names for the coordinate dimensions
    //! Get name of the coordinate dimension by its order number
    virtual std::string GetCoordName(TInt theId) const = 0 ;
    //! Set name of the coordinate dimension by its order number
    virtual void SetCoordName(TInt theId, const std::string& theValue) = 0;

    TString myCoordUnits; //!< Contains units for the coordinate dimensions
    //! Get name of unit for the coordinate dimension by its order number
    virtual std::string GetCoordUnit(TInt theId) const = 0;
    //! Set name of unit for the coordinate dimension by its order number
    virtual void SetCoordUnit(TInt theId, const std::string& theValue) = 0;


    //! Map of index of axes and Table of indexes for certain axe, now used for eGRILLE_CARTESIENNE and eGRILLE_POLAIRE
    TIndexes myIndixes;
    //!Gets a map of Tables (const version)
    const TIndexes& GetMapOfIndexes() const ;
    //!Gets a map of Tables
    TIndexes GetMapOfIndexes();
    //!Gets a Table of indexes for certain axe(const version)
    const TFloatVector& GetIndexes(TInt theAxisNumber) const;
    //!Gets a Table of indexes for certain axe
    TFloatVector GetIndexes(TInt theAxisNumber);
    //!Gets a number of indices per axe
    TInt GetNbIndexes(TInt theAxisNumber);
    
    TInt GetNbNodes();//! Return count of all points
    TInt GetNbCells();//! Return count of all cells
    EGeometrieElement GetGeom();//! Return geometry of cells (calculated from mesh dimension)
    EEntiteMaillage   GetEntity();//! Return entity (eMAILLE)

    /*!
     *Vector of grille structure (Example: {3,4,5}, 3 nodes in X axe, 4 nodes in Y axe, ...)
     *Used only for eGRILLE_STANDARD
     */
    TIntVector myGrilleStructure;
    //!Gets grille structure(const version)
    const TIntVector& GetGrilleStructure() const;
    //!Gets grille structure
    TIntVector GetGrilleStructure();
    //!Sets the grille structure of theAxis axe to theNb.
    void SetGrilleStructure(TInt theAxis,TInt theNb);
    
    /*!
     *Defines sequence MED Family indexes for corresponding mesh entites
     */
    TElemNum myFamNum; 
    //! Get number of a MED FAMILY by order number of the mesh element
    TInt GetFamNum(TInt theId) const;
    //! Set number of a MED FAMILY for the mesh element with the  order number
    void SetFamNum(TInt theId, TInt theVal);
    
    /*!
     *Defines sequence MED Family indexes for corresponding mesh nodes
     */
    TElemNum myFamNumNode;
    //! Get number of a MED FAMILY by order number of the mesh node
    TInt GetFamNumNode(TInt theId) const;
    //! Set number of a MED FAMILY for the mesh node with the  order number
    void SetFamNumNode(TInt theId, TInt theVal);

  };


}

#endif
