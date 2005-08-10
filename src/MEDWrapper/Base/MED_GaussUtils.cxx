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

#include "MED_GaussUtils.hxx"
#include "MED_Utilities.hxx"
 
#ifdef _DEBUG_
static int MYDEBUG = 0;
static int MYVALUEDEBUG = 0;
#else
static int MYDEBUG = 0;
static int MYVALUEDEBUG = 0;
#endif

//#define _DEBUG_REF_COORDS_

namespace MED
{
  //---------------------------------------------------------------
  TGaussCoord
  ::TGaussCoord():
    TModeSwitchInfo(eFULL_INTERLACE),
    myNbElem(0),
    myNbGauss(0),
    myDim(0),
    myGaussStep(0)
  {
  }

  void
  TGaussCoord
  ::Init(TInt theNbElem,
	 TInt theNbGauss,
	 TInt theDim,
	 EModeSwitch theMode)
  {
    myModeSwitch = theMode;

    myNbElem = theNbElem;
    myNbGauss = theNbGauss;
    myDim = theDim;

    myGaussStep = myNbGauss*myDim;

    myGaussCoord.resize(theNbElem*myGaussStep);
  }


  TCCoordSliceArr 
  TGaussCoord
  ::GetCoordSliceArr(TInt theElemId) const
  {
    TCCoordSliceArr aCoordSliceArr(myNbGauss);
    if(GetModeSwitch() == eFULL_INTERLACE){
      TInt anId = theElemId*myGaussStep;
      for(TInt anGaussId = 0; anGaussId < myNbGauss; anGaussId++){
	aCoordSliceArr[anGaussId] =
	  TCCoordSlice(myGaussCoord,std::slice(anId,myDim,1));
	anId += myDim;
      }
    }
    else{
      for(TInt anGaussId = 0; anGaussId < myNbGauss; anGaussId++){
	aCoordSliceArr[anGaussId] =
	  TCCoordSlice(myGaussCoord,std::slice(theElemId,myDim,myGaussStep));
      }
    }
    return aCoordSliceArr;
  }


  TCoordSliceArr 
  TGaussCoord
  ::GetCoordSliceArr(TInt theElemId)
  {
    TCoordSliceArr aCoordSliceArr(myNbGauss);
    if(GetModeSwitch() == eFULL_INTERLACE){
      TInt anId = theElemId*myGaussStep;
      for(TInt anGaussId = 0; anGaussId < myNbGauss; anGaussId++){
	aCoordSliceArr[anGaussId] =
	  TCoordSlice(myGaussCoord,std::slice(anId,myDim,1));
	anId += myDim;
      }
    }
    else{
      for(TInt anGaussId = 0; anGaussId < myNbGauss; anGaussId++){
	aCoordSliceArr[anGaussId] =
	  TCoordSlice(myGaussCoord,std::slice(theElemId,myDim,myGaussStep));
      }
    }
    return aCoordSliceArr;
  }


  //---------------------------------------------------------------
  inline
  bool 
  IsEqual(TFloat theLeft, TFloat theRight)
  {
    static TFloat EPS = 1.0E-3;
    if(fabs(theLeft) + fabs(theRight) > EPS)
      return fabs(theLeft-theRight)/(fabs(theLeft)+fabs(theRight)) < EPS;
    return true;
  }


  struct TShapeFun
  {
    class TFun
    {
      TFloatVector myFun;
      TInt myNbRef;

    public:

      void
      Init(TInt theNbGauss,
	   TInt theNbRef)
      {
	myFun.resize(theNbGauss*theNbRef);
	myNbRef = theNbRef;
      }

      TCFloatVecSlice 
      GetFunSlice(TInt theGaussId) const
      {
	return TCFloatVecSlice(myFun,std::slice(theGaussId*myNbRef,myNbRef,1));
      }

      TFloatVecSlice
      GetFunSlice(TInt theGaussId)
      {
	return TFloatVecSlice(myFun,std::slice(theGaussId*myNbRef,myNbRef,1));
      }
    };

    typedef TVector<TCCoordSlice> TCCoordSliceArr;
    typedef TVector<TCoordSlice> TCoordSliceArr;
    
    TFloatVector myRefCoord;
    TInt myDim;

    TShapeFun(TInt theDim = 0, TInt theNbRef = 0):
      myDim(theDim),
      myRefCoord(theNbRef*theDim)
    {}

    TCCoordSlice 
    GetCoord(TInt theRefId) const
    {
      return TCCoordSlice(myRefCoord,std::slice(theRefId*myDim,myDim,1));
    }
    
    TCoordSlice
    GetCoord(TInt theRefId)
    {
      return TCoordSlice(myRefCoord,std::slice(theRefId*myDim,myDim,1));
    }
    
    void 
    GetFun(const TShapeFun::TCCoordSliceArr& theRef,
	   const TShapeFun::TCCoordSliceArr& theGauss,
	   TFun& theFun) const
    {
      TInt aNbRef = theRef.size();
      TInt aNbGauss = theGauss.size();
      theFun.Init(aNbGauss,aNbRef);
    }

    virtual 
    void
    InitFun(const TShapeFun::TCCoordSliceArr& theRef,
	    const TShapeFun::TCCoordSliceArr& theGauss,
	    TFun& theFun) const = 0;
    
