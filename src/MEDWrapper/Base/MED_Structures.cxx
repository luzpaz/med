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

namespace MED
{
  TInt
  GetNbNodes(EGeometrieElement typmai)
  {
    return typmai%100;
  }

  std::string 
  GetString(TInt theId, 
	    TInt theStep, 
	    const TString& theString)
  {
    const char* aPos = &theString[theId*theStep];
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
    char* aPos = &theString[theId*theStep];
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
  return myAttrId[theId];
}

TInt
TFamilyInfo
::GetAttrVal(TInt theId) const 
{
  return myAttrVal[theId];
}

void
TFamilyInfo
::SetAttrId(TInt theId,TInt theVal) 
{
  myAttrId[theId] = theVal;
}

void
TFamilyInfo
::SetAttrVal(TInt theId,TInt theVal) 
{
  myAttrVal[theId] = theVal;
}

//---------------------------------------------------------------
TInt
TElemInfo
::GetFamNum(TInt theId) const 
{
  return myFamNum[theId];
}

TInt
TElemInfo
::GetElemNum(TInt theId) const 
{
  return myElemNum[theId];
}

void
TElemInfo
::SetFamNum(TInt theId,TInt theVal) 
{
  myFamNum[theId] = theVal;
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

//---------------------------------------------------------------
TCConnSlice 
TCellInfo
::GetConnSlice(TInt theElemId) const
{
  if(GetModeSwitch() == eFULL_INTERLACE)
    return TCConnSlice(myConn,std::slice(GetConnDim()*theElemId,GetNbNodes(myGeom),1));
  else
    return TCConnSlice(myConn,std::slice(theElemId,GetNbNodes(myGeom),GetConnDim()));
}

TConnSlice 
TCellInfo
::GetConnSlice(TInt theElemId)
{
  if(GetModeSwitch() == eFULL_INTERLACE)
    return TConnSlice(myConn,std::slice(GetConnDim()*theElemId,GetNbNodes(myGeom),1));
  else
    return TConnSlice(myConn,std::slice(theElemId,GetNbNodes(myGeom),GetConnDim()));
}


//---------------------------------------------------------------
TInt
TPolygoneInfo
::GetNbConn(TInt theElemId) const 
{
  return myIndex[theElemId + 1] - myIndex[theElemId];
}

TCConnSlice 
TPolygoneInfo
::GetConnSlice(TInt theElemId) const
{
  return TCConnSlice(myConn,std::slice(myIndex[theElemId]-1,GetNbConn(theElemId),1));
}

TConnSlice 
TPolygoneInfo
::GetConnSlice(TInt theElemId)
{
  return TConnSlice(myConn,std::slice(myIndex[theElemId]-1,GetNbConn(theElemId),1));
}


//---------------------------------------------------------------
TInt 
TPolyedreInfo
::GetNbFaces(TInt theElemId) const 
{
  return myIndex[theElemId+1] - myIndex[theElemId];
}

TInt 
TPolyedreInfo
::GetNbNodes(TInt theElemId) const 
{
  TInt aNbNodes = 0;
  TInt aNbFaces = GetNbFaces(theElemId);
  TInt aStartId = myIndex[theElemId] - 1;
  for(TInt aFaceId = 0; aFaceId < aNbFaces; aFaceId++){
    TInt aFinishId = myIndex[theElemId + 1] - 1;
    aNbNodes += aFinishId - aStartId;
    aStartId = aFinishId;
  }
  return aNbNodes;
}

TCConnSliceArr 
TPolyedreInfo
::GetConnSliceArr(TInt theElemId) const
{
  TInt aNbFaces = GetNbFaces(theElemId);
  TCConnSliceArr aConnSliceArr(aNbFaces);
  TInt aStartId = myIndex[theElemId] - 1;
  for(TInt aFaceId = 0; aFaceId < aNbFaces; aFaceId++){
    TInt aFinishId = myIndex[theElemId + 1] - 1;
    TInt aDiff = aFinishId - aStartId;
    aConnSliceArr[aFaceId] =
      TCConnSlice(myConn,std::slice(aStartId,aDiff,1));
    aStartId = aFinishId;
  }
  return aConnSliceArr;
}

TConnSliceArr 
TPolyedreInfo
::GetConnSliceArr(TInt theElemId)
{
  TInt aNbFaces = GetNbFaces(theElemId);
  TConnSliceArr aConnSliceArr(aNbFaces);
  TInt aStartId = myIndex[theElemId] - 1;
  for(TInt aFaceId = 0; aFaceId < aNbFaces; aFaceId++){
    TInt aFinishId = myIndex[theElemId+1] - 1;
    TInt aDiff = aFinishId - aStartId;
    aConnSliceArr[aFaceId] =
      TConnSlice(myConn,std::slice(aStartId,aDiff,1));
    aStartId = aFinishId;
  }
  return aConnSliceArr;
}


//---------------------------------------------------------------
TInt
TProfileInfo
::GetElemNum(TInt theId) const 
{
  return myElemNum[theId];
}

void
TProfileInfo
::SetElemNum(TInt theId,TInt theVal) 
{
  myElemNum[theId] = theVal;
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

bool
TGaussInfo::TLess
::operator()(const TGaussInfo& theLeft, const TGaussInfo& theRight) const
{
  if(theLeft.myGeom != theRight.myGeom)
    return theLeft.myGeom < theRight.myGeom;

  if(theLeft.myRefCoord != theRight.myRefCoord)
    return theLeft.myRefCoord < theRight.myRefCoord;

  return theLeft.myGaussCoord < theRight.myGaussCoord;
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
void
TMeshValue
::Init(TInt theNbElem,
       TInt theNbGauss,
       TInt theNbComp,
       EModeSwitch theMode)
{
  myModeSwitch = theMode;
  
  myNbElem = theNbElem;
  myNbGauss = theNbGauss;
  myNbComp = theNbComp;
  
  myStep = theNbComp*theNbGauss;
  
  myValue.resize(theNbElem*myStep);
}

//---------------------------------------------------------------
TCValueSliceArr 
TMeshValue
::GetGaussValueSliceArr(TInt theElemId) const
{
  TCValueSliceArr aValueSliceArr(myNbGauss);
  if(GetModeSwitch() == eFULL_INTERLACE){
    TInt anId = theElemId*myStep;
    for(TInt aGaussId = 0; aGaussId < myNbGauss; aGaussId++){
      aValueSliceArr[aGaussId] =
	TCValueSlice(myValue,std::slice(anId,myNbComp,1));
      anId += myNbComp;
    }
  }
  else{
    for(TInt aGaussId = 0; aGaussId < myNbGauss; aGaussId++){
      aValueSliceArr[aGaussId] =
	TCValueSlice(myValue,std::slice(theElemId,myNbComp,myStep));
    }
  }
  return aValueSliceArr;
}

TValueSliceArr 
TMeshValue
::GetGaussValueSliceArr(TInt theElemId)
{
  TValueSliceArr aValueSliceArr(myNbGauss);
  if(GetModeSwitch() == eFULL_INTERLACE){
    TInt anId = theElemId*myStep;
    for(TInt aGaussId = 0; aGaussId < myNbGauss; aGaussId++){
      aValueSliceArr[aGaussId] =
	TValueSlice(myValue,std::slice(anId,myNbComp,1));
      anId += myNbComp;
    }
  }
  else{
    for(TInt aGaussId = 0; aGaussId < myNbGauss; aGaussId++){
      aValueSliceArr[aGaussId] =
	TValueSlice(myValue,std::slice(theElemId,myNbComp,myStep));
    }
  }
  return aValueSliceArr;
}

//---------------------------------------------------------------
TCValueSliceArr 
TMeshValue
::GetCompValueSliceArr(TInt theElemId) const
{
  TCValueSliceArr aValueSliceArr(myNbComp);
  if(GetModeSwitch() == eFULL_INTERLACE){
    TInt anId = theElemId*myStep;
    for(TInt aCompId = 0; aCompId < myNbComp; aCompId++){
      aValueSliceArr[aCompId] =
	TCValueSlice(myValue,std::slice(anId,myNbGauss,myNbComp));
      anId += 1;
    }
  }
  else{
    for(TInt aCompId = 0; aCompId < myNbComp; aCompId++){
      aValueSliceArr[aCompId] =
	TCValueSlice(myValue,std::slice(theElemId,myNbGauss,myStep));
    }
  }
  return aValueSliceArr;
}


TValueSliceArr 
TMeshValue
::GetCompValueSliceArr(TInt theElemId)
{
  TValueSliceArr aValueSliceArr(myNbGauss);
  if(GetModeSwitch() == eFULL_INTERLACE){
    TInt anId = theElemId*myStep;
    for(TInt aCompId = 0; aCompId < myNbComp; aCompId++){
      aValueSliceArr[aCompId] =
	TValueSlice(myValue,std::slice(anId,myNbGauss,myNbComp));
      anId += 1;
    }
  }
  else{
    for(TInt aGaussId = 0; aGaussId < myNbGauss; aGaussId++){
      aValueSliceArr[aGaussId] =
	TValueSlice(myValue,std::slice(theElemId,myNbComp,myStep));
    }
  }
  return aValueSliceArr;
}


//---------------------------------------------------------------
const TMeshValue& 
TTimeStampVal
::GetMeshValue(EGeometrieElement theGeom) const
{
  TGeom2Value::const_iterator anIter = myGeom2Value.find(theGeom);
  if(anIter == myGeom2Value.end())
    EXCEPTION(runtime_error,"TTimeStampVal::GetMeshValue - myGeom2Value.find(theGeom) fails");
  return anIter->second;
}

TMeshValue& 
TTimeStampVal
::GetMeshValue(EGeometrieElement theGeom)
{
  return myGeom2Value[theGeom];
}
