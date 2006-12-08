//  Copyright (C) 2003  CEA/DEN, EDF R&D
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : MED_Structure.cxx
//  Author : Eugeny NIKOLAEV
//  Module : MED

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
  TInt aStartFaceId = myIndex[theElemId] - 1;
  for(TInt aFaceId = 0; aFaceId < aNbFaces; aFaceId++, aStartFaceId++){
    TInt aCurrentId = myFaces[aStartFaceId];
    TInt aDiff = myFaces[aStartFaceId + 1] - aCurrentId;
    aNbNodes += aDiff;
  }
  return aNbNodes;
}

TCConnSliceArr 
TPolyedreInfo
::GetConnSliceArr(TInt theElemId) const
{
  TInt aNbFaces = GetNbFaces(theElemId);
  TCConnSliceArr aConnSliceArr(aNbFaces);
  TInt aStartFaceId = myIndex[theElemId] - 1;
  for(TInt aFaceId = 0; aFaceId < aNbFaces; aFaceId++, aStartFaceId++){
    TInt aCurrentId = myFaces[aStartFaceId];
    TInt aDiff = myFaces[aStartFaceId + 1] - aCurrentId;
    aConnSliceArr[aFaceId] =
      TCConnSlice(myConn,std::slice(aCurrentId - 1,aDiff,1));
  }
  return aConnSliceArr;
}