    virtual
    bool 
    IsSatisfy(const TShapeFun::TCCoordSliceArr& theRefCoord) const
    {
      TInt aNbRef = theRefCoord.size();
      TInt aNbRef2 = myRefCoord.size()/myDim;
      INITMSG(MYDEBUG,"TShapeFun::IsSatisfy "<<
	      "- aNbRef("<<aNbRef<<")"<<
	      "; aNbRef2("<<aNbRef2<<")\n");
      bool anIsSatisfy = (aNbRef == aNbRef2);
#ifndef _DEBUG_REF_COORDS_
      static TInt NB_REF_TO_CHECK = 2;
      aNbRef = NB_REF_TO_CHECK;
#endif
      if(anIsSatisfy){
	for(TInt aRefId = 0; aRefId < aNbRef; aRefId++){
	  const TCCoordSlice& aCoord2 = theRefCoord[aRefId];
	  TCCoordSlice aCoord = GetCoord(aRefId);
	  TInt aDim = aCoord.size();
	  bool anIsEqual = false;
	  for(TInt anId = 0; anId < aDim; anId++){
	    anIsEqual = IsEqual(aCoord[anId],aCoord2[anId]);
	    if(!anIsEqual){
	      anIsSatisfy = false;
	      break;
	    }
	  }
	  if(!anIsEqual){
	    const TCCoordSlice& aCoord = theRefCoord[aRefId];
	    INITMSG(MYDEBUG,aRefId + 1<<":{");
	    TInt aDim = aCoord.size();
	    for(TInt anId = 0; anId < aDim; anId++)
	      ADDMSG(MYDEBUG,"\t"<<aCoord[anId]);
	    ADDMSG(MYDEBUG,"}\n");
#ifndef _DEBUG_
	    return anIsSatisfy;
#endif
	  }
	}
      }

      return anIsSatisfy;
    }

    bool
    Eval(const TCellInfo& theCellInfo,
	 const TNodeInfo& theNodeInfo,
	 const TElemNum& theElemNum,
	 const TShapeFun::TCCoordSliceArr& theRef,
	 const TShapeFun::TCCoordSliceArr& theGauss,
	 TGaussCoord& theGaussCoord,
	 EModeSwitch theMode)
    {
      INITMSG(MYDEBUG,"TShapeFun::Eval"<<endl);

      if(IsSatisfy(theRef)){
	const PMeshInfo& aMeshInfo = theCellInfo.GetMeshInfo();
	TInt aDim = aMeshInfo->GetDim();
	TInt aNbGauss = theGauss.size();
	
	bool anIsSubMesh = !theElemNum.empty();
	TInt aNbElem;
	if(anIsSubMesh)
	  aNbElem = theElemNum.size();
	else
	  aNbElem = theCellInfo.GetNbElem();
	
	theGaussCoord.Init(aNbElem,aNbGauss,aDim,theMode);

	TFun aFun;
	InitFun(theRef,theGauss,aFun);
	TInt aConnDim = theCellInfo.GetConnDim();
	
	INITMSG(MYDEBUG,"aDim = "<<aDim<<
		"; aNbGauss = "<<aNbGauss<<
		"; aNbElem = "<<aNbElem<<
		"; aNbNodes = "<<theNodeInfo.GetNbElem()<<
		endl);

	for(TInt anElemId = 0; anElemId < aNbElem; anElemId++){
	  TInt aCellId = anIsSubMesh? theElemNum[anElemId]-1: anElemId;
	  TCConnSlice aConnSlice = theCellInfo.GetConnSlice(aCellId);
	  TCoordSliceArr aCoordSliceArr = theGaussCoord.GetCoordSliceArr(anElemId);

	  for(TInt aGaussId = 0; aGaussId < aNbGauss; aGaussId++){
	    TCoordSlice& aGaussCoordSlice = aCoordSliceArr[aGaussId];
	    TCFloatVecSlice aFunSlice = aFun.GetFunSlice(aGaussId);

	    for(TInt aConnId = 0; aConnId < aConnDim; aConnId++){
	      TInt aNodeId = aConnSlice[aConnId] - 1;      
	      TCCoordSlice aNodeCoordSlice = theNodeInfo.GetCoordSlice(aNodeId);

	      for(TInt aDimId = 0; aDimId < aDim; aDimId++){
		aGaussCoordSlice[aDimId] += aNodeCoordSlice[aDimId]*aFunSlice[aConnId];
	      }
	    }
	  }
	}

#ifdef _DEBUG_
	for(TInt anElemId = 0; anElemId < aNbElem; anElemId++){
	  TCoordSliceArr aCoordSliceArr = theGaussCoord.GetCoordSliceArr(anElemId);
	  INITMSG(MYVALUEDEBUG,"");
	  for(TInt aGaussId = 0; aGaussId < aNbGauss; aGaussId++){
	    TCoordSlice& aCoordSlice = aCoordSliceArr[aGaussId];
	    ADDMSG(MYVALUEDEBUG,"{");
	    for(TInt aDimId = 0; aDimId < aDim; aDimId++){
	      ADDMSG(MYVALUEDEBUG,aCoordSlice[aDimId]<<" ");
	    }
	    ADDMSG(MYVALUEDEBUG,"} ");
	  }
	  ADDMSG(MYVALUEDEBUG,endl);
	}
#endif
	return true;
      }

      return false;
    }
  };


  //---------------------------------------------------------------
  struct TTria3a: TShapeFun
  {
    TTria3a():
      TShapeFun(2,3)
    {
      TInt aNbRef = myRefCoord.size();
      for(TInt aRefId = 0; aRefId < aNbRef; aRefId++){
	TCoordSlice aCoord = GetCoord(aRefId);
	switch(aRefId){
	case  0: aCoord[0] = -1.0;  aCoord[1] =  1.0; break;
	case  1: aCoord[0] = -1.0;  aCoord[1] = -1.0; break;
	case  2: aCoord[0] =  1.0;  aCoord[1] = -1.0; break;
	}
      }
    }

