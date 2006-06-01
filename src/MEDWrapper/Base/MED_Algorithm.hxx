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

#ifndef MED_Algorithm_HeaderFile
#define MED_Algorithm_HeaderFile

#include "MED_Structures.hxx"

#include <set>

namespace MED
{
  //---------------------------------------------------------------
  typedef std::map<EGeometrieElement,PElemInfo> TGeom2ElemInfo;
  typedef std::map<EEntiteMaillage,TGeom2ElemInfo> TEntity2TGeom2ElemInfo;

  //! Get set of TElemInfo by its geometrical type and corresponding MED ENTITY
  TEntity2TGeom2ElemInfo
  GetEntity2TGeom2ElemInfo(const PWrapper& theWrapper, 
			   const PMeshInfo& theMeshInfo,
			   const MED::TEntityInfo& theEntityInfo);


  //---------------------------------------------------------------
  typedef std::set<PFamilyInfo> TFamilyInfoSet;

  //! Read set of MED FAMILIES for defined MED file
  TFamilyInfoSet
  GetFamilyInfoSet(const PWrapper& theWrapper, 
		   const PMeshInfo& theMeshInfo);
  

  //---------------------------------------------------------------
  typedef boost::tuple<PFamilyInfo,TInt> TFamilyTSize;

  bool
  operator<(const TFamilyTSize& theLeft, const TFamilyTSize& theRight);
  typedef std::set<TFamilyTSize> TFamilyTSizeSet;


  //---------------------------------------------------------------
  typedef std::map<EEntiteMaillage,TFamilyTSizeSet> TEntity2FamilySet;
  
  //! Split set of MED FAMILIES by corresponding MED ENTITY
  TEntity2FamilySet 
  GetEntity2FamilySet(const PWrapper& theWrapper, 
		      const TEntity2TGeom2ElemInfo& theEntity2TGeom2ElemInfo,
		      const TFamilyInfoSet& theFamilyInfoSet);
  

  //---------------------------------------------------------------
  typedef std::map<std::string,TFamilyInfoSet> TGroupInfo;
  
  //! Split the input set of MED FAMILIES by corresponding MED GROUPS
  TGroupInfo
  GetGroupInfo(const TFamilyInfoSet& theFamilyInfoSet);
  
  
  //---------------------------------------------------------------
  typedef std::set<PTimeStampInfo> TTimeStampInfoSet;
  typedef std::map<PFieldInfo,TTimeStampInfoSet> TFieldInfo2TimeStampInfoSet;

  //! Read set of MED TIMESTAMPS groupped by corresponding MED FIELDS
  TFieldInfo2TimeStampInfoSet
  GetFieldInfo2TimeStampInfoSet(const PWrapper& theWrapper, 
				const PMeshInfo& theMeshInfo,
				const MED::TEntityInfo& theEntityInfo);
  

  //---------------------------------------------------------------
  typedef std::map<EEntiteMaillage,TFieldInfo2TimeStampInfoSet> TEntite2TFieldInfo2TimeStampInfoSet;

  //! Split the input set of MED TIMESTAMPS by corresponding MED FIELDS and MED ENTITIES
  TEntite2TFieldInfo2TimeStampInfoSet
  GetEntite2TFieldInfo2TimeStampInfoSet(const TFieldInfo2TimeStampInfoSet& theFieldInfo2TimeStampInfoSet);


  //---------------------------------------------------------------
  typedef std::map<TGaussInfo::TKey,PGaussInfo,TGaussInfo::TLess> TKey2Gauss;

  //! Read set of MED GAUSS
  TKey2Gauss
  GetKey2Gauss(const PWrapper& theWrapper, 
	       TErr* theErr = NULL,
	       EModeSwitch theMode = eFULL_INTERLACE);


  //---------------------------------------------------------------
  //! Get MED PROFILE by its name
  PProfileInfo
  GetProfileInfo(const PWrapper& theWrapper, 
		 const std::string& theProfileName,
		 TErr* theErr = NULL,
		 EModeProfil theMode = eCOMPACT);


  //---------------------------------------------------------------
  typedef std::map<TProfileInfo::TKey,PProfileInfo> TKey2Profile;
  typedef boost::tuple<EModeProfil,TKey2Profile> TMKey2Profile;

  //! Read set of MED PROFILES
  TMKey2Profile
  GetMKey2Profile(const PWrapper& theWrapper, 
		  TErr* theErr = NULL,
		  EModeProfil theMode = eCOMPACT);
}

#endif