TConnSliceArr 
TPolyedreInfo
::GetConnSliceArr(TInt theElemId)
{
  TInt aNbFaces = GetNbFaces(theElemId);
  TConnSliceArr aConnSliceArr(aNbFaces);
  TInt aStartFaceId = myIndex[theElemId] - 1;
  for(TInt aFaceId = 0; aFaceId < aNbFaces; aFaceId++, aStartFaceId++){
    TInt aCurrentId = myFaces[aStartFaceId];
    TInt aDiff = myFaces[aStartFaceId + 1] - aCurrentId;
    aConnSliceArr[aFaceId] =
      TConnSlice(myConn,std::slice(aCurrentId - 1,aDiff,1));
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
  if(!&theLeft)
    return true;

  if(!&theRight)
    return false;

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
TInt
TTimeStampInfo
::GetNbGauss(EGeometrieElement theGeom) const
{
  TGeom2NbGauss::const_iterator anIter = myGeom2NbGauss.find(theGeom);
  if(anIter == myGeom2NbGauss.end())
    return 1;//EXCEPTION(runtime_error,"TTimeStampInfo::GetNbGauss - myGeom2NbGauss.find(theGeom) fails");

  return anIter->second;
}


//---------------------------------------------------------------
// TGrilleInfo structure methods
//---------------------------------------------------------------
const EGrilleType&
TGrilleInfo
::GetGrilleType() const
{
  return myGrilleType;
}

EGrilleType
TGrilleInfo
::GetGrilleType()
{
  return myGrilleType;
}

void
TGrilleInfo
::SetGrilleType(EGrilleType theGrilleType)
{
  myGrilleType = theGrilleType;
}

const
TIndexes&
TGrilleInfo
::GetMapOfIndexes() const
{
  return myIndixes;
}

TIndexes&
TGrilleInfo
::GetMapOfIndexes()
{
  return myIndixes;
}

const
TFloatVector&
TGrilleInfo
::GetIndexes(TInt theAxisNumber) const
{
  TIndexes::const_iterator aIter=myIndixes.find(theAxisNumber);
  if(aIter==myIndixes.end())
    EXCEPTION(runtime_error,"const TGrilleInfo::GetIndexes - myIndixes.find(theAxisNumber); fails");
  return aIter->second;
}

TFloatVector&
TGrilleInfo
::GetIndexes(TInt theAxisNumber)
{
  TIndexes::iterator aIter=myIndixes.find(theAxisNumber);
  if(aIter==myIndixes.end())
    EXCEPTION(runtime_error,"TGrilleInfo::GetIndexes - myIndixes.find(theAxisNumber="<<theAxisNumber<<"); fails");
  return aIter->second;
}

TInt
TGrilleInfo
::GetNbIndexes(TInt theAxisNumber)
{
  const TFloatVector& aVector=GetIndexes(theAxisNumber);
  return aVector.size();
}

TInt
TGrilleInfo
::GetNbNodes()
{
  TInt nbNodes=0;
  TInt aDim = myMeshInfo->GetDim();
  if(myGrilleType == eGRILLE_STANDARD)
    for(int i=0;i<aDim;i++)
      if(nbNodes == 0)
	nbNodes = this->GetGrilleStructure()[i];
      else
	nbNodes = nbNodes*this->GetGrilleStructure()[i];
  else
    for(int i=0;i<aDim;i++)
      if(nbNodes == 0)
	nbNodes = GetNbIndexes(i);
      else
	nbNodes = nbNodes*GetNbIndexes(i);
  
  return nbNodes;
}

TInt
TGrilleInfo
::GetNbCells()
{
  TInt nbCells=0;
  TInt aDim = myMeshInfo->GetDim();
  if(this->GetGrilleType() == eGRILLE_STANDARD)
    for(int i=0;i<aDim;i++)
      if(nbCells == 0)
	nbCells = this->GetGrilleStructure()[i]-1;
      else
	nbCells = nbCells*(this->GetGrilleStructure()[i]-1);
  else
    for(int i=0;i<aDim;i++)
      if(nbCells == 0)
	nbCells = GetNbIndexes(i)-1;
      else
	nbCells = nbCells*(GetNbIndexes(i)-1);
  return nbCells;
}

EGeometrieElement
TGrilleInfo
::GetGeom()
{
  TInt aDim = myMeshInfo->GetDim();
  switch(aDim){
  case 1:
    return eSEG2;
  case 2:
    return eQUAD4;
  case 3:
    return eHEXA8;
  default:
    return eNONE;
  }
}

EEntiteMaillage
TGrilleInfo
::GetEntity()
{
  return eMAILLE;
}

const
TIntVector&
TGrilleInfo
::GetGrilleStructure() const
{
  return myGrilleStructure;
}

TIntVector
TGrilleInfo
::GetGrilleStructure()
{
  return myGrilleStructure;
}

void
TGrilleInfo
::SetGrilleStructure(TInt theAxis,TInt theNb)
{
  if(theAxis >= 0 && theAxis <=2 && theNb >= 0)
  myGrilleStructure[theAxis]=theNb;
}

const
TNodeCoord&
TGrilleInfo
::GetNodeCoord() const
{
  return myCoord;
}

TNodeCoord&
TGrilleInfo
::GetNodeCoord()
{
  return myCoord;
}

TNodeCoord
TGrilleInfo
::GetCoord(TInt theId)
{
  TNodeCoord aCoord;
  TInt aDim       = myMeshInfo->GetDim();
  TInt aNbNodes   = this->GetNbNodes();
  aCoord.resize(aDim);
  
  if(theId >= aNbNodes)
    EXCEPTION(runtime_error,"TGrilleInfo::GetCoord - theId out of range");

  if(myGrilleType == eGRILLE_STANDARD){
    switch(aDim){
    case 3:
      aCoord[2] = myCoord[aDim*theId+2];
    case 2:
      aCoord[1] = myCoord[aDim*theId+1];
    case 1:{
      aCoord[0] = myCoord[aDim*theId];
      break;
    }
    }
  } else {

    TFloatVector aVecX  = this->GetIndexes(0);
    TInt nbIndxX        = this->GetNbIndexes(0);
    
    switch(aDim){
    case 1:{
      aCoord[0] = aVecX[theId];
      break;
    }
    case 2:{
      TFloatVector aVecY = this->GetIndexes(1);
      TInt i,j,k;
      i = j = k = 0;
      i = theId % nbIndxX;
      j = theId / nbIndxX;
      if(myGrilleType == eGRILLE_CARTESIENNE){
	aCoord[0] = aVecX[i];
	aCoord[1] = aVecY[j];
      } else { // eGRILLE_POLAIRE (cylindrical)
	aCoord[0] = aVecX[i] * cos(aVecY[j]);
	aCoord[1] = aVecX[i] * sin(aVecY[j]);
      }
      break;
    }
    case 3:{
      TFloatVector aVecY = this->GetIndexes(1);
      TInt nbIndxY       = this->GetNbIndexes(1);
      TFloatVector aVecZ = this->GetIndexes(2);
      TInt i,j,k;
      i = j = k = 0;
      
      i = theId % nbIndxX;
      j = (theId / nbIndxX) % nbIndxY;
      k = theId / (nbIndxX*nbIndxY);

      if(myGrilleType == eGRILLE_CARTESIENNE){
	aCoord[0] = aVecX[i];
	aCoord[1] = aVecY[j];
	aCoord[2] = aVecZ[k];
      } else { // eGRILLE_POLAIRE (cylindrical)
	aCoord[0] = aVecX[i] * cos(aVecY[j]);
	aCoord[1] = aVecX[i] * sin(aVecY[j]);
	aCoord[2] = aVecZ[k];
      }
      
      break;
    }
    }
  }
    
  return aCoord;
}

TIntVector
TGrilleInfo
::GetConn(TInt theId)
{
  TIntVector anIndexes;
  TInt aDim       = myMeshInfo->GetDim();
  TInt aArrSize   = 2;
  for(int i=1;i<aDim;i++) aArrSize = aArrSize*2;
  
  TInt idx;
  TInt iMin, iMax, jMin, jMax, kMin, kMax;
  TInt loc[3];

  loc[0] = loc[1] = loc[2] = 0;
  iMin = iMax = jMin = jMax = kMin = kMax = 0;
 
  TInt nbX,nbY;
  if (myGrilleType == eGRILLE_STANDARD)
    {
      nbX = this->GetGrilleStructure()[0];
      nbY = this->GetGrilleStructure()[1];
    }
  else
    {
      nbX = this->GetNbIndexes(0);
      nbY = this->GetNbIndexes(1);
    }

  TInt d01 = nbX*nbY;
  
  switch(aDim){
  case 3:{
    iMin = theId % (nbX - 1);
    iMax = iMin + 1;
    jMin = (theId / (nbX - 1)) % (nbY - 1);
    jMax = jMin + 1;
    kMin = theId / ((nbX - 1) * (nbY - 1));
    kMax = kMin + 1;
    break;
  }
  case 2:{
    iMin = theId % (nbX-1);
    iMax = iMin + 1;
    jMin = theId / (nbX-1);
    jMax = jMin + 1;
    break;
  }
  case 1:{
    iMin = theId;
    iMax = theId + 1;
    break;
  }
  }
  
  
  for (loc[2]=kMin; loc[2]<=kMax; loc[2]++)
    {
      for (loc[1]=jMin; loc[1]<=jMax; loc[1]++)
	{
	  for (loc[0]=iMin; loc[0]<=iMax; loc[0]++)
	    {
	      idx = loc[0] + loc[1]*nbX + loc[2]*d01;
	      anIndexes.push_back(idx);
	    }
	}
    }
  
  return anIndexes;
}

TInt
TGrilleInfo
::GetFamNumNode(TInt theId) const 
{
  return myFamNumNode[theId];
}

void
TGrilleInfo
::SetFamNumNode(TInt theId,TInt theVal) 
{
  myFamNumNode[theId] = theVal;
}

TInt
TGrilleInfo
::GetFamNum(TInt theId) const 
{
  return myFamNum[theId];
}

void
TGrilleInfo
::SetFamNum(TInt theId,TInt theVal) 
{
  myFamNum[theId] = theVal;
}