    virtual 
    void
    InitFun(const TShapeFun::TCCoordSliceArr& theRef,
	    const TShapeFun::TCCoordSliceArr& theGauss,
	    TFun& theFun) const
    {
      GetFun(theRef,theGauss,theFun);

      TInt aNbGauss = theGauss.size();
      for(TInt aGaussId = 0; aGaussId < aNbGauss; aGaussId++){
	const TCCoordSlice& aCoord = theGauss[aGaussId];
	TFloatVecSlice aSlice = theFun.GetFunSlice(aGaussId);

	aSlice[0] = 0.5*(1.0 + aCoord[1]);
	aSlice[1] = -0.5*(aCoord[0] + aCoord[1]);
	aSlice[2] = 0.5*(1.0 + aCoord[0]);
      }
    }
  };


  //---------------------------------------------------------------
  struct TTria6a: TShapeFun
  {
    TTria6a():
      TShapeFun(2,6)
    {
      TInt aNbRef = myRefCoord.size();
      for(TInt aRefId = 0; aRefId < aNbRef; aRefId++){
	TCoordSlice aCoord = GetCoord(aRefId);
	switch(aRefId){
	case  0: aCoord[0] = -1.0;  aCoord[1] =  1.0; break;
	case  1: aCoord[0] = -1.0;  aCoord[1] = -1.0; break;
	case  2: aCoord[0] =  1.0;  aCoord[1] = -1.0; break;

	case  3: aCoord[0] = -1.0;  aCoord[1] =  1.0; break;
	case  4: aCoord[0] =  0.0;  aCoord[1] = -1.0; break;
	case  5: aCoord[0] =  0.0;  aCoord[1] =  0.0; break;
	}
      }
    }

    virtual 
    void
    InitFun(const TShapeFun::TCCoordSliceArr& theRef,
	    const TShapeFun::TCCoordSliceArr& theGauss,
	    TFun& theFun) const
    {
      GetFun(theRef,theGauss,theFun);

      TInt aNbGauss = theGauss.size();
      for(TInt aGaussId = 0; aGaussId < aNbGauss; aGaussId++){
	const TCCoordSlice& aCoord = theGauss[aGaussId];
	TFloatVecSlice aSlice = theFun.GetFunSlice(aGaussId);

	aSlice[0] = 0.5*(1.0 + aCoord[1])*aCoord[1];
	aSlice[1] = 0.5*(aCoord[0] + aCoord[1])*(aCoord[0] + aCoord[1] + 1);
	aSlice[2] = 0.5*(1.0 + aCoord[0])*aCoord[0];
	
	aSlice[3] = -1.0*(1.0 + aCoord[1])*(aCoord[0] + aCoord[1]);
	aSlice[4] = -1.0*(1.0 + aCoord[0])*(aCoord[0] + aCoord[1]);
	aSlice[5] = (1.0 + aCoord[1])*(1.0 + aCoord[1]);
      }
    }
  };


  //---------------------------------------------------------------
  struct TTria3b: TShapeFun
  {
    TTria3b():
      TShapeFun(2,3)
    {
      TInt aNbRef = myRefCoord.size();
      for(TInt aRefId = 0; aRefId < aNbRef; aRefId++){
	TCoordSlice aCoord = GetCoord(aRefId);
	switch(aRefId){
	case  0: aCoord[0] =  0.0;  aCoord[1] =  0.0; break;
	case  1: aCoord[0] =  1.0;  aCoord[1] =  0.0; break;
	case  2: aCoord[0] =  0.0;  aCoord[1] =  1.0; break;
	}
      }
    }

    virtual 
    void
    InitFun(const TShapeFun::TCCoordSliceArr& theRef,
	    const TShapeFun::TCCoordSliceArr& theGauss,
	    TFun& theFun) const
    {
      GetFun(theRef,theGauss,theFun);

      TInt aNbGauss = theGauss.size();
      for(TInt aGaussId = 0; aGaussId < aNbGauss; aGaussId++){
	const TCCoordSlice& aCoord = theGauss[aGaussId];
	TFloatVecSlice aSlice = theFun.GetFunSlice(aGaussId);

	aSlice[0] = 1.0 - aCoord[0] - aCoord[1];
	aSlice[1] = aCoord[0];
	aSlice[2] = aCoord[1];
      }
    }
  };


  //---------------------------------------------------------------
  struct TTria6b: TShapeFun
  {
    TTria6b():
      TShapeFun(2,6)
    {
      TInt aNbRef = myRefCoord.size();
      for(TInt aRefId = 0; aRefId < aNbRef; aRefId++){
	TCoordSlice aCoord = GetCoord(aRefId);
	switch(aRefId){
	case  0: aCoord[0] =  0.0;  aCoord[1] =  0.0; break;
	case  1: aCoord[0] =  1.0;  aCoord[1] =  0.0; break;
	case  2: aCoord[0] =  0.0;  aCoord[1] =  1.0; break;

	case  3: aCoord[0] =  0.5;  aCoord[1] =  0.0; break;
	case  4: aCoord[0] =  0.5;  aCoord[1] =  0.5; break;
	case  5: aCoord[0] =  0.0;  aCoord[1] =  0.5; break;
	}
      }
    }

