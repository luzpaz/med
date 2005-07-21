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

#ifndef MED_Common_HeaderFile
#define MED_Common_HeaderFile

#include <vector>
#include <string>
#include <set>
#include <map>

extern "C"{
#include <hdf5.h>
}  

#include <boost/tuple/tuple.hpp>

#include "SALOMEconfig.h"

#include "MED_SharedPtr.hxx"
#include "MED_SliceArray.hxx"

namespace MED{

  enum EVersion {eVUnknown = -1, eV2_1, eV2_2};
  
  typedef enum {eFAUX, eVRAI} EBooleen ; 
  typedef double TFloat;
#if defined(SUN4SOL2) || defined(PCLINUX) || defined(OSF1_32) || defined(IRIX64_32) || defined(RS6000) || defined(HP9000)
  typedef int TInt;
#endif
#if defined(IRIX64) || defined(OSF1)
  typedef long TInt;
#endif
  typedef hid_t TIdt;
  typedef herr_t TErr;

  typedef enum {eFULL_INTERLACE, eNO_INTERLACE} EModeSwitch;

  typedef enum {eFLOAT64=6, eINT=26} ETypeChamp;

  typedef enum {eNON_STRUCTURE, eSTRUCTURE} EMaillage;

  typedef enum {eCART, eCYL, eSPHER} ERepere; 

  typedef enum {eNOD, eDESC} EConnectivite ; 

  typedef enum {ePOINT1=1, eSEG2=102, eSEG3=103, eTRIA3=203,
		eQUAD4=204, eTRIA6=206,eQUAD8=208, eTETRA4=304,
		ePYRA5=305, ePENTA6=306, eHEXA8=308, eTETRA10=310, 
		ePYRA13=313, ePENTA15=315, eHEXA20=320, 
		ePOLYGONE=400, ePOLYEDRE=500, eNONE=0} EGeometrieElement;

  typedef enum {eMAILLE, eFACE, eARETE, eNOEUD} EEntiteMaillage; 

  typedef enum {eNO_PFLMOD, eGLOBAL, eCOMPACT}  EModeProfil; 

  typedef std::vector<TFloat> TFloatVector;
  typedef std::vector<std::string> TStringVector;
  typedef std::vector<TInt> TIntVector;
  typedef std::set<std::string> TStringSet;
  
  typedef std::map<EGeometrieElement,TInt> TGeom2Size;
  typedef std::map<EEntiteMaillage,TGeom2Size> TEntityInfo;

  typedef std::set<EGeometrieElement> TGeomSet;
  typedef std::map<EEntiteMaillage,TGeomSet> TEntity2GeomSet;

  const TEntity2GeomSet& 
  GetEntity2GeomSet();

  template<int>
  TInt
  GetNbConn(EGeometrieElement typmai,
	    EEntiteMaillage typent,
	    TInt mdim);
  
  template<>
  TInt
  GetNbConn<eV2_1>(EGeometrieElement typmai,
		   EEntiteMaillage typent,
		   TInt mdim);
  
  template<>
  TInt
  GetNbConn<eV2_2>(EGeometrieElement typmai,
		   EEntiteMaillage typent,
		   TInt mdim);

  TInt 
  GetNbNodes(EGeometrieElement typmai);

  struct TNameInfo;
  typedef SharedPtr<TNameInfo> PNameInfo;
  
  struct TMeshInfo;
  typedef SharedPtr<TMeshInfo> PMeshInfo;
  
  struct TFamilyInfo;
  typedef SharedPtr<TFamilyInfo> PFamilyInfo;

  struct TElemInfo;
  typedef SharedPtr<TElemInfo> PElemInfo;

  struct TNodeInfo;
  typedef SharedPtr<TNodeInfo> PNodeInfo;

  struct TPolygoneInfo;
  typedef SharedPtr<TPolygoneInfo> PPolygoneInfo;

  struct TPolyedreInfo;
  typedef SharedPtr<TPolyedreInfo> PPolyedreInfo;

  struct TCellInfo;
  typedef SharedPtr<TCellInfo> PCellInfo;

  struct TFieldInfo;
  typedef SharedPtr<TFieldInfo> PFieldInfo;

  struct TTimeStampInfo;
  typedef SharedPtr<TTimeStampInfo> PTimeStampInfo;

  struct TProfileInfo;
  typedef SharedPtr<TProfileInfo> PProfileInfo;
  
  struct TGaussInfo;
  typedef SharedPtr<TGaussInfo> PGaussInfo;
  
  struct TTimeStampVal;
  typedef SharedPtr<TTimeStampVal> PTimeStampVal;

  class TWrapper;
  typedef SharedPtr<TWrapper> PWrapper;
};


#endif
