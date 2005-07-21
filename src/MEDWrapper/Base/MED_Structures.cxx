//  Copyright (C) 2003  CEA/DEN, EDF R&D
//
//
//
//  File   : VISU_Structure.cxx
//  Author : Eugeny NIKOLAEV
//  Module : VISU

#include "MED_Structures.hxx"
#include "MED_Utilities.hxx"
using namespace MED;

#if defined __GNUC__
  #if __GNUC__ == 2
    #define __GNUC_2__
  #endif
#endif

#if defined __GNUC_2__
#define GETINDEX(anArray,ind) anArray[ind]
#else
#define GETINDEX(anArray,ind) anArray.at(ind)
#endif

namespace MED{

  TInt
  GetNbNodes(EGeometrieElement typmai)
  {
    return typmai%100;
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

  template<>
  TInt
  GetNbConn<eV2_2>(EGeometrieElement typmai,
		   EEntiteMaillage typent,
		   TInt mdim)
  {
    return typmai%100;
  }

  std::string 
  GetString(TInt theId, 
	    TInt theStep, 
	    const TString& theString)
  {
    const char* aPos = &GETINDEX(theString,theId*theStep);
    TInt aSize = std::min(TInt(strlen(aPos)),theStep);
    return std::string(aPos,aSize);
  }

  void 
  SetString(TInt theId, 
	    TInt theStep, 
	    TString& theString, 
	    const std::string& theValue)
  {
    TInt aSize = std::min(TInt(theValue.size()+1),theStep);
    char* aPos = &GETINDEX(theString,theId*theStep);
    strncpy(aPos,theValue.c_str(),aSize);
  }

  TInt
  GetDimGaussCoord(EGeometrieElement theGeom)
  {
    return theGeom/100;
  }

  TInt
  GetNbRefCoord(EGeometrieElement theGeom)
  {
    return (theGeom%100);
  }

}

//---------------------------------------------------------------
TInt
TFamilyInfo
::GetAttrId(TInt theId) const 
{
  return GETINDEX(myAttrId,theId);
}

TInt
TFamilyInfo
::GetAttrVal(TInt theId) const 
{
  return GETINDEX(myAttrVal,theId);
}

void
TFamilyInfo
::SetAttrId(TInt theId,TInt theVal) 
{
  GETINDEX(myAttrId,theId) = theVal;
}

void
TFamilyInfo
::SetAttrVal(TInt theId,TInt theVal) 
{
  GETINDEX(myAttrVal,theId) = theVal;
}

//---------------------------------------------------------------
TInt
TElemInfo
::GetFamNum(TInt theId) const 
{
  return GETINDEX(myFamNum,theId);
}

TInt
TElemInfo
::GetElemNum(TInt theId) const 
{
  return GETINDEX(myElemNum,theId);
}

void
TElemInfo
::SetFamNum(TInt theId,TInt theVal) 
{
  GETINDEX(myFamNum,theId) = theVal;
}

//---------------------------------------------------------------
TCCoordSlice 
TNodeInfo
::GetCoordSlice(TInt theId) const
{
  TInt aDim = myMeshInfo->GetDim();
  if(GetModeSwitch() == eFULL_INTERLACE)
    return TCCoordSlice(myCoord,std::slice(theId*aDim,aDim,1));
  else
    return TCCoordSlice(myCoord,std::slice(theId,aDim,aDim));
}

TCoordSlice 
TNodeInfo
::GetCoordSlice(TInt theId)
{
  TInt aDim = myMeshInfo->GetDim();
  if(GetModeSwitch() == eFULL_INTERLACE)
    return TCoordSlice(myCoord,std::slice(theId*aDim,aDim,1));
  else
    return TCoordSlice(myCoord,std::slice(theId,aDim,aDim));
}

TFloat
TNodeInfo
::GetNodeCoord(TInt theId,TInt theComp) const 
{
  return GETINDEX(myCoord,myMeshInfo->myDim*theId + theComp);
}

void
TNodeInfo
::SetNodeCoord(TInt theId,TInt theComp,TFloat theVal) 
{
  GETINDEX(myCoord,myMeshInfo->myDim*theId + theComp) = theVal;
}

//---------------------------------------------------------------
TInt
TCellInfo
::GetConn(TInt theElemId, TInt theConnId) const 
{
  return GETINDEX(myConn,GetConnDim()*theElemId + theConnId);
}

void
TCellInfo
::SetConn(TInt theElemId, TInt theConnId, TInt theVal)
{
  GETINDEX(myConn,GetConnDim()*theElemId + theConnId) = theVal;
}

TCConnSlice 
TCellInfo
::GetConnSlice(TInt theElemId) const
{
  return TCConnSlice(myConn,std::slice(GetConnDim()*theElemId,GetNbNodes(myGeom),1));
}

TConnSlice 
TCellInfo
::GetConnSlice(TInt theElemId)
{
  return TConnSlice(myConn,std::slice(GetConnDim()*theElemId,GetNbNodes(myGeom),1));
}

//---------------------------------------------------------------
TInt
TPolygoneInfo
::GetNbConn(TInt theElemId) const 
{
  TInt i1 = GETINDEX(myIndex,theElemId);
  TInt i2 = GETINDEX(myIndex,theElemId+1);
  TInt ret = i2 - i1;
  return ret;
}

//---------------------------------------------------------------
TInt 
TPolyedreInfo
::GetNbConn(TInt theElemId) const 
{
  TInt ind1 = GETINDEX(myIndex,theElemId);
  TInt ind2 = GETINDEX(myIndex,theElemId+1);

  TInt inf1 = GETINDEX(myFaces,ind1-1);
  TInt inf2 = GETINDEX(myFaces,ind2-1);

  TInt ret = inf2-inf1;
  return ret;
}

//---------------------------------------------------------------
TInt
TProfileInfo
::GetElemNum(TInt theId) const 
{
  return GETINDEX(myElemNum,theId);
}

void
TProfileInfo
::SetElemNum(TInt theId,TInt theVal) 
{
  GETINDEX(myElemNum,theId) = theVal;
}

//---------------------------------------------------------------
bool
TGaussInfo::TLess
::operator()(const TKey& theLeft, const TKey& theRight) const
{
  EGeometrieElement aLGeom = boost::get<0>(theLeft);
  EGeometrieElement aRGeom = boost::get<0>(theRight);
  if(aLGeom != aRGeom)
    return aLGeom < aRGeom;

  const std::string& aLStr = boost::get<1>(theLeft);
  const std::string& aRStr = boost::get<1>(theRight);
  return aLStr < aRStr;
}

TCCoordSlice 
TGaussInfo
::GetRefCoordSlice(TInt theId) const
{
  if(GetModeSwitch() == eFULL_INTERLACE)
    return TCCoordSlice(myRefCoord,std::slice(theId*GetDim(),GetDim(),1));
  else
    return TCCoordSlice(myRefCoord,std::slice(theId,GetDim(),GetDim()));
}

TCoordSlice 
TGaussInfo
::GetRefCoordSlice(TInt theId)
{
  if(GetModeSwitch() == eFULL_INTERLACE)
    return TCoordSlice(myRefCoord,std::slice(theId*GetDim(),GetDim(),1));
  else
    return TCoordSlice(myRefCoord,std::slice(theId,GetDim(),GetDim()));
}

TCCoordSlice 
TGaussInfo
::GetGaussCoordSlice(TInt theId) const
{
  if(GetModeSwitch() == eFULL_INTERLACE)
    return TCCoordSlice(myGaussCoord,std::slice(theId*GetDim(),GetDim(),1));
  else
    return TCCoordSlice(myGaussCoord,std::slice(theId,GetDim(),GetDim()));
}

TCoordSlice 
TGaussInfo
::GetGaussCoordSlice(TInt theId)
{
  if(GetModeSwitch() == eFULL_INTERLACE)
    return TCoordSlice(myGaussCoord,std::slice(theId*GetDim(),GetNbGauss(),1));
  else
    return TCoordSlice(myGaussCoord,std::slice(theId,GetNbGauss(),GetDim()));
}


//---------------------------------------------------------------
TFloat
TTimeStampVal
::GetVal(EGeometrieElement theGeom, 
	 TInt theId, 
	 TInt theComp, 
	 TInt theGauss) const 
{
  TInt aNbComp = myTimeStampInfo->myFieldInfo->myNbComp;
  TInt aNbGauss = myTimeStampInfo->myNbGauss;
  TInt aStep = aNbComp*aNbGauss;
  TMeshValue::const_iterator anIter = myMeshValue.find(theGeom);
  if(anIter != myMeshValue.end()){
    TFloat aRet=GETINDEX(anIter->second,theId*aStep + theComp*aNbGauss + theGauss);
    return aRet;
  }
  return TFloat();
}

void
TTimeStampVal
::SetVal(EGeometrieElement theGeom, 
	 TInt theId, 
	 TInt theComp, 
	 TFloat theVal, 
	 TInt theGauss)
{
  TInt aNbComp = myTimeStampInfo->myFieldInfo->myNbComp;
  TInt aNbGauss = myTimeStampInfo->myNbGauss;
  TInt aStep = aNbComp*aNbGauss;
  GETINDEX(myMeshValue[theGeom],theId*aStep + theComp*aNbGauss + theGauss) = theVal;
}