    virtual 
    void
    InitFun(const TShapeFun::TCCoordSliceArr& theRef,
	    const TShapeFun::TCCoordSliceArr& theGauss,
	    TFun& theFun) const
    {
      GetFun(theRef,theGauss,theFun);

      TInt aNbGauss = theGauss.size();
      for(TInt aGaussId = 0; aGaussId < aNbGauss; aGaussId++){
	const TCCoordSlice& aCoord = theGauss[aGaussId];
	TFloatVecSlice aSlice = theFun.GetFunSlice(aGaussId);

	aSlice[0] = (1.0 - aCoord[0] - aCoord[1])*(1.0 - 2.0*aCoord[0] - 2.0*aCoord[1]);
	aSlice[1] = aCoord[0]*(2.0*aCoord[0] - 1.0);
	aSlice[2] = aCoord[1]*(2.0*aCoord[1] - 1.0);
	
	aSlice[3] = 4.0*aCoord[0]*(1.0 - aCoord[0] - aCoord[1]);
	aSlice[4] = 4.0*aCoord[0]*aCoord[1];
	aSlice[5] = 4.0*aCoord[1]*(1.0 - aCoord[0] - aCoord[1]);
      }
    }
  };


  //---------------------------------------------------------------
  struct TQuad4a: TShapeFun
  {
    TQuad4a():
      TShapeFun(2,4)
    {
      TInt aNbRef = myRefCoord.size();
      for(TInt aRefId = 0; aRefId < aNbRef; aRefId++){
	TCoordSlice aCoord = GetCoord(aRefId);
	switch(aRefId){
	case  0: aCoord[0] = -1.0;  aCoord[1] =  1.0; break;
	case  1: aCoord[0] = -1.0;  aCoord[1] = -1.0; break;
	case  2: aCoord[0] =  1.0;  aCoord[1] = -1.0; break;
	case  3: aCoord[0] =  1.0;  aCoord[1] =  1.0; break;
	}
      }
    }

    virtual 
    void
    InitFun(const TShapeFun::TCCoordSliceArr& theRef,
	    const TShapeFun::TCCoordSliceArr& theGauss,
	    TFun& theFun) const
    {
      GetFun(theRef,theGauss,theFun);

      TInt aNbGauss = theGauss.size();
      for(TInt aGaussId = 0; aGaussId < aNbGauss; aGaussId++){
	const TCCoordSlice& aCoord = theGauss[aGaussId];
	TFloatVecSlice aSlice = theFun.GetFunSlice(aGaussId);

	aSlice[0] = 0.25*(1.0 + aCoord[1])*(1.0 - aCoord[0]);
	aSlice[1] = 0.25*(1.0 - aCoord[1])*(1.0 - aCoord[0]);
	aSlice[2] = 0.25*(1.0 - aCoord[1])*(1.0 + aCoord[0]);
	aSlice[3] = 0.25*(1.0 + aCoord[0])*(1.0 + aCoord[1]);
      }
    }
  };


  //---------------------------------------------------------------
  struct THexa8a: TShapeFun
  {
    THexa8a():
      TShapeFun(3,8)
    {
      TInt aNbRef = myRefCoord.size();
      for(TInt aRefId = 0; aRefId < aNbRef; aRefId++){
	TCoordSlice aCoord = GetCoord(aRefId);
	switch(aRefId){
	case  0: aCoord[0] = -1.0;  aCoord[1] = -1.0;  aCoord[2] = -1.0; break;
	case  1: aCoord[0] =  1.0;  aCoord[1] = -1.0;  aCoord[2] = -1.0; break;
	case  2: aCoord[0] =  1.0;  aCoord[1] =  1.0;  aCoord[2] = -1.0; break;
	case  3: aCoord[0] = -1.0;  aCoord[1] =  1.0;  aCoord[2] = -1.0; break;
	case  4: aCoord[0] = -1.0;  aCoord[1] = -1.0;  aCoord[2] =  1.0; break;
	case  5: aCoord[0] =  1.0;  aCoord[1] = -1.0;  aCoord[2] =  1.0; break;
	case  6: aCoord[0] =  1.0;  aCoord[1] =  1.0;  aCoord[2] =  1.0; break;
	case  7: aCoord[0] = -1.0;  aCoord[1] =  1.0;  aCoord[2] =  1.0; break;
	}
      }
    }

    virtual 
    void
    InitFun(const TShapeFun::TCCoordSliceArr& theRef,
	    const TShapeFun::TCCoordSliceArr& theGauss,
	    TFun& theFun) const
    {
      GetFun(theRef,theGauss,theFun);

      TInt aNbGauss = theGauss.size();
      for(TInt aGaussId = 0; aGaussId < aNbGauss; aGaussId++){
	const TCCoordSlice& aCoord = theGauss[aGaussId];
	TFloatVecSlice aSlice = theFun.GetFunSlice(aGaussId);

	aSlice[0] = 0.125*(1.0 - aCoord[0])*(1.0 - aCoord[1])*(1.0 - aCoord[2]);
	aSlice[1] = 0.125*(1.0 + aCoord[0])*(1.0 - aCoord[1])*(1.0 - aCoord[2]);
	aSlice[2] = 0.125*(1.0 + aCoord[0])*(1.0 + aCoord[1])*(1.0 - aCoord[2]);
	aSlice[3] = 0.125*(1.0 - aCoord[0])*(1.0 + aCoord[1])*(1.0 - aCoord[2]);

	aSlice[4] = 0.125*(1.0 - aCoord[0])*(1.0 - aCoord[1])*(1.0 + aCoord[2]);
	aSlice[5] = 0.125*(1.0 + aCoord[0])*(1.0 - aCoord[1])*(1.0 + aCoord[2]);
	aSlice[6] = 0.125*(1.0 + aCoord[0])*(1.0 + aCoord[1])*(1.0 + aCoord[2]);
	aSlice[7] = 0.125*(1.0 - aCoord[0])*(1.0 + aCoord[1])*(1.0 + aCoord[2]);
      }
    }
  };


  struct THexa20a: TShapeFun
  {
    THexa20a():
      TShapeFun(3,20)
    {
      TInt aNbRef = myRefCoord.size();
      for(TInt aRefId = 0; aRefId < aNbRef; aRefId++){
	TCoordSlice aCoord = GetCoord(aRefId);
	switch(aRefId){
	case  0: aCoord[0] = -1.0;  aCoord[1] = -1.0;  aCoord[2] = -1.0; break;
	case  1: aCoord[0] =  1.0;  aCoord[1] = -1.0;  aCoord[2] = -1.0; break;
	case  2: aCoord[0] =  1.0;  aCoord[1] =  1.0;  aCoord[2] = -1.0; break;
	case  3: aCoord[0] = -1.0;  aCoord[1] =  1.0;  aCoord[2] = -1.0; break;
	case  4: aCoord[0] = -1.0;  aCoord[1] = -1.0;  aCoord[2] =  1.0; break;
	case  5: aCoord[0] =  1.0;  aCoord[1] = -1.0;  aCoord[2] =  1.0; break;
	case  6: aCoord[0] =  1.0;  aCoord[1] =  1.0;  aCoord[2] =  1.0; break;
	case  7: aCoord[0] = -1.0;  aCoord[1] =  1.0;  aCoord[2] =  1.0; break;

	case  8: aCoord[0] =  0.0;  aCoord[1] = -1.0;  aCoord[2] = -1.0; break;
	case  9: aCoord[0] =  1.0;  aCoord[1] =  0.0;  aCoord[2] = -1.0; break;
	case 10: aCoord[0] =  0.0;  aCoord[1] =  1.0;  aCoord[2] = -1.0; break;
	case 11: aCoord[0] = -1.0;  aCoord[1] =  0.0;  aCoord[2] = -1.0; break;
	case 12: aCoord[0] = -1.0;  aCoord[1] = -1.0;  aCoord[2] =  0.0; break;
	case 13: aCoord[0] =  1.0;  aCoord[1] = -1.0;  aCoord[2] =  0.0; break;
	case 14: aCoord[0] =  1.0;  aCoord[1] =  1.0;  aCoord[2] =  0.0; break;
	case 15: aCoord[0] = -1.0;  aCoord[1] =  1.0;  aCoord[2] =  0.0; break;
	case 16: aCoord[0] =  0.0;  aCoord[1] = -1.0;  aCoord[2] =  1.0; break;
	case 17: aCoord[0] =  1.0;  aCoord[1] =  0.0;  aCoord[2] =  1.0; break;
	case 18: aCoord[0] =  0.0;  aCoord[1] =  1.0;  aCoord[2] =  1.0; break;
	case 19: aCoord[0] = -1.0;  aCoord[1] =  0.0;  aCoord[2] =  1.0; break;
	}
      }
    }

    virtual 
    void
    InitFun(const TShapeFun::TCCoordSliceArr& theRef,
	    const TShapeFun::TCCoordSliceArr& theGauss,
	    TFun& theFun) const
    {
      GetFun(theRef,theGauss,theFun);

      TInt aNbGauss = theGauss.size();
      for(TInt aGaussId = 0; aGaussId < aNbGauss; aGaussId++){
	const TCCoordSlice& aCoord = theGauss[aGaussId];
	TFloatVecSlice aSlice = theFun.GetFunSlice(aGaussId);

	aSlice[0] = 0.125*(1.0 - aCoord[0])*(1.0 - aCoord[1])*(1.0 - aCoord[2])*
	  (-2.0 - aCoord[0] - aCoord[1] - aCoord[2]);
	aSlice[1] = 0.125*(1.0 + aCoord[0])*(1.0 - aCoord[1])*(1.0 - aCoord[2])*
	  (-2.0 + aCoord[0] - aCoord[1] - aCoord[2]);
	aSlice[2] = 0.125*(1.0 + aCoord[0])*(1.0 + aCoord[1])*(1.0 - aCoord[2])*
	  (-2.0 + aCoord[0] + aCoord[1] - aCoord[2]);
	aSlice[3] = 0.125*(1.0 - aCoord[0])*(1.0 + aCoord[1])*(1.0 - aCoord[2])*
	  (-2.0 - aCoord[0] + aCoord[1] - aCoord[2]);
	aSlice[4] = 0.125*(1.0 - aCoord[0])*(1.0 - aCoord[1])*(1.0 + aCoord[2])*
	  (-2.0 - aCoord[0] - aCoord[1] + aCoord[2]);
	aSlice[5] = 0.125*(1.0 + aCoord[0])*(1.0 - aCoord[1])*(1.0 + aCoord[2])*
	  (-2.0 + aCoord[0] - aCoord[1] + aCoord[2]);
	aSlice[6] = 0.125*(1.0 + aCoord[0])*(1.0 + aCoord[1])*(1.0 + aCoord[2])*
	  (-2.0 + aCoord[0] + aCoord[1] + aCoord[2]);
	aSlice[7] = 0.125*(1.0 - aCoord[0])*(1.0 + aCoord[1])*(1.0 + aCoord[2])*
	  (-2.0 - aCoord[0] + aCoord[1] + aCoord[2]);

	aSlice[8] = 0.25*(1.0 - aCoord[0]*aCoord[0])*(1.0 - aCoord[1])*(1.0 - aCoord[2]);
	aSlice[9] = 0.25*(1.0 - aCoord[1]*aCoord[1])*(1.0 + aCoord[0])*(1.0 - aCoord[2]);
	aSlice[10] = 0.25*(1.0 - aCoord[0]*aCoord[0])*(1.0 + aCoord[1])*(1.0 - aCoord[2]);
	aSlice[11] = 0.25*(1.0 - aCoord[1]*aCoord[1])*(1.0 - aCoord[0])*(1.0 - aCoord[2]);
	aSlice[12] = 0.25*(1.0 - aCoord[2]*aCoord[2])*(1.0 - aCoord[0])*(1.0 - aCoord[1]);
	aSlice[13] = 0.25*(1.0 - aCoord[2]*aCoord[2])*(1.0 + aCoord[0])*(1.0 - aCoord[1]);
	aSlice[14] = 0.25*(1.0 - aCoord[2]*aCoord[2])*(1.0 + aCoord[0])*(1.0 + aCoord[1]);
	aSlice[15] = 0.25*(1.0 - aCoord[2]*aCoord[2])*(1.0 - aCoord[0])*(1.0 + aCoord[1]);
	aSlice[16] = 0.25*(1.0 - aCoord[0]*aCoord[0])*(1.0 - aCoord[1])*(1.0 + aCoord[2]);
	aSlice[17] = 0.25*(1.0 - aCoord[1]*aCoord[1])*(1.0 + aCoord[0])*(1.0 + aCoord[2]);
	aSlice[18] = 0.25*(1.0 - aCoord[0]*aCoord[0])*(1.0 + aCoord[1])*(1.0 + aCoord[2]);
	aSlice[19] = 0.25*(1.0 - aCoord[1]*aCoord[1])*(1.0 - aCoord[0])*(1.0 + aCoord[2]);
      }
    }
  };


  //---------------------------------------------------------------
  struct TPenta6a: TShapeFun
  {
    TPenta6a():
      TShapeFun(3,6)
    {
      TInt aNbRef = myRefCoord.size();
      for(TInt aRefId = 0; aRefId < aNbRef; aRefId++){
	TCoordSlice aCoord = GetCoord(aRefId);
	switch(aRefId){
	case  0: aCoord[0] = -1.0;  aCoord[1] =  1.0;  aCoord[2] =  0.0; break;
	case  1: aCoord[0] = -1.0;  aCoord[1] = -0.0;  aCoord[2] =  1.0; break;
	case  2: aCoord[0] = -1.0;  aCoord[1] =  0.0;  aCoord[2] =  0.0; break;
	case  3: aCoord[0] =  1.0;  aCoord[1] =  1.0;  aCoord[2] =  0.0; break;
	case  4: aCoord[0] =  1.0;  aCoord[1] =  0.0;  aCoord[2] =  1.0; break;
	case  5: aCoord[0] =  1.0;  aCoord[1] =  0.0;  aCoord[2] =  0.0; break;
	}
      }
    }

    virtual 
    void
    InitFun(const TShapeFun::TCCoordSliceArr& theRef,
	    const TShapeFun::TCCoordSliceArr& theGauss,
	    TFun& theFun) const
    {
      GetFun(theRef,theGauss,theFun);

      TInt aNbGauss = theGauss.size();
      for(TInt aGaussId = 0; aGaussId < aNbGauss; aGaussId++){
	const TCCoordSlice& aCoord = theGauss[aGaussId];
	TFloatVecSlice aSlice = theFun.GetFunSlice(aGaussId);

	aSlice[0] = 0.5*aCoord[1]*(1.0 - aCoord[0]);
	aSlice[1] = 0.5*aCoord[2]*(1.0 - aCoord[0]);
	aSlice[2] = 0.5*(1.0 - aCoord[1] - aCoord[2])*(1.0 - aCoord[0]);

	aSlice[3] = 0.5*aCoord[1]*(aCoord[0] + 1.0);
	aSlice[4] = 0.5*aCoord[2]*(aCoord[0] + 1.0);
	aSlice[5] = 0.5*(1.0 - aCoord[1] - aCoord[2])*(1.0 + aCoord[0]);
      }
    }
  };


  //---------------------------------------------------------------
  struct TPenta15a: TShapeFun
  {
    TPenta15a():
      TShapeFun(3,15)
    {
      TInt aNbRef = myRefCoord.size();
      for(TInt aRefId = 0; aRefId < aNbRef; aRefId++){
	TCoordSlice aCoord = GetCoord(aRefId);
	switch(aRefId){
	case  0: aCoord[0] = -1.0;  aCoord[1] =  1.0;  aCoord[2] =  0.0; break;
	case  1: aCoord[0] = -1.0;  aCoord[1] = -0.0;  aCoord[2] =  1.0; break;
	case  2: aCoord[0] = -1.0;  aCoord[1] =  0.0;  aCoord[2] =  0.0; break;
	case  3: aCoord[0] =  1.0;  aCoord[1] =  1.0;  aCoord[2] =  0.0; break;
	case  4: aCoord[0] =  1.0;  aCoord[1] =  0.0;  aCoord[2] =  1.0; break;
	case  5: aCoord[0] =  1.0;  aCoord[1] =  0.0;  aCoord[2] =  0.0; break;

	case  6: aCoord[0] = -1.0;  aCoord[1] =  0.5;  aCoord[2] =  0.5; break;
	case  7: aCoord[0] = -1.0;  aCoord[1] =  0.0;  aCoord[2] =  0.5; break;
	case  8: aCoord[0] = -1.0;  aCoord[1] =  0.5;  aCoord[2] =  0.0; break;
	case  9: aCoord[0] =  0.0;  aCoord[1] =  1.0;  aCoord[2] =  0.0; break;
	case 10: aCoord[0] =  0.0;  aCoord[1] =  0.0;  aCoord[2] =  1.0; break;
	case 11: aCoord[0] =  0.0;  aCoord[1] =  0.0;  aCoord[2] =  0.0; break;
	case 12: aCoord[0] =  1.0;  aCoord[1] =  0.5;  aCoord[2] =  0.5; break;
	case 13: aCoord[0] =  1.0;  aCoord[1] =  0.0;  aCoord[2] =  0.5; break;
	case 14: aCoord[0] =  1.0;  aCoord[1] =  0.5;  aCoord[2] =  0.0; break;
	}
      }
    }

    virtual 
    void
    InitFun(const TShapeFun::TCCoordSliceArr& theRef,
	    const TShapeFun::TCCoordSliceArr& theGauss,
	    TFun& theFun) const
    {
      GetFun(theRef,theGauss,theFun);

      TInt aNbGauss = theGauss.size();
      for(TInt aGaussId = 0; aGaussId < aNbGauss; aGaussId++){
	const TCCoordSlice& aCoord = theGauss[aGaussId];
	TFloatVecSlice aSlice = theFun.GetFunSlice(aGaussId);

	aSlice[0] = 0.5*aCoord[1]*(1.0 - aCoord[0])*(2.0*aCoord[1] - 2.0 - aCoord[0]);
	aSlice[1] = 0.5*aCoord[2]*(1.0 - aCoord[0])*(2.0*aCoord[2] - 2.0 - aCoord[0]);
	aSlice[2] = 0.5*(aCoord[0] - 1.0)*(1.0 - aCoord[1] - aCoord[2])*(aCoord[0] + 2.0*aCoord[1] + 2.0*aCoord[2]);

	aSlice[3] = 0.5*aCoord[1]*(1.0 + aCoord[0])*(2.0*aCoord[1] - 2.0 - aCoord[0]);
	aSlice[4] = 0.5*aCoord[2]*(1.0 + aCoord[0])*(2.0*aCoord[2] - 2.0 - aCoord[0]);
	aSlice[5] = -0.5*(aCoord[0] + 1.0)*(1.0 - aCoord[1] - aCoord[2])*(-aCoord[0] + 2.0*aCoord[1] + 2.0*aCoord[2]);

	aSlice[6] = 2.0*aCoord[1]*aCoord[2]*(1.0 - aCoord[0]);
	aSlice[7] = 2.0*aCoord[2]*(1.0 - aCoord[1] - aCoord[2])*(1.0 - aCoord[0]);
	aSlice[8] = 2.0*aCoord[1]*(1.0 - aCoord[1] - aCoord[2])*(1.0 - aCoord[0]);

	aSlice[9] = aCoord[1]*(1.0 - aCoord[0]*aCoord[0]);
	aSlice[10] = aCoord[2]*(1.0 - aCoord[0]*aCoord[0]);
	aSlice[11] = (1.0 - aCoord[1] - aCoord[2])*(1.0 - aCoord[0]*aCoord[0]);

	aSlice[12] = 2.0*aCoord[1]*aCoord[2]*(1.0 + aCoord[0]);
	aSlice[13] = 2.0*aCoord[2]*(1.0 - aCoord[1] - aCoord[2])*(1.0 + aCoord[0]);
	aSlice[14] = 2.0*aCoord[1]*(1.0 - aCoord[1] - aCoord[2])*(1.0 + aCoord[0]);
      }
    }
  };


  //---------------------------------------------------------------
  bool
  GetGaussCoord3D(const TGaussInfo& theGaussInfo, 
		  const TCellInfo& theCellInfo,
		  const TNodeInfo& theNodeInfo,
		  TGaussCoord& theGaussCoord,
		  const TElemNum& theElemNum,
		  EModeSwitch theMode)
  {
    INITMSG(MYDEBUG,"GetGaussCoord3D\n");

    if(theGaussInfo.myGeom == theCellInfo.myGeom){
      EGeometrieElement aGeom = theGaussInfo.myGeom;

      TInt aNbRef = theGaussInfo.GetNbRef();
      TCCoordSliceArr aRefSlice(aNbRef);
      for(TInt anId = 0; anId < aNbRef; anId++)
	aRefSlice[anId] = theGaussInfo.GetRefCoordSlice(anId);

      TInt aNbGauss = theGaussInfo.GetNbGauss();
      TCCoordSliceArr aGaussSlice(aNbGauss);
      for(TInt anId = 0; anId < aNbGauss; anId++)
	aGaussSlice[anId] = theGaussInfo.GetGaussCoordSlice(anId);

      switch(aGeom){
      case eSEG2: {
	INITMSG(MYDEBUG,"eSEG2"<<endl);
	break;
      }
      case eSEG3: {
	INITMSG(MYDEBUG,"eSEG3"<<endl);
	break;
      }
      case eTRIA3: {
	INITMSG(MYDEBUG,"eTRIA3"<<endl);

	if(TTria3a().Eval(theCellInfo,theNodeInfo,theElemNum,aRefSlice,aGaussSlice,theGaussCoord,theMode))
	  return true;

	if(TTria3b().Eval(theCellInfo,theNodeInfo,theElemNum,aRefSlice,aGaussSlice,theGaussCoord,theMode))
	  return true;

	break;
      }
      case eTRIA6: {
	INITMSG(MYDEBUG,"eTRIA6"<<endl);

	if(TTria6a().Eval(theCellInfo,theNodeInfo,theElemNum,aRefSlice,aGaussSlice,theGaussCoord,theMode))
	  return true;

	if(TTria6b().Eval(theCellInfo,theNodeInfo,theElemNum,aRefSlice,aGaussSlice,theGaussCoord,theMode))
	  return true;
	
	break;
      }
      case eQUAD4: {
	INITMSG(MYDEBUG,"eQUAD4"<<endl);

	if(TQuad4a().Eval(theCellInfo,theNodeInfo,theElemNum,aRefSlice,aGaussSlice,theGaussCoord,theMode))
	  return true;

	break;
      }
      case eQUAD8: {
	INITMSG(MYDEBUG,"eQUAD8"<<endl);
	break;
      }
      case eTETRA4: {
	INITMSG(MYDEBUG,"eTETRA4"<<endl);
	break;
      }
      case ePYRA5: {
	INITMSG(MYDEBUG,"ePYRA5"<<endl);
	break;
      }
      case ePENTA6: {
	INITMSG(MYDEBUG,"ePENTA6"<<endl);

	if(TPenta6a().Eval(theCellInfo,theNodeInfo,theElemNum,aRefSlice,aGaussSlice,theGaussCoord,theMode))
	  return true;

	break;
      }
      case eHEXA8: {
	INITMSG(MYDEBUG,"eHEXA8"<<endl);

	if(THexa8a().Eval(theCellInfo,theNodeInfo,theElemNum,aRefSlice,aGaussSlice,theGaussCoord,theMode))
	  return true;

	break;
      }
      case eTETRA10: {
	INITMSG(MYDEBUG,"eTETRA10"<<endl);
	break;
      }
      case ePYRA13: {
	INITMSG(MYDEBUG,"ePYRA13"<<endl);
	break;
      }
      case ePENTA15: {
	INITMSG(MYDEBUG,"ePENTA15"<<endl);
	break;
      }
      case eHEXA20: {
	INITMSG(MYDEBUG,"eHEXA20"<<endl);

	if(THexa20a().Eval(theCellInfo,theNodeInfo,theElemNum,aRefSlice,aGaussSlice,theGaussCoord,theMode))
	  return true;

	break;
      }
      default: 
	INITMSG(MYDEBUG,"eNONE"<<endl);
	return false;
      }
    }

    return false;
  }

  //---------------------------------------------------------------
  bool
  GetBaryCenter(const TCellInfo& theCellInfo,
		const TNodeInfo& theNodeInfo,
		TGaussCoord& theGaussCoord,
		const TElemNum& theElemNum,
		EModeSwitch theMode)
  {
    INITMSG(MYDEBUG,"GetBaryCenter\n");
    const PMeshInfo& aMeshInfo = theCellInfo.GetMeshInfo();
    TInt aDim = aMeshInfo->GetDim();
    static TInt aNbGauss = 1;
	
    bool anIsSubMesh = !theElemNum.empty();
    TInt aNbElem;
    if(anIsSubMesh)
      aNbElem = theElemNum.size();
    else
      aNbElem = theCellInfo.GetNbElem();
	
    theGaussCoord.Init(aNbElem,aNbGauss,aDim,theMode);

    TInt aConnDim = theCellInfo.GetConnDim();
    
    INITMSGA(MYDEBUG,0,
	     "- aDim = "<<aDim<<
	     "; aNbGauss = "<<aNbGauss<<
	     "; aNbElem = "<<aNbElem<<
	     "; aNbNodes = "<<theNodeInfo.GetNbElem()<<
	     endl);
    
    for(TInt anElemId = 0; anElemId < aNbElem; anElemId++){
      TInt aCellId = anIsSubMesh? theElemNum[anElemId]-1: anElemId;
      TCConnSlice aConnSlice = theCellInfo.GetConnSlice(aCellId);
      TCoordSliceArr aCoordSliceArr = theGaussCoord.GetCoordSliceArr(anElemId);
      
      for(TInt aGaussId = 0; aGaussId < aNbGauss; aGaussId++){
	TCoordSlice& aGaussCoordSlice = aCoordSliceArr[aGaussId];
	
	for(TInt aConnId = 0; aConnId < aConnDim; aConnId++){
	  TInt aNodeId = aConnSlice[aConnId] - 1;      
	  TCCoordSlice aNodeCoordSlice = theNodeInfo.GetCoordSlice(aNodeId);
	  
	  for(TInt aDimId = 0; aDimId < aDim; aDimId++){
	    aGaussCoordSlice[aDimId] += aNodeCoordSlice[aDimId];
	  }
	}
	
	for(TInt aDimId = 0; aDimId < aDim; aDimId++){
	  aGaussCoordSlice[aDimId] /= aConnDim;
	}
      }
    }
    
#ifdef _DEBUG_
    for(TInt anElemId = 0; anElemId < aNbElem; anElemId++){
      TCoordSliceArr aCoordSliceArr = theGaussCoord.GetCoordSliceArr(anElemId);
      INITMSG(MYVALUEDEBUG,"");
      for(TInt aGaussId = 0; aGaussId < aNbGauss; aGaussId++){
	TCoordSlice& aCoordSlice = aCoordSliceArr[aGaussId];
	ADDMSG(MYVALUEDEBUG,"{");
	for(TInt aDimId = 0; aDimId < aDim; aDimId++){
	  ADDMSG(MYVALUEDEBUG,aCoordSlice[aDimId]<<" ");
	}
	ADDMSG(MYVALUEDEBUG,"} ");
      }
      ADDMSG(MYVALUEDEBUG,endl);
    }
#endif

    return true;
  }

}
