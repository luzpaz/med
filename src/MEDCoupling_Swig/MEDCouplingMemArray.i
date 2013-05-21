// Copyright (C) 2007-2013  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// Author : Anthony Geay (CEA/DEN)

namespace ParaMEDMEM
{
  class DataArray : public RefCountObject, public TimeLabel
  {
  public:
    void setName(const char *name);
    void copyStringInfoFrom(const DataArray& other) throw(INTERP_KERNEL::Exception);
    void copyPartOfStringInfoFrom(const DataArray& other, const std::vector<int>& compoIds) throw(INTERP_KERNEL::Exception);
    void copyPartOfStringInfoFrom2(const std::vector<int>& compoIds, const DataArray& other) throw(INTERP_KERNEL::Exception);
    bool areInfoEqualsIfNotWhy(const DataArray& other, std::string& reason) const throw(INTERP_KERNEL::Exception);
    bool areInfoEquals(const DataArray& other) const throw(INTERP_KERNEL::Exception);
    std::string cppRepr(const char *varName) const throw(INTERP_KERNEL::Exception);
    std::string getName() const;
    void setInfoOnComponents(const std::vector<std::string>& info) throw(INTERP_KERNEL::Exception);
    void setInfoAndChangeNbOfCompo(const std::vector<std::string>& info) throw(INTERP_KERNEL::Exception);
    std::vector<std::string> getVarsOnComponent() const throw(INTERP_KERNEL::Exception);
    std::vector<std::string> getUnitsOnComponent() const throw(INTERP_KERNEL::Exception);
    std::string getInfoOnComponent(int i) const throw(INTERP_KERNEL::Exception);
    std::string getVarOnComponent(int i) const throw(INTERP_KERNEL::Exception);
    std::string getUnitOnComponent(int i) const throw(INTERP_KERNEL::Exception);
    void setInfoOnComponent(int i, const char *info) throw(INTERP_KERNEL::Exception);
    int getNumberOfComponents() const;
    virtual void alloc(int nbOfTuple, int nbOfCompo=1) throw(INTERP_KERNEL::Exception);
    virtual bool isAllocated() const throw(INTERP_KERNEL::Exception);
    virtual int getNumberOfTuples() const throw(INTERP_KERNEL::Exception);
    virtual std::size_t getNbOfElems() const throw(INTERP_KERNEL::Exception);
    virtual std::size_t getNbOfElemAllocated() const throw(INTERP_KERNEL::Exception);
    void checkNbOfTuples(int nbOfTuples, const char *msg) const throw(INTERP_KERNEL::Exception);
    void checkNbOfComps(int nbOfCompo, const char *msg) const throw(INTERP_KERNEL::Exception);
    void checkNbOfTuplesAndComp(const DataArray& other, const char *msg) const throw(INTERP_KERNEL::Exception);
    void checkNbOfTuplesAndComp(int nbOfTuples, int nbOfCompo, const char *msg) const throw(INTERP_KERNEL::Exception);
    void checkNbOfElems(std::size_t nbOfElems, const char *msg) const throw(INTERP_KERNEL::Exception);
    static int GetNumberOfItemGivenBES(int begin, int end, int step, const char *msg) throw(INTERP_KERNEL::Exception);
    static int GetNumberOfItemGivenBESRelative(int begin, int end, int step, const char *msg) throw(INTERP_KERNEL::Exception);
    static int GetPosOfItemGivenBESRelativeNoThrow(int value, int begin, int end, int step) throw(INTERP_KERNEL::Exception);
    static std::string GetVarNameFromInfo(const std::string& info) throw(INTERP_KERNEL::Exception);
    static std::string GetUnitFromInfo(const std::string& info) throw(INTERP_KERNEL::Exception);
    void updateTime() const;
    %extend
    {
      PyObject *getInfoOnComponents() const throw(INTERP_KERNEL::Exception)
      {
        const std::vector<std::string>& comps=self->getInfoOnComponents();
        PyObject *ret=PyList_New((int)comps.size());
        for(int i=0;i<(int)comps.size();i++)
          PyList_SetItem(ret,i,PyString_FromString(comps[i].c_str()));
        return ret;
      }
      
      void copyPartOfStringInfoFrom(const DataArray& other, PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        std::vector<int> tmp;
        convertPyToNewIntArr3(li,tmp);
        self->copyPartOfStringInfoFrom(other,tmp);
      }

      void copyPartOfStringInfoFrom2(PyObject *li, const DataArray& other) throw(INTERP_KERNEL::Exception)
      {
        std::vector<int> tmp;
        convertPyToNewIntArr3(li,tmp);
        self->copyPartOfStringInfoFrom2(tmp,other);
      }

      virtual void renumberInPlace(PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            self->renumberInPlace(tmp);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            if(!da2)
              throw INTERP_KERNEL::Exception("Not null DataArrayInt instance expected !");
            da2->checkAllocated();
            int size=self->getNumberOfTuples();
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            self->renumberInPlace(da2->getConstPointer());
          }
      }

      virtual void renumberInPlaceR(PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            self->renumberInPlaceR(tmp);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            if(!da2)
              throw INTERP_KERNEL::Exception("Not null DataArrayInt instance expected !");
            da2->checkAllocated();
            int size=self->getNumberOfTuples();
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            self->renumberInPlaceR(da2->getConstPointer());
          }
      }

      //tuplesSelec in PyObject * because DataArrayInt is not already existing !
      virtual void setContigPartOfSelectedValues(int tupleIdStart, PyObject *aBase, PyObject *tuplesSelec) throw(INTERP_KERNEL::Exception)
      {
        static const char msg[]="DataArray::setContigPartOfSelectedValues2 : 4th parameter \"tuplesSelec\" should be of type DataArrayInt";
          DataArray *a=CheckAndRetrieveDataArrayInstance(aBase,"DataArray::setContigPartOfSelectedValues2 : 3rd parameter \"aBase\" should be of type DataArray");
        DataArray *tuplesSelecPtr=CheckAndRetrieveDataArrayInstance(tuplesSelec,msg);
        DataArrayInt *tuplesSelecPtr2=0;
        if(tuplesSelecPtr)
          {
            tuplesSelecPtr2=dynamic_cast<DataArrayInt *>(tuplesSelecPtr);
            if(!tuplesSelecPtr2)
              throw INTERP_KERNEL::Exception(msg);
          }
        self->setContigPartOfSelectedValues(tupleIdStart,a,tuplesSelecPtr2);
      }
      
      virtual void setContigPartOfSelectedValues2(int tupleIdStart, PyObject *aBase, int bg, int end2, int step) throw(INTERP_KERNEL::Exception)
      {
        DataArray *a=CheckAndRetrieveDataArrayInstance(aBase,"DataArray::setContigPartOfSelectedValues2 : 2nd parameter \"aBase\" should be of type DataArray");
        self->setContigPartOfSelectedValues2(tupleIdStart,a,bg,end2,step);
      }
    }
  };
  
  class DataArrayInt;
  class DataArrayDoubleIterator;
  
  class DataArrayDouble : public DataArray
  {
  public:
    static DataArrayDouble *New();
    void checkAllocated() const throw(INTERP_KERNEL::Exception);
    double doubleValue() const throw(INTERP_KERNEL::Exception);
    bool empty() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *deepCpy() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *performCpy(bool deepCpy) const throw(INTERP_KERNEL::Exception);
    void cpyFrom(const DataArrayDouble& other) throw(INTERP_KERNEL::Exception);
    void reserve(std::size_t nbOfElems) throw(INTERP_KERNEL::Exception);
    void pushBackSilent(double val) throw(INTERP_KERNEL::Exception);
    void pushBackValsSilent(const double *valsBg, const double *valsEnd) throw(INTERP_KERNEL::Exception);
    double popBackSilent() throw(INTERP_KERNEL::Exception);
    void pack() const throw(INTERP_KERNEL::Exception);
    void allocIfNecessary(int nbOfTuple, int nbOfCompo) throw(INTERP_KERNEL::Exception);
    void fillWithZero() throw(INTERP_KERNEL::Exception);
    void fillWithValue(double val) throw(INTERP_KERNEL::Exception);
    void iota(double init=0.) throw(INTERP_KERNEL::Exception);
    bool isUniform(double val, double eps) const throw(INTERP_KERNEL::Exception);
    void sort(bool asc=true) throw(INTERP_KERNEL::Exception);
    void reverse() throw(INTERP_KERNEL::Exception);
    void checkMonotonic(bool increasing, double eps) const throw(INTERP_KERNEL::Exception);
    bool isMonotonic(bool increasing, double eps) const throw(INTERP_KERNEL::Exception);
    std::string repr() const throw(INTERP_KERNEL::Exception);
    std::string reprZip() const throw(INTERP_KERNEL::Exception);
    bool isEqual(const DataArrayDouble& other, double prec) const throw(INTERP_KERNEL::Exception);
    bool isEqualWithoutConsideringStr(const DataArrayDouble& other, double prec) const throw(INTERP_KERNEL::Exception);
    void reAlloc(int nbOfTuples) throw(INTERP_KERNEL::Exception);
    DataArrayInt *convertToIntArr() const;
    DataArrayDouble *fromNoInterlace() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *toNoInterlace() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *selectByTupleId2(int bg, int end2, int step) const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *substr(int tupleIdBg, int tupleIdEnd=-1) const throw(INTERP_KERNEL::Exception);
    void rearrange(int newNbOfCompo) throw(INTERP_KERNEL::Exception);
    void transpose() throw(INTERP_KERNEL::Exception);
    DataArrayDouble *changeNbOfComponents(int newNbOfComp, double dftValue) const throw(INTERP_KERNEL::Exception);
    void meldWith(const DataArrayDouble *other) throw(INTERP_KERNEL::Exception);
    DataArrayDouble *duplicateEachTupleNTimes(int nbTimes) const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *getDifferentValues(double prec, int limitTupleId=-1) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *findClosestTupleId(const DataArrayDouble *other) const throw(INTERP_KERNEL::Exception);
    void setPartOfValues1(const DataArrayDouble *a, int bgTuples, int endTuples, int stepTuples, int bgComp, int endComp, int stepComp, bool strictCompoCompare=true) throw(INTERP_KERNEL::Exception);
    void setPartOfValuesSimple1(double a, int bgTuples, int endTuples, int stepTuples, int bgComp, int endComp, int stepComp) throw(INTERP_KERNEL::Exception);
    void setPartOfValuesAdv(const DataArrayDouble *a, const DataArrayInt *tuplesSelec) throw(INTERP_KERNEL::Exception);
    double getIJ(int tupleId, int compoId) const;
    double back() const throw(INTERP_KERNEL::Exception);
    double getIJSafe(int tupleId, int compoId) const throw(INTERP_KERNEL::Exception);
    void setIJ(int tupleId, int compoId, double newVal) throw(INTERP_KERNEL::Exception);
    void setIJSilent(int tupleId, int compoId, double newVal) throw(INTERP_KERNEL::Exception);
    double *getPointer() throw(INTERP_KERNEL::Exception);
    void checkNoNullValues() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *computeBBoxPerTuple(double epsilon=0.0) const throw(INTERP_KERNEL::Exception);
    void recenterForMaxPrecision(double eps) throw(INTERP_KERNEL::Exception);
    double getMaxValue(int& tupleId) const throw(INTERP_KERNEL::Exception);
    double getMaxValueInArray() const throw(INTERP_KERNEL::Exception);
    double getMinValue(int& tupleId) const throw(INTERP_KERNEL::Exception);
    double getMinValueInArray() const throw(INTERP_KERNEL::Exception);
    double getAverageValue() const throw(INTERP_KERNEL::Exception);
    double norm2() const throw(INTERP_KERNEL::Exception);
    double normMax() const throw(INTERP_KERNEL::Exception);
    void accumulate(double *res) const throw(INTERP_KERNEL::Exception);
    double accumulate(int compId) const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *fromPolarToCart() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *fromCylToCart() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *fromSpherToCart() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *doublyContractedProduct() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *determinant() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *eigenValues() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *eigenVectors() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *inverse() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *trace() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *deviator() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *magnitude() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *maxPerTuple() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *buildEuclidianDistanceDenseMatrix() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *buildEuclidianDistanceDenseMatrixWith(const DataArrayDouble *other) const throw(INTERP_KERNEL::Exception);
    void sortPerTuple(bool asc) throw(INTERP_KERNEL::Exception);
    void abs() throw(INTERP_KERNEL::Exception);
    void applyLin(double a, double b, int compoId) throw(INTERP_KERNEL::Exception);
    void applyLin(double a, double b) throw(INTERP_KERNEL::Exception);
    void applyInv(double numerator) throw(INTERP_KERNEL::Exception);
    void applyPow(double val) throw(INTERP_KERNEL::Exception);
    void applyRPow(double val) throw(INTERP_KERNEL::Exception);
    DataArrayDouble *negate() const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *applyFunc(int nbOfComp, FunctionToEvaluate func) const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *applyFunc(int nbOfComp, const char *func) const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *applyFunc(const char *func) const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *applyFunc2(int nbOfComp, const char *func) const throw(INTERP_KERNEL::Exception);
    DataArrayDouble *applyFunc3(int nbOfComp, const std::vector<std::string>& varsOrder, const char *func) const throw(INTERP_KERNEL::Exception);
    void applyFuncFast32(const char *func) throw(INTERP_KERNEL::Exception);
    void applyFuncFast64(const char *func) throw(INTERP_KERNEL::Exception);
    DataArrayInt *getIdsInRange(double vmin, double vmax) const throw(INTERP_KERNEL::Exception);
    static DataArrayDouble *Aggregate(const DataArrayDouble *a1, const DataArrayDouble *a2) throw(INTERP_KERNEL::Exception);
    static DataArrayDouble *Aggregate(const std::vector<const DataArrayDouble *>& arr) throw(INTERP_KERNEL::Exception);
    static DataArrayDouble *Meld(const DataArrayDouble *a1, const DataArrayDouble *a2) throw(INTERP_KERNEL::Exception);
    static DataArrayDouble *Meld(const std::vector<const DataArrayDouble *>& arr) throw(INTERP_KERNEL::Exception);
    static DataArrayDouble *Dot(const DataArrayDouble *a1, const DataArrayDouble *a2) throw(INTERP_KERNEL::Exception);
    static DataArrayDouble *CrossProduct(const DataArrayDouble *a1, const DataArrayDouble *a2) throw(INTERP_KERNEL::Exception);
    static DataArrayDouble *Max(const DataArrayDouble *a1, const DataArrayDouble *a2) throw(INTERP_KERNEL::Exception);
    static DataArrayDouble *Min(const DataArrayDouble *a1, const DataArrayDouble *a2) throw(INTERP_KERNEL::Exception);
    static DataArrayDouble *Add(const DataArrayDouble *a1, const DataArrayDouble *a2) throw(INTERP_KERNEL::Exception);
    void addEqual(const DataArrayDouble *other) throw(INTERP_KERNEL::Exception);
    static DataArrayDouble *Substract(const DataArrayDouble *a1, const DataArrayDouble *a2) throw(INTERP_KERNEL::Exception);
    void substractEqual(const DataArrayDouble *other) throw(INTERP_KERNEL::Exception);
    static DataArrayDouble *Multiply(const DataArrayDouble *a1, const DataArrayDouble *a2) throw(INTERP_KERNEL::Exception);
    void multiplyEqual(const DataArrayDouble *other) throw(INTERP_KERNEL::Exception);
    static DataArrayDouble *Divide(const DataArrayDouble *a1, const DataArrayDouble *a2) throw(INTERP_KERNEL::Exception);
    void divideEqual(const DataArrayDouble *other) throw(INTERP_KERNEL::Exception);
    static DataArrayDouble *Pow(const DataArrayDouble *a1, const DataArrayDouble *a2) throw(INTERP_KERNEL::Exception);
    void powEqual(const DataArrayDouble *other) throw(INTERP_KERNEL::Exception);
    %extend
    {
      DataArrayDouble() throw(INTERP_KERNEL::Exception)
        {
          return DataArrayDouble::New();
        }

      static DataArrayDouble *New(PyObject *elt0, PyObject *nbOfTuples=0, PyObject *elt2=0) throw(INTERP_KERNEL::Exception)
      {
        const char *msgBase="ParaMEDMEM::DataArrayDouble::New : Available API are : \n-DataArrayDouble.New()\n-DataArrayDouble.New([1.,3.,4.])\n-DataArrayDouble.New([1.,3.,4.],3)\n-DataArrayDouble.New([1.,3.,4.,5.],2,2)\n-DataArrayDouble.New([1.,3.,4.,5.,7,8.],3,2)\n-DataArrayDouble.New([(1.,3.),(4.,5.),(7,8.)])\n-DataArrayDouble.New(5)\n-DataArrayDouble.New(5,2)";
        std::string msg(msgBase);
#ifdef WITH_NUMPY
        msg+="\n-DataArrayDouble.New(numpy array with dtype=float64)";
#endif
        msg+=" !";
        if(PyList_Check(elt0) || PyTuple_Check(elt0))
          {
            if(nbOfTuples)
              {
                if(PyInt_Check(nbOfTuples))
                  {
                    int nbOfTuples1=PyInt_AS_LONG(nbOfTuples);
                    if(nbOfTuples1<0)
                      throw INTERP_KERNEL::Exception("DataArrayDouble::New : should be a positive set of allocated memory !");
                    if(elt2)
                      {
                        if(PyInt_Check(elt2))
                          {//DataArrayDouble.New([1.,3.,4.,5.],2,2)
                            int nbOfCompo=PyInt_AS_LONG(elt2);
                            if(nbOfCompo<0)
                              throw INTERP_KERNEL::Exception("DataArrayDouble::New : should be a positive number of components !");
                            MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=DataArrayDouble::New();
                            std::vector<double> tmp=fillArrayWithPyListDbl2(elt0,nbOfTuples1,nbOfCompo);
                            ret->alloc(nbOfTuples1,nbOfCompo); std::copy(tmp.begin(),tmp.end(),ret->getPointer());
                            return ret.retn();
                          }
                        else
                          throw INTERP_KERNEL::Exception(msg.c_str());
                      }
                    else
                      {//DataArrayDouble.New([1.,3.,4.],3)
                        MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=DataArrayDouble::New();
                        int tmpp1=-1;
                        std::vector<double> tmp=fillArrayWithPyListDbl2(elt0,nbOfTuples1,tmpp1);
                        ret->alloc(nbOfTuples1,tmpp1); std::copy(tmp.begin(),tmp.end(),ret->getPointer());
                        return ret.retn();
                      }
                  }
                else
                  throw INTERP_KERNEL::Exception(msg.c_str());
              }
            else
              {// DataArrayDouble.New([1.,3.,4.])
                MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=DataArrayDouble::New();
                int tmpp1=-1,tmpp2=-1;
                std::vector<double> tmp=fillArrayWithPyListDbl2(elt0,tmpp1,tmpp2);
                ret->alloc(tmpp1,tmpp2); std::copy(tmp.begin(),tmp.end(),ret->getPointer());
                return ret.retn();
              }
          }
        else if(PyInt_Check(elt0))
          {
            int nbOfTuples1=PyInt_AS_LONG(elt0);
            if(nbOfTuples1<0)
              throw INTERP_KERNEL::Exception("DataArrayDouble::New : should be a positive set of allocated memory !");
            if(nbOfTuples)
              {
                if(!elt2)
                  {
                    if(PyInt_Check(nbOfTuples))
                      {//DataArrayDouble.New(5,2)
                        int nbOfCompo=PyInt_AS_LONG(nbOfTuples);
                        if(nbOfCompo<0)
                          throw INTERP_KERNEL::Exception("DataArrayDouble::New : should be a positive number of components !");
                        MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=DataArrayDouble::New();
                        ret->alloc(nbOfTuples1,nbOfCompo);
                        return ret.retn();
                      }
                    else
                      throw INTERP_KERNEL::Exception(msg.c_str());
                  }
                else
                  throw INTERP_KERNEL::Exception(msg.c_str());
              }
            else
              {//DataArrayDouble.New(5)
                MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=DataArrayDouble::New();
                ret->alloc(nbOfTuples1,1);
                return ret.retn();
              }
          }
#ifdef WITH_NUMPY
        else if(PyArray_Check(elt0) && nbOfTuples==NULL && elt2==NULL)
          {//DataArrayDouble.New(numpyArray)
            return BuildNewInstance<DataArrayDouble,double>(elt0,NPY_DOUBLE,&PyCallBackDataArrayDouble_RefType,"FLOAT64");
          }
#endif
        else
          throw INTERP_KERNEL::Exception(msg.c_str());
      }
   
      DataArrayDouble(PyObject *elt0, PyObject *nbOfTuples=0, PyObject *elt2=0) throw(INTERP_KERNEL::Exception)
        {
          return ParaMEDMEM_DataArrayDouble_New__SWIG_1(elt0,nbOfTuples,elt2);
        }

      void pushBackValsSilent(PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        double val;
        std::vector<double> bb;
        int sw,nbTuples=-1;
        const char msg[]="Python wrap of DataArrayDouble::pushBackValsSilent : ";
        const double *tmp=convertObjToPossibleCpp5_SingleCompo(li,sw,val,bb,msg,true,nbTuples);
        self->pushBackValsSilent(tmp,tmp+nbTuples);
      }

      std::string __repr__() const throw(INTERP_KERNEL::Exception)
      {
        std::ostringstream oss;
        self->reprQuickOverview(oss);
        return oss.str();
      }

      std::string __str__() const throw(INTERP_KERNEL::Exception)
      {
        return self->repr();
      }

      double __float__() const throw(INTERP_KERNEL::Exception)
      {
        return self->doubleValue();
      }

      int __len__() const throw(INTERP_KERNEL::Exception)
      {
        if(self->isAllocated())
          {
            return self->getNumberOfTuples();
          }
        else
          {
            throw INTERP_KERNEL::Exception("DataArrayDouble::__len__ : Instance is NOT allocated !");
          }
      }

      DataArrayDoubleIterator *__iter__()
      {
        return self->iterator();
      }
   
      void setValues(PyObject *li, PyObject *nbOfTuples=0, PyObject *nbOfComp=0) throw(INTERP_KERNEL::Exception)
      {
        const char *msg="ParaMEDMEM::DataArrayDouble::setValues : Available API are : \n-DataArrayDouble.setValues([1.,3.,4.])\n-DataArrayDouble.setValues([1.,3.,4.],3)\n-DataArrayDouble.setValues([1.,3.,4.,5.],2,2)\n-DataArrayDouble.setValues([(1.,1.7),(3.,3.7),(4.,4.7)])\n !";
        if(PyList_Check(li) || PyTuple_Check(li))
          {
            if(nbOfTuples)
              {
                if(PyInt_Check(nbOfTuples))
                  {
                    int nbOfTuples1=PyInt_AS_LONG(nbOfTuples);
                    if(nbOfTuples1<0)
                      throw INTERP_KERNEL::Exception("DataArrayDouble::setValues : should be a positive set of allocated memory !");
                    if(nbOfComp)
                      {
                        if(PyInt_Check(nbOfComp))
                          {//DataArrayDouble.setValues([1.,3.,4.,5.],2,2)
                            int nbOfCompo=PyInt_AS_LONG(nbOfComp);
                            if(nbOfCompo<0)
                              throw INTERP_KERNEL::Exception("DataArrayDouble::setValues : should be a positive number of components !");
                            std::vector<double> tmp=fillArrayWithPyListDbl2(li,nbOfTuples1,nbOfCompo);
                            self->alloc(nbOfTuples1,nbOfCompo); std::copy(tmp.begin(),tmp.end(),self->getPointer());
                          }
                        else
                          throw INTERP_KERNEL::Exception(msg);
                      }
                    else
                      {//DataArrayDouble.setValues([1.,3.,4.],3)
                        int tmpp1=-1;
                        std::vector<double> tmp=fillArrayWithPyListDbl2(li,nbOfTuples1,tmpp1);
                        self->alloc(nbOfTuples1,tmpp1); std::copy(tmp.begin(),tmp.end(),self->getPointer());
                      }
                  }
                else
                  throw INTERP_KERNEL::Exception(msg);
              }
            else
              {// DataArrayDouble.setValues([1.,3.,4.])
                int tmpp1=-1,tmpp2=-1;
                std::vector<double> tmp=fillArrayWithPyListDbl2(li,tmpp1,tmpp2);
                self->alloc(tmpp1,tmpp2); std::copy(tmp.begin(),tmp.end(),self->getPointer());
              }
          }
        else
          throw INTERP_KERNEL::Exception(msg);
      }

      PyObject *getValues() const throw(INTERP_KERNEL::Exception)
      {
        const double *vals=self->getConstPointer();
        return convertDblArrToPyList(vals,self->getNbOfElems());
      }

#ifdef WITH_NUMPY
      PyObject *toNumPyArray() throw(INTERP_KERNEL::Exception) // not const. It is not a bug !
      {
        return ToNumPyArray<DataArrayDouble,double>(self,NPY_DOUBLE,"DataArrayDouble");
      }
#endif

      PyObject *isEqualIfNotWhy(const DataArrayDouble& other, double prec) const throw(INTERP_KERNEL::Exception)
      {
        std::string ret1;
        bool ret0=self->isEqualIfNotWhy(other,prec,ret1);
        PyObject *ret=PyTuple_New(2);
        PyObject *ret0Py=ret0?Py_True:Py_False;
        Py_XINCREF(ret0Py);
        PyTuple_SetItem(ret,0,ret0Py);
        PyTuple_SetItem(ret,1,PyString_FromString(ret1.c_str()));
        return ret;
      }

      PyObject *getValuesAsTuple() const throw(INTERP_KERNEL::Exception)
      {
        const double *vals=self->getConstPointer();
        int nbOfComp=self->getNumberOfComponents();
        int nbOfTuples=self->getNumberOfTuples();
        return convertDblArrToPyListOfTuple(vals,nbOfComp,nbOfTuples);
      }

      DataArrayDouble *renumber(PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumber(tmp);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            if(!da2)
              throw INTERP_KERNEL::Exception("Not null DataArrayInt instance expected !");
            da2->checkAllocated();
            int size=self->getNumberOfTuples();
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumber(da2->getConstPointer());
          }
      }

      DataArrayDouble *renumberR(PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumberR(tmp);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            if(!da2)
              throw INTERP_KERNEL::Exception("Not null DataArrayInt instance expected !");
            da2->checkAllocated();
            int size=self->getNumberOfTuples();
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumberR(da2->getConstPointer());
          }
      }

      DataArrayDouble *renumberAndReduce(PyObject *li, int newNbOfTuple) throw(INTERP_KERNEL::Exception)
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumberAndReduce(tmp,newNbOfTuple);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            if(!da2)
              throw INTERP_KERNEL::Exception("Not null DataArrayInt instance expected !");
            da2->checkAllocated();
            int size=self->getNumberOfTuples();
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumberAndReduce(da2->getConstPointer(),newNbOfTuple);
          }
      }

      DataArrayDouble *selectByTupleId(PyObject *li) const throw(INTERP_KERNEL::Exception)
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            return self->selectByTupleId(tmp,tmp+size);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            if(!da2)
              throw INTERP_KERNEL::Exception("Not null DataArrayInt instance expected !");
            da2->checkAllocated();
            return self->selectByTupleId(da2->getConstPointer(),da2->getConstPointer()+da2->getNbOfElems());
          }
      }

      DataArrayDouble *selectByTupleIdSafe(PyObject *li) const throw(INTERP_KERNEL::Exception)
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            return self->selectByTupleIdSafe(tmp,tmp+size);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            if(!da2)
              throw INTERP_KERNEL::Exception("Not null DataArrayInt instance expected !");
            da2->checkAllocated();
            return self->selectByTupleIdSafe(da2->getConstPointer(),da2->getConstPointer()+da2->getNbOfElems());
          }
      }

      PyObject *minimalDistanceTo(const DataArrayDouble *other) const throw(INTERP_KERNEL::Exception)
      {
        int thisTupleId,otherTupleId;
        double r0=self->minimalDistanceTo(other,thisTupleId,otherTupleId);
        PyObject *ret=PyTuple_New(3);
        PyTuple_SetItem(ret,0,PyFloat_FromDouble(r0));
        PyTuple_SetItem(ret,1,PyInt_FromLong(thisTupleId));
        PyTuple_SetItem(ret,2,PyInt_FromLong(otherTupleId));
        return ret;
      }

      PyObject *getMaxValue() const throw(INTERP_KERNEL::Exception)
      {
        int tmp;
        double r1=self->getMaxValue(tmp);
        PyObject *ret=PyTuple_New(2);
        PyTuple_SetItem(ret,0,PyFloat_FromDouble(r1));
        PyTuple_SetItem(ret,1,PyInt_FromLong(tmp));
        return ret;
      }

      PyObject *getMaxValue2() const throw(INTERP_KERNEL::Exception)
      {
        DataArrayInt *tmp;
        double r1=self->getMaxValue2(tmp);
        PyObject *ret=PyTuple_New(2);
        PyTuple_SetItem(ret,0,PyFloat_FromDouble(r1));
        PyTuple_SetItem(ret,1,SWIG_NewPointerObj(SWIG_as_voidptr(tmp),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 ));
        return ret;
      }

      PyObject *getMinValue() const throw(INTERP_KERNEL::Exception)
      {
        int tmp;
        double r1=self->getMinValue(tmp);
        PyObject *ret=PyTuple_New(2);
        PyTuple_SetItem(ret,0,PyFloat_FromDouble(r1));
        PyTuple_SetItem(ret,1,PyInt_FromLong(tmp));
        return ret;
      }

      PyObject *getMinValue2() const throw(INTERP_KERNEL::Exception)
      {
        DataArrayInt *tmp;
        double r1=self->getMinValue2(tmp);
        PyObject *ret=PyTuple_New(2);
        PyTuple_SetItem(ret,0,PyFloat_FromDouble(r1));
        PyTuple_SetItem(ret,1,SWIG_NewPointerObj(SWIG_as_voidptr(tmp),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 ));
        return ret;
      }

      PyObject *getMinMaxPerComponent() const throw(INTERP_KERNEL::Exception)
      {
        int nbOfCompo=self->getNumberOfComponents();
        INTERP_KERNEL::AutoPtr<double> tmp=new double[2*nbOfCompo];
        self->getMinMaxPerComponent(tmp);
        PyObject *ret=convertDblArrToPyListOfTuple(tmp,2,nbOfCompo);
        return ret;
      }

      PyObject *accumulate() const throw(INTERP_KERNEL::Exception)
      {
        int sz=self->getNumberOfComponents();
        INTERP_KERNEL::AutoPtr<double> tmp=new double[sz];
        self->accumulate(tmp);
        return convertDblArrToPyList(tmp,sz);
      }
   
      DataArrayDouble *keepSelectedComponents(PyObject *li) const throw(INTERP_KERNEL::Exception)
      {
        std::vector<int> tmp;
        convertPyToNewIntArr3(li,tmp);
        return self->keepSelectedComponents(tmp);
      }

      PyObject *findCommonTuples(double prec, int limitNodeId=-1) const throw(INTERP_KERNEL::Exception)
      {
        DataArrayInt *comm, *commIndex;
        self->findCommonTuples(prec,limitNodeId,comm,commIndex);
        PyObject *res = PyList_New(2);
        PyList_SetItem(res,0,SWIG_NewPointerObj(SWIG_as_voidptr(comm),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 ));
        PyList_SetItem(res,1,SWIG_NewPointerObj(SWIG_as_voidptr(commIndex),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 ));
        return res;
      }

      PyObject *distanceToTuple(PyObject *tuple) const throw(INTERP_KERNEL::Exception)
      {
        double val;
        DataArrayDouble *a;
        DataArrayDoubleTuple *aa;
        std::vector<double> bb;
        int sw;
        int tupleId=-1,nbOfCompo=self->getNumberOfComponents();
        const double *pt=convertObjToPossibleCpp5_Safe(tuple,sw,val,a,aa,bb,"Python wrap of DataArrayDouble::distanceToTuple",1,nbOfCompo,true);
        //
        double ret0=self->distanceToTuple(pt,pt+nbOfCompo,tupleId);
        PyObject *ret=PyTuple_New(2);
        PyTuple_SetItem(ret,0,PyFloat_FromDouble(ret0));
        PyTuple_SetItem(ret,1,PyInt_FromLong(tupleId));
        return ret;
      }

      void setSelectedComponents(const DataArrayDouble *a, PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        std::vector<int> tmp;
        convertPyToNewIntArr3(li,tmp);
        self->setSelectedComponents(a,tmp);
      }
   
      PyObject *getTuple(int tupleId) throw(INTERP_KERNEL::Exception)
      {
        int sz=self->getNumberOfComponents();
        INTERP_KERNEL::AutoPtr<double> tmp=new double[sz];
        self->getTuple(tupleId,tmp);
        return convertDblArrToPyList(tmp,sz);
      }

      static DataArrayDouble *Aggregate(PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        std::vector<const DataArrayDouble *> tmp;
        convertFromPyObjVectorOfObj<const DataArrayDouble *>(li,SWIGTYPE_p_ParaMEDMEM__DataArrayDouble,"DataArrayDouble",tmp);
        return DataArrayDouble::Aggregate(tmp);
      }

      static DataArrayDouble *Meld(PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        std::vector<const DataArrayDouble *> tmp;
        convertFromPyObjVectorOfObj<const DataArrayDouble *>(li,SWIGTYPE_p_ParaMEDMEM__DataArrayDouble,"DataArrayDouble",tmp);
        return DataArrayDouble::Meld(tmp);
      }

      DataArrayDouble *selectByTupleRanges(PyObject *li) const throw(INTERP_KERNEL::Exception)
      {
        std::vector<std::pair<int,int> > ranges;
        convertPyToVectorPairInt(li,ranges);
        return self->selectByTupleRanges(ranges);
      }

      PyObject *computeTupleIdsNearTuples(PyObject *pt, double eps) const throw(INTERP_KERNEL::Exception)
      {
        double val;
        DataArrayDouble *a;
        DataArrayDoubleTuple *aa;
        std::vector<double> bb;
        int sw;
        int nbComp=self->getNumberOfComponents(),nbTuples=-1;
        const char msg[]="Python wrap of DataArrayDouble::computeTupleIdsNearTuples : ";
        const double *pos=convertObjToPossibleCpp5_Safe2(pt,sw,val,a,aa,bb,msg,nbComp,true,nbTuples);
        MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> inpu=DataArrayDouble::New(); inpu->useArray(pos,false,CPP_DEALLOC,nbTuples,nbComp);
        DataArrayInt *c=0,*cI=0;
        self->computeTupleIdsNearTuples(inpu,eps,c,cI);
        PyObject *ret=PyTuple_New(2);
        PyTuple_SetItem(ret,0,SWIG_NewPointerObj(SWIG_as_voidptr(c),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 ));
        PyTuple_SetItem(ret,1,SWIG_NewPointerObj(SWIG_as_voidptr(cI),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 ));
        return ret;
      }

      PyObject *__getitem__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in DataArrayDouble::__getitem__ !";
        const char msg2[]="DataArrayDouble::__getitem__ : Mismatch of slice values in 2nd parameter (components) !";
        self->checkAllocated();
        int nbOfTuples=self->getNumberOfTuples();
        int nbOfComponents=self->getNumberOfComponents();
        int it1,ic1;
        std::vector<int> vt1,vc1;
        std::pair<int, std::pair<int,int> > pt1,pc1;
        DataArrayInt *dt1=0,*dc1=0;
        int sw;
        convertObjToPossibleCpp3(obj,nbOfTuples,nbOfComponents,sw,it1,ic1,vt1,vc1,pt1,pc1,dt1,dc1);
        MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret;
        switch(sw)
          {
          case 1:
            if(nbOfComponents==1)
              return PyFloat_FromDouble(self->getIJSafe(it1,0));
            return SWIG_NewPointerObj(SWIG_as_voidptr(self->selectByTupleIdSafe(&it1,&it1+1)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
          case 2:
            return SWIG_NewPointerObj(SWIG_as_voidptr(self->selectByTupleIdSafe(&vt1[0],&vt1[0]+vt1.size())),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
          case 3:
            return SWIG_NewPointerObj(SWIG_as_voidptr(self->selectByTupleId2(pt1.first,pt1.second.first,pt1.second.second)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
          case 4:
            return SWIG_NewPointerObj(SWIG_as_voidptr(self->selectByTupleIdSafe(dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems())),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
          case 5:
            return PyFloat_FromDouble(self->getIJSafe(it1,ic1));
          case 6:
            {
              ret=self->selectByTupleIdSafe(&vt1[0],&vt1[0]+vt1.size());
              std::vector<int> v2(1,ic1);
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(v2)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 7:
            {
              ret=self->selectByTupleId2(pt1.first,pt1.second.first,pt1.second.second);
              std::vector<int> v2(1,ic1);
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(v2)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 8:
            {
              ret=self->selectByTupleIdSafe(dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems());
              std::vector<int> v2(1,ic1);
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(v2)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 9:
            {
              ret=self->selectByTupleIdSafe(&it1,&it1+1);
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(vc1)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 10:
            {
              ret=self->selectByTupleIdSafe(&vt1[0],&vt1[0]+vt1.size());
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(vc1)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 11:
            {
              ret=self->selectByTupleId2(pt1.first,pt1.second.first,pt1.second.second);
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(vc1)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 12:
            {
              ret=self->selectByTupleIdSafe(dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems());
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(vc1)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 13:
            {
              ret=self->selectByTupleIdSafe(&it1,&it1+1);
              int nbOfComp=DataArray::GetNumberOfItemGivenBESRelative(pc1.first,pc1.second.first,pc1.second.second,msg2);
              std::vector<int> v2(nbOfComp);
              for(int i=0;i<nbOfComp;i++)
                v2[i]=pc1.first+i*pc1.second.second;
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(v2)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 14:
            {
              ret=self->selectByTupleIdSafe(&vt1[0],&vt1[0]+vt1.size());
              int nbOfComp=DataArray::GetNumberOfItemGivenBESRelative(pc1.first,pc1.second.first,pc1.second.second,msg2);
              std::vector<int> v2(nbOfComp);
              for(int i=0;i<nbOfComp;i++)
                v2[i]=pc1.first+i*pc1.second.second;
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(v2)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 15:
            {
              ret=self->selectByTupleId2(pt1.first,pt1.second.first,pt1.second.second);
              int nbOfComp=DataArray::GetNumberOfItemGivenBESRelative(pc1.first,pc1.second.first,pc1.second.second,msg2);
              std::vector<int> v2(nbOfComp);
              for(int i=0;i<nbOfComp;i++)
                v2[i]=pc1.first+i*pc1.second.second;
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(v2)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 16:
            {
              ret=self->selectByTupleIdSafe(dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems());
              int nbOfComp=DataArray::GetNumberOfItemGivenBESRelative(pc1.first,pc1.second.first,pc1.second.second,msg2);
              std::vector<int> v2(nbOfComp);
              for(int i=0;i<nbOfComp;i++)
                v2[i]=pc1.first+i*pc1.second.second;
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(v2)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayDouble *__setitem__(PyObject *obj, PyObject *value) throw(INTERP_KERNEL::Exception)
      {
        self->checkAllocated();
        const char msg[]="Unexpected situation in DataArrayDouble::__setitem__ !";
        int nbOfTuples=self->getNumberOfTuples();
        int nbOfComponents=self->getNumberOfComponents();
        int sw1,sw2;
        double i1;
        std::vector<double> v1;
        DataArrayDouble *d1=0;
        convertObjToPossibleCpp4(value,sw1,i1,v1,d1);
        int it1,ic1;
        std::vector<int> vt1,vc1;
        std::pair<int, std::pair<int,int> > pt1,pc1;
        DataArrayInt *dt1=0,*dc1=0;
        convertObjToPossibleCpp3(obj,nbOfTuples,nbOfComponents,sw2,it1,ic1,vt1,vc1,pt1,pc1,dt1,dc1);
        MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> tmp;
        switch(sw2)
          {
          case 1:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple1(i1,it1,it1+1,1,0,nbOfComponents,1);
                  return self;
                case 2:
                  tmp=DataArrayDouble::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues1(tmp,it1,it1+1,1,0,nbOfComponents,1,false);
                  return self;
                case 3:
                  self->setPartOfValues1(d1,it1,it1+1,1,0,nbOfComponents,1);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 2:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple3(i1,&vt1[0],&vt1[0]+vt1.size(),0,nbOfComponents,1);
                  return self;
                case 2:
                  tmp=DataArrayDouble::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues3(tmp,&vt1[0],&vt1[0]+vt1.size(),0,nbOfComponents,1,false);
                  return self;
                case 3:
                  self->setPartOfValues3(d1,&vt1[0],&vt1[0]+vt1.size(),0,nbOfComponents,1);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 3:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple1(i1,pt1.first,pt1.second.first,pt1.second.second,0,nbOfComponents,1);
                  return self;
                case 2:
                  tmp=DataArrayDouble::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues1(tmp,pt1.first,pt1.second.first,pt1.second.second,0,nbOfComponents,1,false);
                  return self;
                case 3:
                  self->setPartOfValues1(d1,pt1.first,pt1.second.first,pt1.second.second,0,nbOfComponents,1);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 4:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple3(i1,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),0,nbOfComponents,1);
                  return self;
                case 2:
                  tmp=DataArrayDouble::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues3(tmp,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),0,nbOfComponents,1,false);
                  return self;
                case 3:
                  self->setPartOfValues3(d1,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),0,nbOfComponents,1);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 5:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple1(i1,it1,it1+1,1,ic1,ic1+1,1);
                  return self;
                case 2:
                  tmp=DataArrayDouble::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues1(tmp,it1,it1+1,1,ic1,ic1+1,1,false);
                  return self;
                case 3:
                  self->setPartOfValues1(d1,it1,it1+1,1,ic1,ic1+1,1);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 6:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple3(i1,&vt1[0],&vt1[0]+vt1.size(),ic1,ic1+1,1);
                  return self;
                case 2:
                  tmp=DataArrayDouble::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues3(tmp,&vt1[0],&vt1[0]+vt1.size(),ic1,ic1+1,1,false);
                  return self;
                case 3:
                  self->setPartOfValues3(d1,&vt1[0],&vt1[0]+vt1.size(),ic1,ic1+1,1);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 7:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple1(i1,pt1.first,pt1.second.first,pt1.second.second,ic1,ic1+1,1);
                  return self;
                case 2:
                  tmp=DataArrayDouble::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues1(tmp,pt1.first,pt1.second.first,pt1.second.second,ic1,ic1+1,1,false);
                  return self;
                case 3:
                  self->setPartOfValues1(d1,pt1.first,pt1.second.first,pt1.second.second,ic1,ic1+1,1);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 8:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple3(i1,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),ic1,ic1+1,1);
                  return self;
                case 2:
                  tmp=DataArrayDouble::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues3(tmp,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),ic1,ic1+1,1,false);
                  return self;
                case 3:
                  self->setPartOfValues3(d1,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),ic1,ic1+1,1);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 9:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple2(i1,&it1,&it1+1,&vc1[0],&vc1[0]+vc1.size());
                  return self;
                case 2:
                  tmp=DataArrayDouble::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues2(tmp,&it1,&it1+1,&vc1[0],&vc1[0]+vc1.size(),false);
                  return self;
                case 3:
                  self->setPartOfValues2(d1,&it1,&it1+1,&vc1[0],&vc1[0]+vc1.size());
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 10:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple2(i1,&vt1[0],&vt1[0]+vt1.size(),&vc1[0],&vc1[0]+vc1.size());
                  return self;
                case 2:
                  tmp=DataArrayDouble::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues2(tmp,&vt1[0],&vt1[0]+vt1.size(),&vc1[0],&vc1[0]+vc1.size(),false);
                  return self;
                case 3:
                  self->setPartOfValues2(d1,&vt1[0],&vt1[0]+vt1.size(),&vc1[0],&vc1[0]+vc1.size());
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 11:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple4(i1,pt1.first,pt1.second.first,pt1.second.second,&vc1[0],&vc1[0]+vc1.size());
                  return self;
                case 2:
                  tmp=DataArrayDouble::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues4(tmp,pt1.first,pt1.second.first,pt1.second.second,&vc1[0],&vc1[0]+vc1.size(),false);
                  return self;
                case 3:
                  self->setPartOfValues4(d1,pt1.first,pt1.second.first,pt1.second.second,&vc1[0],&vc1[0]+vc1.size());
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 12:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple2(i1,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),&vc1[0],&vc1[0]+vc1.size());
                  return self;
                case 2:
                  tmp=DataArrayDouble::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues2(tmp,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),&vc1[0],&vc1[0]+vc1.size(),false);
                  return self;
                case 3:
                  self->setPartOfValues2(d1,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),&vc1[0],&vc1[0]+vc1.size());
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 13:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple1(i1,it1,it1+1,1,pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                case 2:
                  tmp=DataArrayDouble::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues1(tmp,it1,it1+1,1,pc1.first,pc1.second.first,pc1.second.second,false);
                  return self;
                case 3:
                  self->setPartOfValues1(d1,it1,it1+1,1,pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 14:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple3(i1,&vt1[0],&vt1[0]+vt1.size(),pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                case 2:
                  tmp=DataArrayDouble::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues3(tmp,&vt1[0],&vt1[0]+vt1.size(),pc1.first,pc1.second.first,pc1.second.second,false);
                  return self;
                case 3:
                  self->setPartOfValues3(d1,&vt1[0],&vt1[0]+vt1.size(),pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 15:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple1(i1,pt1.first,pt1.second.first,pt1.second.second,pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                case 2:
                  tmp=DataArrayDouble::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues1(tmp,pt1.first,pt1.second.first,pt1.second.second,pc1.first,pc1.second.first,pc1.second.second,false);
                  return self;
                case 3:
                  self->setPartOfValues1(d1,pt1.first,pt1.second.first,pt1.second.second,pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 16:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple3(i1,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                case 2:
                  tmp=DataArrayDouble::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues3(tmp,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),pc1.first,pc1.second.first,pc1.second.second,false);
                  return self;
                case 3:
                  self->setPartOfValues3(d1,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
        return self;
      }

      DataArrayDouble *__neg__() const throw(INTERP_KERNEL::Exception)
      {
        return self->negate();
      }

      PyObject *__add__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in DataArrayDouble.__add__ !";
        double val;
        DataArrayDouble *a;
        DataArrayDoubleTuple *aa;
        std::vector<double> bb;
        int sw;
        //
        void *argp;
        if(SWIG_IsOK(SWIG_ConvertPtr(obj,&argp,SWIGTYPE_p_ParaMEDMEM__MEDCouplingFieldDouble,0|0)))
          {
            MEDCouplingFieldDouble *other=reinterpret_cast< ParaMEDMEM::MEDCouplingFieldDouble * >(argp);
            if(other)
              {
                PyObject *tmp=SWIG_NewPointerObj(SWIG_as_voidptr(self),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, 0 | 0 );
                MEDCouplingAutoRefCountObjectPtr<MEDCouplingFieldDouble> ret=ParaMEDMEM_MEDCouplingFieldDouble___radd__Impl(other,tmp);
                Py_XDECREF(tmp);
                return SWIG_NewPointerObj(SWIG_as_voidptr(ret.retn()),SWIGTYPE_p_ParaMEDMEM__MEDCouplingFieldDouble, SWIG_POINTER_OWN | 0 );
              }
            else
              throw INTERP_KERNEL::Exception(msg);
          }
        //
        convertObjToPossibleCpp5(obj,sw,val,a,aa,bb);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=self->deepCpy();
              ret->applyLin(1.,val);
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret.retn()),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 2:
            {
              return SWIG_NewPointerObj(SWIG_as_voidptr(DataArrayDouble::Add(self,a)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 3:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=aa->buildDADouble(1,self->getNumberOfComponents());
              return SWIG_NewPointerObj(SWIG_as_voidptr(DataArrayDouble::Add(self,aaa)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=DataArrayDouble::New(); aaa->useArray(&bb[0],false,CPP_DEALLOC,1,(int)bb.size());
              return SWIG_NewPointerObj(SWIG_as_voidptr(DataArrayDouble::Add(self,aaa)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayDouble *__radd__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __radd__ !";
        double val;
        DataArrayDouble *a;
        DataArrayDoubleTuple *aa;
        std::vector<double> bb;
        int sw;
        convertObjToPossibleCpp5(obj,sw,val,a,aa,bb);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=self->deepCpy();
              ret->applyLin(1.,val);
              return ret.retn();
            }
          case 3:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=aa->buildDADouble(1,self->getNumberOfComponents());
              return DataArrayDouble::Add(self,aaa);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=DataArrayDouble::New(); aaa->useArray(&bb[0],false,CPP_DEALLOC,1,(int)bb.size());
              return DataArrayDouble::Add(self,aaa);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }
   
      PyObject *___iadd___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __iadd__ !";
        double val;
        DataArrayDouble *a;
        DataArrayDoubleTuple *aa;
        std::vector<double> bb;
        int sw;
        convertObjToPossibleCpp5(obj,sw,val,a,aa,bb);
        switch(sw)
          {
          case 1:
            {
              self->applyLin(1.,val);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 2:
            {
              self->addEqual(a);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 3:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=aa->buildDADouble(1,self->getNumberOfComponents());
              self->addEqual(aaa);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=DataArrayDouble::New(); aaa->useArray(&bb[0],false,CPP_DEALLOC,1,(int)bb.size());
              self->addEqual(aaa);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      PyObject *__sub__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __sub__ !";
        double val;
        DataArrayDouble *a;
        DataArrayDoubleTuple *aa;
        std::vector<double> bb;
        int sw;
        //
        void *argp;
        if(SWIG_IsOK(SWIG_ConvertPtr(obj,&argp,SWIGTYPE_p_ParaMEDMEM__MEDCouplingFieldDouble,0|0)))
          {
            MEDCouplingFieldDouble *other=reinterpret_cast< ParaMEDMEM::MEDCouplingFieldDouble * >(argp);
            if(other)
              {
                PyObject *tmp=SWIG_NewPointerObj(SWIG_as_voidptr(self),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, 0 | 0 );
                MEDCouplingAutoRefCountObjectPtr<MEDCouplingFieldDouble> ret=ParaMEDMEM_MEDCouplingFieldDouble___rsub__Impl(other,tmp);
                Py_XDECREF(tmp);
                return SWIG_NewPointerObj(SWIG_as_voidptr(ret.retn()),SWIGTYPE_p_ParaMEDMEM__MEDCouplingFieldDouble, SWIG_POINTER_OWN | 0 );
              }
            else
              throw INTERP_KERNEL::Exception(msg);
          }
        //
        convertObjToPossibleCpp5(obj,sw,val,a,aa,bb);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=self->deepCpy();
              ret->applyLin(1.,-val);
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret.retn()),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 2:
            {
              return SWIG_NewPointerObj(SWIG_as_voidptr(DataArrayDouble::Substract(self,a)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 3:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=aa->buildDADouble(1,self->getNumberOfComponents());
              return SWIG_NewPointerObj(SWIG_as_voidptr(DataArrayDouble::Substract(self,aaa)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=DataArrayDouble::New(); aaa->useArray(&bb[0],false,CPP_DEALLOC,1,(int)bb.size());
              return SWIG_NewPointerObj(SWIG_as_voidptr(DataArrayDouble::Substract(self,aaa)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayDouble *__rsub__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __rsub__ !";
        double val;
        DataArrayDouble *a;
        DataArrayDoubleTuple *aa;
        std::vector<double> bb;
        int sw;
        convertObjToPossibleCpp5(obj,sw,val,a,aa,bb);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=self->deepCpy();
              ret->applyLin(-1.,val);
              return ret.retn();
            }
          case 3:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=aa->buildDADouble(1,self->getNumberOfComponents());
              return DataArrayDouble::Substract(aaa,self);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=DataArrayDouble::New(); aaa->useArray(&bb[0],false,CPP_DEALLOC,1,(int)bb.size());
              return DataArrayDouble::Substract(aaa,self);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      PyObject *___isub___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __isub__ !";
        double val;
        DataArrayDouble *a;
        DataArrayDoubleTuple *aa;
        std::vector<double> bb;
        int sw;
        convertObjToPossibleCpp5(obj,sw,val,a,aa,bb);
        switch(sw)
          {
          case 1:
            {
              self->applyLin(1,-val);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 2:
            {
              self->substractEqual(a);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 3:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=aa->buildDADouble(1,self->getNumberOfComponents());
              self->substractEqual(aaa);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=DataArrayDouble::New(); aaa->useArray(&bb[0],false,CPP_DEALLOC,1,(int)bb.size());
              self->substractEqual(aaa);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      PyObject *__mul__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __mul__ !";
        double val;
        DataArrayDouble *a;
        DataArrayDoubleTuple *aa;
        std::vector<double> bb;
        int sw;
        //
        void *argp;
        if(SWIG_IsOK(SWIG_ConvertPtr(obj,&argp,SWIGTYPE_p_ParaMEDMEM__MEDCouplingFieldDouble,0|0)))
          {
            MEDCouplingFieldDouble *other=reinterpret_cast< ParaMEDMEM::MEDCouplingFieldDouble * >(argp);
            if(other)
              {
                PyObject *tmp=SWIG_NewPointerObj(SWIG_as_voidptr(self),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, 0 | 0 );
                MEDCouplingAutoRefCountObjectPtr<MEDCouplingFieldDouble> ret=ParaMEDMEM_MEDCouplingFieldDouble___rmul__Impl(other,tmp);
                Py_XDECREF(tmp);
                return SWIG_NewPointerObj(SWIG_as_voidptr(ret.retn()),SWIGTYPE_p_ParaMEDMEM__MEDCouplingFieldDouble, SWIG_POINTER_OWN | 0 );
              }
            else
              throw INTERP_KERNEL::Exception(msg);
          }
        //
        convertObjToPossibleCpp5(obj,sw,val,a,aa,bb);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=self->deepCpy();
              ret->applyLin(val,0.);
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret.retn()),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 2:
            {
              return SWIG_NewPointerObj(SWIG_as_voidptr(DataArrayDouble::Multiply(self,a)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 3:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=aa->buildDADouble(1,self->getNumberOfComponents());
              return SWIG_NewPointerObj(SWIG_as_voidptr(DataArrayDouble::Multiply(self,aaa)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=DataArrayDouble::New(); aaa->useArray(&bb[0],false,CPP_DEALLOC,1,(int)bb.size());
              return SWIG_NewPointerObj(SWIG_as_voidptr(DataArrayDouble::Multiply(self,aaa)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayDouble *__rmul__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __rmul__ !";
        double val;
        DataArrayDouble *a;
        DataArrayDoubleTuple *aa;
        std::vector<double> bb;
        int sw;
        convertObjToPossibleCpp5(obj,sw,val,a,aa,bb);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=self->deepCpy();
              ret->applyLin(val,0.);
              return ret.retn();
            }
          case 3:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=aa->buildDADouble(1,self->getNumberOfComponents());
              return DataArrayDouble::Multiply(self,aaa);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=DataArrayDouble::New(); aaa->useArray(&bb[0],false,CPP_DEALLOC,1,(int)bb.size());
              return DataArrayDouble::Multiply(self,aaa);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      PyObject *___imul___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __imul__ !";
        double val;
        DataArrayDouble *a;
        DataArrayDoubleTuple *aa;
        std::vector<double> bb;
        int sw;
        convertObjToPossibleCpp5(obj,sw,val,a,aa,bb);
        switch(sw)
          {
          case 1:
            {
              self->applyLin(val,0.);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 2:
            {
              self->multiplyEqual(a);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 3:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=aa->buildDADouble(1,self->getNumberOfComponents());
              self->multiplyEqual(aaa);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=DataArrayDouble::New(); aaa->useArray(&bb[0],false,CPP_DEALLOC,1,(int)bb.size());
              self->multiplyEqual(aaa);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      PyObject *__div__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __div__ !";
        double val;
        DataArrayDouble *a;
        DataArrayDoubleTuple *aa;
        std::vector<double> bb;
        int sw;
        //
        void *argp;
        if(SWIG_IsOK(SWIG_ConvertPtr(obj,&argp,SWIGTYPE_p_ParaMEDMEM__MEDCouplingFieldDouble,0|0)))
          {
            MEDCouplingFieldDouble *other=reinterpret_cast< ParaMEDMEM::MEDCouplingFieldDouble * >(argp);
            if(other)
              {
                PyObject *tmp=SWIG_NewPointerObj(SWIG_as_voidptr(self),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, 0 | 0 );
                MEDCouplingAutoRefCountObjectPtr<MEDCouplingFieldDouble> ret=ParaMEDMEM_MEDCouplingFieldDouble___rdiv__Impl(other,tmp);
                Py_XDECREF(tmp);
                return SWIG_NewPointerObj(SWIG_as_voidptr(ret.retn()),SWIGTYPE_p_ParaMEDMEM__MEDCouplingFieldDouble, SWIG_POINTER_OWN | 0 );
              }
            else
              throw INTERP_KERNEL::Exception(msg);
          }
        //
        convertObjToPossibleCpp5(obj,sw,val,a,aa,bb);
        switch(sw)
          {
          case 1:
            {
              if(val==0.)
                throw INTERP_KERNEL::Exception("DataArrayDouble::__div__ : trying to divide by zero !");
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=self->deepCpy();
              ret->applyLin(1/val,0.);
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret.retn()),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 2:
            {
              return SWIG_NewPointerObj(SWIG_as_voidptr(DataArrayDouble::Divide(self,a)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 3:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=aa->buildDADouble(1,self->getNumberOfComponents());
              return SWIG_NewPointerObj(SWIG_as_voidptr(DataArrayDouble::Divide(self,aaa)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=DataArrayDouble::New(); aaa->useArray(&bb[0],false,CPP_DEALLOC,1,(int)bb.size());
              return SWIG_NewPointerObj(SWIG_as_voidptr(DataArrayDouble::Divide(self,aaa)),SWIGTYPE_p_ParaMEDMEM__DataArrayDouble, SWIG_POINTER_OWN | 0 );
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayDouble *__rdiv__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __rdiv__ !";
        double val;
        DataArrayDouble *a;
        DataArrayDoubleTuple *aa;
        std::vector<double> bb;
        int sw;
        convertObjToPossibleCpp5(obj,sw,val,a,aa,bb);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=self->deepCpy();
              ret->applyInv(val);
              return ret.retn();
            }
          case 3:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=aa->buildDADouble(1,self->getNumberOfComponents());
              return DataArrayDouble::Divide(aaa,self);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=DataArrayDouble::New(); aaa->useArray(&bb[0],false,CPP_DEALLOC,1,(int)bb.size());
              return DataArrayDouble::Divide(aaa,self);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      PyObject *___idiv___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __idiv__ !";
        double val;
        DataArrayDouble *a;
        DataArrayDoubleTuple *aa;
        std::vector<double> bb;
        int sw;
        convertObjToPossibleCpp5(obj,sw,val,a,aa,bb);
        switch(sw)
          {
          case 1:
            {
              if(val==0.)
                throw INTERP_KERNEL::Exception("DataArrayDouble::__div__ : trying to divide by zero !");
              self->applyLin(1./val,0.);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 2:
            {
              self->divideEqual(a);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 3:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=aa->buildDADouble(1,self->getNumberOfComponents());
              self->divideEqual(aaa);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=DataArrayDouble::New(); aaa->useArray(&bb[0],false,CPP_DEALLOC,1,(int)bb.size());
              self->divideEqual(aaa);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }
   
      DataArrayDouble *__pow__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __pow__ !";
        double val;
        DataArrayDouble *a;
        DataArrayDoubleTuple *aa;
        std::vector<double> bb;
        int sw;
        convertObjToPossibleCpp5(obj,sw,val,a,aa,bb);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=self->deepCpy();
              ret->applyPow(val);
              return ret.retn();
            }
          case 2:
            {
              return DataArrayDouble::Pow(self,a);
            }
          case 3:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=aa->buildDADouble(1,self->getNumberOfComponents());
              return DataArrayDouble::Pow(self,aaa);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=DataArrayDouble::New(); aaa->useArray(&bb[0],false,CPP_DEALLOC,1,(int)bb.size());
              return DataArrayDouble::Pow(self,aaa);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayDouble *__rpow__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __rpow__ !";
        double val;
        DataArrayDouble *a;
        DataArrayDoubleTuple *aa;
        std::vector<double> bb;
        int sw;
        convertObjToPossibleCpp5(obj,sw,val,a,aa,bb);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=self->deepCpy();
              ret->applyRPow(val);
              return ret.retn();
            }
          case 3:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=aa->buildDADouble(1,self->getNumberOfComponents());
              return DataArrayDouble::Pow(aaa,self);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=DataArrayDouble::New(); aaa->useArray(&bb[0],false,CPP_DEALLOC,1,(int)bb.size());
              return DataArrayDouble::Pow(aaa,self);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      PyObject *___ipow___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __ipow__ !";
        double val;
        DataArrayDouble *a;
        DataArrayDoubleTuple *aa;
        std::vector<double> bb;
        int sw;
        convertObjToPossibleCpp5(obj,sw,val,a,aa,bb);
        switch(sw)
          {
          case 1:
            {
              self->applyPow(val);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 2:
            {
              self->powEqual(a);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 3:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=aa->buildDADouble(1,self->getNumberOfComponents());
              self->powEqual(aaa);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> aaa=DataArrayDouble::New(); aaa->useArray(&bb[0],false,CPP_DEALLOC,1,(int)bb.size());
              self->powEqual(aaa);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }
   
      PyObject *computeTupleIdsNearTuples(const DataArrayDouble *other, double eps)
      {
        DataArrayInt *c=0,*cI=0;
        //
        self->computeTupleIdsNearTuples(other,eps,c,cI);
        PyObject *ret=PyTuple_New(2);
        PyTuple_SetItem(ret,0,SWIG_NewPointerObj(SWIG_as_voidptr(c),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 ));
        PyTuple_SetItem(ret,1,SWIG_NewPointerObj(SWIG_as_voidptr(cI),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 ));
        return ret;
      }
    }
  };

  class DataArrayDoubleTuple;

  class DataArrayDoubleIterator
  {
  public:
    DataArrayDoubleIterator(DataArrayDouble *da);
    ~DataArrayDoubleIterator();
    %extend
    {
      PyObject *next()
      {
        DataArrayDoubleTuple *ret=self->nextt();
        if(ret)
          return SWIG_NewPointerObj(SWIG_as_voidptr(ret),SWIGTYPE_p_ParaMEDMEM__DataArrayDoubleTuple,SWIG_POINTER_OWN|0);
        else
          {
            PyErr_SetString(PyExc_StopIteration,"No more data.");
            return 0;
          }
      }
    }
  };

  class DataArrayDoubleTuple
  {
  public:
    int getNumberOfCompo() const;
    DataArrayDouble *buildDADouble(int nbOfTuples, int nbOfCompo) const throw(INTERP_KERNEL::Exception);
    %extend
    {
      std::string __str__() const throw(INTERP_KERNEL::Exception)
      {
        return self->repr();
      }

      double __float__() const throw(INTERP_KERNEL::Exception)
      {
        return self->doubleValue();
      }

      DataArrayDouble *buildDADouble()
      {
        return self->buildDADouble(1,self->getNumberOfCompo());
      }

      PyObject *___iadd___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=self->buildDADouble(1,self->getNumberOfCompo());
        ParaMEDMEM_DataArrayDouble____iadd___(ret,0,obj);
        Py_XINCREF(trueSelf);
        return trueSelf;
      }
  
      PyObject *___isub___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=self->buildDADouble(1,self->getNumberOfCompo());
        ParaMEDMEM_DataArrayDouble____isub___(ret,0,obj);
        Py_XINCREF(trueSelf);
        return trueSelf;
      }
  
      PyObject *___imul___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=self->buildDADouble(1,self->getNumberOfCompo());
        ParaMEDMEM_DataArrayDouble____imul___(ret,0,obj);
        Py_XINCREF(trueSelf);
        return trueSelf;
      }

      PyObject *___idiv___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        MEDCouplingAutoRefCountObjectPtr<DataArrayDouble> ret=self->buildDADouble(1,self->getNumberOfCompo());
        ParaMEDMEM_DataArrayDouble____idiv___(ret,0,obj);
        Py_XINCREF(trueSelf);
        return trueSelf;
      }

      PyObject *__getitem__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg2[]="DataArrayDoubleTuple::__getitem__ : Mismatch of slice values in 2nd parameter (components) !";
        int sw;
        int singleVal;
        std::vector<int> multiVal;
        std::pair<int, std::pair<int,int> > slic;
        ParaMEDMEM::DataArrayInt *daIntTyypp=0;
        const double *pt=self->getConstPointer();
        int nbc=self->getNumberOfCompo();
        convertObjToPossibleCpp2(obj,nbc,sw,singleVal,multiVal,slic,daIntTyypp);
        switch(sw)
          {
          case 1:
            {
              if(singleVal>=nbc)
                {
                  std::ostringstream oss;
                  oss << "Requesting for id " << singleVal << " having only " << nbc << " components !";
                  throw INTERP_KERNEL::Exception(oss.str().c_str());
                }
              if(singleVal>=0)
                return PyFloat_FromDouble(pt[singleVal]);
              else
                {
                  if(nbc+singleVal>0)
                    return PyFloat_FromDouble(pt[nbc+singleVal]);
                  else
                    {
                      std::ostringstream oss;
                      oss << "Requesting for id " << singleVal << " having only " << nbc << " components !";
                      throw INTERP_KERNEL::Exception(oss.str().c_str());
                    }
                }
            }
          case 2:
            {
              PyObject *t=PyTuple_New(multiVal.size());
              for(int j=0;j<(int)multiVal.size();j++)
                {
                  int cid=multiVal[j];
                  if(cid>=nbc)
                    {
                      std::ostringstream oss;
                      oss << "Requesting for id #" << cid << " having only " << nbc << " components !";
                      throw INTERP_KERNEL::Exception(oss.str().c_str());
                    }
                  PyTuple_SetItem(t,j,PyFloat_FromDouble(pt[cid]));
                }
              return t;
            }
          case 3:
            {
              int sz=DataArray::GetNumberOfItemGivenBES(slic.first,slic.second.first,slic.second.second,msg2);
              PyObject *t=PyTuple_New(sz);
              for(int j=0;j<sz;j++)
                PyTuple_SetItem(t,j,PyFloat_FromDouble(pt[slic.first+j*slic.second.second]));
              return t;
            }
          default:
            throw INTERP_KERNEL::Exception("DataArrayDoubleTuple::__getitem__ : unrecognized type entered !");
          }
      }

      DataArrayDoubleTuple *__setitem__(PyObject *obj, PyObject *value) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="DataArrayDoubleTuple::__setitem__ : unrecognized type entered, int, slice, list<int>, tuple<int> !";
        const char msg2[]="DataArrayDoubleTuple::__setitem__ : Mismatch of slice values in 2nd parameter (components) !";
        int sw1,sw2;
        double singleValV;
        std::vector<double> multiValV;
        ParaMEDMEM::DataArrayDoubleTuple *daIntTyyppV=0;
        int nbc=self->getNumberOfCompo();
        convertObjToPossibleCpp44(value,sw1,singleValV,multiValV,daIntTyyppV);
        int singleVal;
        std::vector<int> multiVal;
        std::pair<int, std::pair<int,int> > slic;
        ParaMEDMEM::DataArrayInt *daIntTyypp=0;
        double *pt=self->getPointer();
        convertObjToPossibleCpp2(obj,nbc,sw2,singleVal,multiVal,slic,daIntTyypp);
        switch(sw2)
          {
          case 1:
            {
              if(singleVal>=nbc)
                {
                  std::ostringstream oss;
                  oss << "Requesting for setting id # " << singleVal << " having only " << nbc << " components !";
                  throw INTERP_KERNEL::Exception(oss.str().c_str());
                }
              switch(sw1)
                {
                case 1:
                  {
                    pt[singleVal]=singleValV;
                    return self;
                  }
                case 2:
                  {
                    if(multiValV.size()!=1)
                      {
                        std::ostringstream oss;
                        oss << "Requesting for setting id # " << singleVal << " with a list or tuple with size != 1 ! ";
                        throw INTERP_KERNEL::Exception(oss.str().c_str());
                      }
                    pt[singleVal]=multiValV[0];
                    return self;
                  }
                case 3:
                  {
                    pt[singleVal]=daIntTyyppV->getConstPointer()[0];
                    return self;
                  }
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
            }
          case 2:
            {
              switch(sw1)
                {
                case 1:
                  {
                    for(std::vector<int>::const_iterator it=multiVal.begin();it!=multiVal.end();it++)
                      {
                        if(*it>=nbc)
                          {
                            std::ostringstream oss;
                            oss << "Requesting for setting id # " << *it << " having only " << nbc << " components !";
                            throw INTERP_KERNEL::Exception(oss.str().c_str());
                          }
                        pt[*it]=singleValV;
                      }
                    return self;
                  }
                case 2:
                  {
                    if(multiVal.size()!=multiValV.size())
                      {
                        std::ostringstream oss;
                        oss << "Mismatch length of during assignment : " << multiValV.size() << " != " << multiVal.size() << " !";
                        throw INTERP_KERNEL::Exception(oss.str().c_str());
                      }
                    for(int i=0;i<(int)multiVal.size();i++)
                      {
                        int pos=multiVal[i];
                        if(pos>=nbc)
                          {
                            std::ostringstream oss;
                            oss << "Requesting for setting id # " << pos << " having only " << nbc << " components !";
                            throw INTERP_KERNEL::Exception(oss.str().c_str());
                          }
                        pt[multiVal[i]]=multiValV[i];
                      }
                    return self;
                  }
                case 3:
                  {
                    const double *ptV=daIntTyyppV->getConstPointer();
                    if(nbc>daIntTyyppV->getNumberOfCompo())
                      {
                        std::ostringstream oss;
                        oss << "Mismatch length of during assignment : " << nbc << " != " << daIntTyyppV->getNumberOfCompo() << " !";
                        throw INTERP_KERNEL::Exception(oss.str().c_str());
                      }
                    std::copy(ptV,ptV+nbc,pt);
                    return self;
                  }
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
            }
          case 3:
            {
              int sz=DataArray::GetNumberOfItemGivenBES(slic.first,slic.second.first,slic.second.second,msg2);
              switch(sw1)
                {
                case 1:
                  {
                    for(int j=0;j<sz;j++)
                      pt[slic.first+j*slic.second.second]=singleValV;
                    return self;
                  }
                case 2:
                  {
                    if(sz!=(int)multiValV.size())
                      {
                        std::ostringstream oss;
                        oss << "Mismatch length of during assignment : " << multiValV.size() << " != " << sz << " !";
                        throw INTERP_KERNEL::Exception(oss.str().c_str());
                      }
                    for(int j=0;j<sz;j++)
                      pt[slic.first+j*slic.second.second]=multiValV[j];
                    return self;
                  }
                case 3:
                  {
                    const double *ptV=daIntTyyppV->getConstPointer();
                    if(sz>daIntTyyppV->getNumberOfCompo())
                      {
                        std::ostringstream oss;
                        oss << "Mismatch length of during assignment : " << nbc << " != " << daIntTyyppV->getNumberOfCompo() << " !";
                        throw INTERP_KERNEL::Exception(oss.str().c_str());
                      }
                    for(int j=0;j<sz;j++)
                      pt[slic.first+j*slic.second.second]=ptV[j];
                    return self;
                  }
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }
    }
  };

  class DataArrayIntIterator;

  class DataArrayInt : public DataArray
  {
  public:
    static DataArrayInt *New();
    void checkAllocated() const throw(INTERP_KERNEL::Exception);
    int intValue() const throw(INTERP_KERNEL::Exception);
    int getHashCode() const throw(INTERP_KERNEL::Exception);
    bool empty() const throw(INTERP_KERNEL::Exception);
    DataArrayInt *deepCpy() const throw(INTERP_KERNEL::Exception);
    DataArrayInt *performCpy(bool deepCpy) const throw(INTERP_KERNEL::Exception);
    void cpyFrom(const DataArrayInt& other) throw(INTERP_KERNEL::Exception);
    void reserve(std::size_t nbOfElems) throw(INTERP_KERNEL::Exception);
    void pushBackSilent(int val) throw(INTERP_KERNEL::Exception);
    int popBackSilent() throw(INTERP_KERNEL::Exception);
    void pack() const throw(INTERP_KERNEL::Exception);
    void allocIfNecessary(int nbOfTuple, int nbOfCompo) throw(INTERP_KERNEL::Exception);
    bool isEqual(const DataArrayInt& other) const throw(INTERP_KERNEL::Exception);
    bool isEqualWithoutConsideringStr(const DataArrayInt& other) const throw(INTERP_KERNEL::Exception);
    bool isEqualWithoutConsideringStrAndOrder(const DataArrayInt& other) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *buildPermutationArr(const DataArrayInt& other) const throw(INTERP_KERNEL::Exception);
    void sort(bool asc=true) throw(INTERP_KERNEL::Exception);
    void reverse() throw(INTERP_KERNEL::Exception);
    void checkMonotonic(bool increasing) const throw(INTERP_KERNEL::Exception);
    bool isMonotonic(bool increasing) const throw(INTERP_KERNEL::Exception);
    void checkStrictlyMonotonic(bool increasing) const throw(INTERP_KERNEL::Exception);
    bool isStrictlyMonotonic(bool increasing) const throw(INTERP_KERNEL::Exception);
    void fillWithZero() throw(INTERP_KERNEL::Exception);
    void fillWithValue(int val) throw(INTERP_KERNEL::Exception);
    void iota(int init=0) throw(INTERP_KERNEL::Exception);
    std::string repr() const throw(INTERP_KERNEL::Exception);
    std::string reprZip() const throw(INTERP_KERNEL::Exception);
    DataArrayInt *invertArrayO2N2N2O(int newNbOfElem) const;
    DataArrayInt *invertArrayN2O2O2N(int oldNbOfElem) const;
    DataArrayInt *invertArrayO2N2N2OBis(int newNbOfElem) const throw(INTERP_KERNEL::Exception);
    void reAlloc(int nbOfTuples) throw(INTERP_KERNEL::Exception);
    DataArrayDouble *convertToDblArr() const;
    DataArrayInt *fromNoInterlace() const throw(INTERP_KERNEL::Exception);
    DataArrayInt *toNoInterlace() const throw(INTERP_KERNEL::Exception);
    DataArrayInt *selectByTupleId2(int bg, int end, int step) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *checkAndPreparePermutation() const throw(INTERP_KERNEL::Exception);
    DataArrayInt *buildPermArrPerLevel() const throw(INTERP_KERNEL::Exception);
    bool isIdentity() const throw(INTERP_KERNEL::Exception);
    bool isUniform(int val) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *substr(int tupleIdBg, int tupleIdEnd=-1) const throw(INTERP_KERNEL::Exception);
    void rearrange(int newNbOfCompo) throw(INTERP_KERNEL::Exception);
    void transpose() throw(INTERP_KERNEL::Exception);
    DataArrayInt *changeNbOfComponents(int newNbOfComp, int dftValue) const throw(INTERP_KERNEL::Exception);
    void meldWith(const DataArrayInt *other) throw(INTERP_KERNEL::Exception);
    void setPartOfValues1(const DataArrayInt *a, int bgTuples, int endTuples, int stepTuples, int bgComp, int endComp, int stepComp, bool strictCompoCompare=true) throw(INTERP_KERNEL::Exception);
    void setPartOfValuesSimple1(int a, int bgTuples, int endTuples, int stepTuples, int bgComp, int endComp, int stepComp) throw(INTERP_KERNEL::Exception);
    void setPartOfValuesAdv(const DataArrayInt *a, const DataArrayInt *tuplesSelec) throw(INTERP_KERNEL::Exception);
    void getTuple(int tupleId, int *res) const throw(INTERP_KERNEL::Exception);
    int getIJ(int tupleId, int compoId) const throw(INTERP_KERNEL::Exception);
    int getIJSafe(int tupleId, int compoId) const throw(INTERP_KERNEL::Exception);
    int back() const throw(INTERP_KERNEL::Exception);
    void setIJ(int tupleId, int compoId, int newVal) throw(INTERP_KERNEL::Exception);
    void setIJSilent(int tupleId, int compoId, int newVal) throw(INTERP_KERNEL::Exception);
    int *getPointer() throw(INTERP_KERNEL::Exception);
    const int *getConstPointer() const throw(INTERP_KERNEL::Exception);
    DataArrayIntIterator *iterator() throw(INTERP_KERNEL::Exception);
    const int *begin() const throw(INTERP_KERNEL::Exception);
    const int *end() const throw(INTERP_KERNEL::Exception);
    DataArrayInt *getIdsEqual(int val) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *getIdsNotEqual(int val) const throw(INTERP_KERNEL::Exception);
    int changeValue(int oldValue, int newValue) throw(INTERP_KERNEL::Exception);
    int locateTuple(const std::vector<int>& tupl) const throw(INTERP_KERNEL::Exception);
    int locateValue(int value) const throw(INTERP_KERNEL::Exception);
    int locateValue(const std::vector<int>& vals) const throw(INTERP_KERNEL::Exception);
    int search(const std::vector<int>& vals) const throw(INTERP_KERNEL::Exception);
    bool presenceOfTuple(const std::vector<int>& tupl) const throw(INTERP_KERNEL::Exception);
    bool presenceOfValue(int value) const throw(INTERP_KERNEL::Exception);
    bool presenceOfValue(const std::vector<int>& vals) const throw(INTERP_KERNEL::Exception);
    void accumulate(int *res) const throw(INTERP_KERNEL::Exception);
    int accumulate(int compId) const throw(INTERP_KERNEL::Exception);
    int getMaxValue(int& tupleId) const throw(INTERP_KERNEL::Exception);
    int getMaxValueInArray() const throw(INTERP_KERNEL::Exception);
    int getMinValue(int& tupleId) const throw(INTERP_KERNEL::Exception);
    int getMinValueInArray() const throw(INTERP_KERNEL::Exception);
    void abs() throw(INTERP_KERNEL::Exception);
    void applyLin(int a, int b, int compoId) throw(INTERP_KERNEL::Exception);
    void applyLin(int a, int b) throw(INTERP_KERNEL::Exception);
    void applyInv(int numerator) throw(INTERP_KERNEL::Exception);
    DataArrayInt *negate() const throw(INTERP_KERNEL::Exception);
    void applyDivideBy(int val) throw(INTERP_KERNEL::Exception);
    void applyModulus(int val) throw(INTERP_KERNEL::Exception);
    void applyRModulus(int val) throw(INTERP_KERNEL::Exception);
    void applyPow(int val) throw(INTERP_KERNEL::Exception);
    void applyRPow(int val) throw(INTERP_KERNEL::Exception);
    DataArrayInt *getIdsInRange(int vmin, int vmax) const throw(INTERP_KERNEL::Exception);
    static DataArrayInt *Aggregate(const DataArrayInt *a1, const DataArrayInt *a2, int offsetA2);
    static DataArrayInt *Aggregate(const std::vector<const DataArrayInt *>& arr) throw(INTERP_KERNEL::Exception);
    static DataArrayInt *Meld(const DataArrayInt *a1, const DataArrayInt *a2) throw(INTERP_KERNEL::Exception);
    static DataArrayInt *Meld(const std::vector<const DataArrayInt *>& arr) throw(INTERP_KERNEL::Exception);
    static DataArrayInt *MakePartition(const std::vector<const DataArrayInt *>& groups, int newNb, std::vector< std::vector<int> >& fidsOfGroups) throw(INTERP_KERNEL::Exception);
    static DataArrayInt *BuildUnion(const std::vector<const DataArrayInt *>& arr) throw(INTERP_KERNEL::Exception);
    static DataArrayInt *BuildIntersection(const std::vector<const DataArrayInt *>& arr) throw(INTERP_KERNEL::Exception);
    DataArrayInt *buildComplement(int nbOfElement) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *buildSubstraction(const DataArrayInt *other) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *buildSubstractionOptimized(const DataArrayInt *other) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *buildUnion(const DataArrayInt *other) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *buildIntersection(const DataArrayInt *other) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *buildUnique() const throw(INTERP_KERNEL::Exception);
    DataArrayInt *deltaShiftIndex() const throw(INTERP_KERNEL::Exception);
    void computeOffsets() throw(INTERP_KERNEL::Exception);
    void computeOffsets2() throw(INTERP_KERNEL::Exception);
    DataArrayInt *buildExplicitArrByRanges(const DataArrayInt *offsets) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *findRangeIdForEachTuple(const DataArrayInt *ranges) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *findIdInRangeForEachTuple(const DataArrayInt *ranges) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *duplicateEachTupleNTimes(int nbTimes) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *getDifferentValues() const throw(INTERP_KERNEL::Exception);
    static DataArrayInt *Add(const DataArrayInt *a1, const DataArrayInt *a2) throw(INTERP_KERNEL::Exception);
    void addEqual(const DataArrayInt *other) throw(INTERP_KERNEL::Exception);
    static DataArrayInt *Substract(const DataArrayInt *a1, const DataArrayInt *a2) throw(INTERP_KERNEL::Exception);
    void substractEqual(const DataArrayInt *other) throw(INTERP_KERNEL::Exception);
    static DataArrayInt *Multiply(const DataArrayInt *a1, const DataArrayInt *a2) throw(INTERP_KERNEL::Exception);
    void multiplyEqual(const DataArrayInt *other) throw(INTERP_KERNEL::Exception);
    static DataArrayInt *Divide(const DataArrayInt *a1, const DataArrayInt *a2) throw(INTERP_KERNEL::Exception);
    void divideEqual(const DataArrayInt *other) throw(INTERP_KERNEL::Exception);
    static DataArrayInt *Modulus(const DataArrayInt *a1, const DataArrayInt *a2) throw(INTERP_KERNEL::Exception);
    void modulusEqual(const DataArrayInt *other) throw(INTERP_KERNEL::Exception);
    static DataArrayInt *Pow(const DataArrayInt *a1, const DataArrayInt *a2) throw(INTERP_KERNEL::Exception);
    void powEqual(const DataArrayInt *other) throw(INTERP_KERNEL::Exception);
  public:
    static int *CheckAndPreparePermutation(const int *start, const int *end);
    static DataArrayInt *Range(int begin, int end, int step) throw(INTERP_KERNEL::Exception);
    %extend
    {
      DataArrayInt() throw(INTERP_KERNEL::Exception)
        {
          return DataArrayInt::New();
        }

      static DataArrayInt *New(PyObject *elt0, PyObject *nbOfTuples=0, PyObject *nbOfComp=0) throw(INTERP_KERNEL::Exception)
      {
        const char *msgBase="ParaMEDMEM::DataArrayInt::New : Available API are : \n-DataArrayInt.New()\n-DataArrayInt.New([1,3,4])\n-DataArrayInt.New([1,3,4],3)\n-DataArrayInt.New([1,3,4,5],2,2)\n-DataArrayInt.New([1,3,4,5,7,8],3,2)\n-DataArrayInt.New([(1,3),(4,5),(7,8)])\n-DataArrayInt.New(5)\n-DataArrayInt.New(5,2)";
        std::string msg(msgBase);
#ifdef WITH_NUMPY
        msg+="\n-DataArrayInt.New(numpy array with dtype=int32)";
#endif
        msg+=" !";
        if(PyList_Check(elt0) || PyTuple_Check(elt0))
          {
            if(nbOfTuples)
              {
                if(PyInt_Check(nbOfTuples))
                  {
                    int nbOfTuples1=PyInt_AS_LONG(nbOfTuples);
                    if(nbOfTuples1<0)
                      throw INTERP_KERNEL::Exception("DataArrayInt::New : should be a positive set of allocated memory !");
                    if(nbOfComp)
                      {
                        if(PyInt_Check(nbOfComp))
                          {//DataArrayInt.New([1,3,4,5],2,2)
                            int nbOfCompo=PyInt_AS_LONG(nbOfComp);
                            if(nbOfCompo<0)
                              throw INTERP_KERNEL::Exception("DataArrayInt::New : should be a positive number of components !");
                            MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New();
                            std::vector<int> tmp=fillArrayWithPyListInt2(elt0,nbOfTuples1,nbOfCompo);
                            ret->alloc(nbOfTuples1,nbOfCompo); std::copy(tmp.begin(),tmp.end(),ret->getPointer());
                            return ret.retn();
                          }
                        else
                          throw INTERP_KERNEL::Exception(msg.c_str());
                      }
                    else
                      {//DataArrayInt.New([1,3,4],3)
                        MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New();
                        int tmpp1=-1;
                        std::vector<int> tmp=fillArrayWithPyListInt2(elt0,nbOfTuples1,tmpp1);
                        ret->alloc(nbOfTuples1,tmpp1); std::copy(tmp.begin(),tmp.end(),ret->getPointer());
                        return ret.retn();
                      }
                  }
                else
                  throw INTERP_KERNEL::Exception(msg.c_str());
              }
            else
              {// DataArrayInt.New([1,3,4])
                MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New();
                int tmpp1=-1,tmpp2=-1;
                std::vector<int> tmp=fillArrayWithPyListInt2(elt0,tmpp1,tmpp2);
                ret->alloc(tmpp1,tmpp2); std::copy(tmp.begin(),tmp.end(),ret->getPointer());
                return ret.retn();
              }
          }
        else if(PyInt_Check(elt0))
          {
            int nbOfTuples1=PyInt_AS_LONG(elt0);
            if(nbOfTuples1<0)
              throw INTERP_KERNEL::Exception("DataArrayInt::New : should be a positive set of allocated memory !");
            if(nbOfTuples)
              {
                if(!nbOfComp)
                  {
                    if(PyInt_Check(nbOfTuples))
                      {//DataArrayInt.New(5,2)
                        int nbOfCompo=PyInt_AS_LONG(nbOfTuples);
                        if(nbOfCompo<0)
                          throw INTERP_KERNEL::Exception("DataArrayInt::New : should be a positive number of components !");
                        MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New();
                        ret->alloc(nbOfTuples1,nbOfCompo);
                        return ret.retn();
                      }
                    else
                      throw INTERP_KERNEL::Exception(msg.c_str());
                  }
                else
                  throw INTERP_KERNEL::Exception(msg.c_str());
              }
            else
              {//DataArrayInt.New(5)
                MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=DataArrayInt::New();
                ret->alloc(nbOfTuples1,1);
                return ret.retn();
              }
          }
#ifdef WITH_NUMPY
        else if(PyArray_Check(elt0) && nbOfTuples==NULL && nbOfComp==NULL)
          {//DataArrayInt.New(numpyArray)
            return BuildNewInstance<DataArrayInt,int>(elt0,NPY_INT,&PyCallBackDataArrayInt_RefType,"INT32");
          }
#endif
        else
          throw INTERP_KERNEL::Exception(msg.c_str());
      }

      DataArrayInt(PyObject *elt0, PyObject *nbOfTuples=0, PyObject *nbOfComp=0) throw(INTERP_KERNEL::Exception)
        {
          return ParaMEDMEM_DataArrayInt_New__SWIG_1(elt0,nbOfTuples,nbOfComp);
        }

      std::string __str__() const throw(INTERP_KERNEL::Exception)
      {
        return self->repr();
      }

      int __len__() const throw(INTERP_KERNEL::Exception)
      {
        if(self->isAllocated())
          {
            return self->getNumberOfTuples();
          }
        else
          {
            throw INTERP_KERNEL::Exception("DataArrayInt::__len__ : Instance is NOT allocated !");
          }
      }

      int __int__() const throw(INTERP_KERNEL::Exception)
      {
        return self->intValue();
      }

      DataArrayIntIterator *__iter__()
      {
        return self->iterator();
      }
   
      PyObject *accumulate() const throw(INTERP_KERNEL::Exception)
      {
        int sz=self->getNumberOfComponents();
        INTERP_KERNEL::AutoPtr<int> tmp=new int[sz];
        self->accumulate(tmp);
        return convertIntArrToPyList(tmp,sz);
      }
   
      static PyObject *BuildOld2NewArrayFromSurjectiveFormat2(int nbOfOldTuples, PyObject *arr, PyObject *arrI) throw(INTERP_KERNEL::Exception)
      {
        int newNbOfTuples=-1;
        int szArr,szArrI,sw,iTypppArr,iTypppArrI;
        std::vector<int> stdvecTyyppArr,stdvecTyyppArrI;
        const int *arrPtr=convertObjToPossibleCpp1_Safe(arr,sw,szArr,iTypppArr,stdvecTyyppArr);
        const int *arrIPtr=convertObjToPossibleCpp1_Safe(arrI,sw,szArrI,iTypppArrI,stdvecTyyppArrI);
        DataArrayInt *ret0=ParaMEDMEM::DataArrayInt::BuildOld2NewArrayFromSurjectiveFormat2(nbOfOldTuples,arrPtr,arrIPtr,arrIPtr+szArrI,newNbOfTuples);
        PyObject *ret=PyTuple_New(2);
        PyTuple_SetItem(ret,0,SWIG_NewPointerObj((void*)ret0,SWIGTYPE_p_ParaMEDMEM__DataArrayInt,SWIG_POINTER_OWN | 0));
        PyTuple_SetItem(ret,1,PyInt_FromLong(newNbOfTuples));
        return ret;
      }

      void setValues(PyObject *li, PyObject *nbOfTuples=0, PyObject *nbOfComp=0) throw(INTERP_KERNEL::Exception)
      {
        const char *msg="ParaMEDMEM::DataArrayInt::setValues : Available API are : \n-DataArrayInt.setValues([1,3,4])\n-DataArrayInt.setValues([1,3,4],3)\n-DataArrayInt.setValues([1,3,4,5],2,2)\n-DataArrayInt.New(5)\n !";
        if(PyList_Check(li) || PyTuple_Check(li))
          {
            if(nbOfTuples)
              {
                if(PyInt_Check(nbOfTuples))
                  {
                    int nbOfTuples1=PyInt_AS_LONG(nbOfTuples);
                    if(nbOfTuples<0)
                      throw INTERP_KERNEL::Exception("DataArrayInt::setValue : should be a positive set of allocated memory !");
                    if(nbOfComp)
                      {
                        if(PyInt_Check(nbOfComp))
                          {//DataArrayInt.setValues([1,3,4,5],2,2)
                            int nbOfCompo=PyInt_AS_LONG(nbOfComp);
                            if(nbOfCompo<0)
                              throw INTERP_KERNEL::Exception("DataArrayInt::setValue : should be a positive number of components !");
                            std::vector<int> tmp=fillArrayWithPyListInt2(li,nbOfTuples1,nbOfCompo);
                            self->alloc(nbOfTuples1,nbOfCompo); std::copy(tmp.begin(),tmp.end(),self->getPointer());
                          }
                        else
                          throw INTERP_KERNEL::Exception(msg);
                      }
                    else
                      {//DataArrayInt.setValues([1,3,4],3)
                        int tmpp1=-1;
                        std::vector<int> tmp=fillArrayWithPyListInt2(li,nbOfTuples1,tmpp1);
                        self->alloc(nbOfTuples1,tmpp1); std::copy(tmp.begin(),tmp.end(),self->getPointer());
                      }
                  }
                else
                  throw INTERP_KERNEL::Exception(msg);
              }
            else
              {// DataArrayInt.setValues([1,3,4])
                int tmpp1=-1,tmpp2=-1;
                std::vector<int> tmp=fillArrayWithPyListInt2(li,tmpp1,tmpp2);
                self->alloc(tmpp1,tmpp2); std::copy(tmp.begin(),tmp.end(),self->getPointer());
              }
          }
        else
          throw INTERP_KERNEL::Exception(msg);
      }

      PyObject *getValues() const throw(INTERP_KERNEL::Exception)
      {
        const int *vals=self->getConstPointer();
        return convertIntArrToPyList(vals,self->getNbOfElems());
      }

#ifdef WITH_NUMPY
      PyObject *toNumPyArray() throw(INTERP_KERNEL::Exception) // not const. It is not a bug !
      {
        return ToNumPyArray<DataArrayInt,int>(self,NPY_INT,"DataArrayInt");
      }
#endif

      PyObject *isEqualIfNotWhy(const DataArrayInt& other) const throw(INTERP_KERNEL::Exception)
      {
        std::string ret1;
        bool ret0=self->isEqualIfNotWhy(other,ret1);
        PyObject *ret=PyTuple_New(2);
        PyObject *ret0Py=ret0?Py_True:Py_False;
        Py_XINCREF(ret0Py);
        PyTuple_SetItem(ret,0,ret0Py);
        PyTuple_SetItem(ret,1,PyString_FromString(ret1.c_str()));
        return ret;
      }

      PyObject *getValuesAsTuple() const throw(INTERP_KERNEL::Exception)
      {
        const int *vals=self->getConstPointer();
        int nbOfComp=self->getNumberOfComponents();
        int nbOfTuples=self->getNumberOfTuples();
        return convertIntArrToPyListOfTuple(vals,nbOfComp,nbOfTuples);
      }

      static PyObject *MakePartition(PyObject *gps, int newNb) throw(INTERP_KERNEL::Exception)
      {
        std::vector<const DataArrayInt *> groups;
        std::vector< std::vector<int> > fidsOfGroups;
        convertFromPyObjVectorOfObj<const ParaMEDMEM::DataArrayInt *>(gps,SWIGTYPE_p_ParaMEDMEM__DataArrayInt,"DataArrayInt",groups);
        ParaMEDMEM::DataArrayInt *ret0=ParaMEDMEM::DataArrayInt::MakePartition(groups,newNb,fidsOfGroups);
        PyObject *ret = PyList_New(2);
        PyList_SetItem(ret,0,SWIG_NewPointerObj(SWIG_as_voidptr(ret0),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 ));
        int sz=fidsOfGroups.size();
        PyObject *ret1 = PyList_New(sz);
        for(int i=0;i<sz;i++)
          PyList_SetItem(ret1,i,convertIntArrToPyList2(fidsOfGroups[i]));
        PyList_SetItem(ret,1,ret1);
        return ret;
      }

      void transformWithIndArr(PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            self->transformWithIndArr(tmp,tmp+size);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            self->transformWithIndArr(da2->getConstPointer(),da2->getConstPointer()+da2->getNbOfElems());
          }
      }

      DataArrayInt *getIdsEqualList(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        int sw;
        int singleVal;
        std::vector<int> multiVal;
        std::pair<int, std::pair<int,int> > slic;
        ParaMEDMEM::DataArrayInt *daIntTyypp=0;
        convertObjToPossibleCpp2(obj,self->getNumberOfTuples(),sw,singleVal,multiVal,slic,daIntTyypp);
        switch(sw)
          {
          case 1:
            return self->getIdsEqualList(&singleVal,&singleVal+1);
          case 2:
            return self->getIdsEqualList(&multiVal[0],&multiVal[0]+multiVal.size());
          case 4:
            return self->getIdsEqualList(daIntTyypp->begin(),daIntTyypp->end());
          default:
            throw INTERP_KERNEL::Exception("DataArrayInt::getIdsEqualList : unrecognized type entered, expected list of int, tuple of int or DataArrayInt !");
          }
      }

      DataArrayInt *getIdsNotEqualList(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        int sw;
        int singleVal;
        std::vector<int> multiVal;
        std::pair<int, std::pair<int,int> > slic;
        ParaMEDMEM::DataArrayInt *daIntTyypp=0;
        convertObjToPossibleCpp2(obj,self->getNumberOfTuples(),sw,singleVal,multiVal,slic,daIntTyypp);
        switch(sw)
          {
          case 1:
            return self->getIdsNotEqualList(&singleVal,&singleVal+1);
          case 2:
            return self->getIdsNotEqualList(&multiVal[0],&multiVal[0]+multiVal.size());
          case 4:
            return self->getIdsNotEqualList(daIntTyypp->begin(),daIntTyypp->end());
          default:
            throw INTERP_KERNEL::Exception("DataArrayInt::getIdsNotEqualList : unrecognized type entered, expected list of int, tuple of int or DataArrayInt !");
          }
      }

      PyObject *splitByValueRange(PyObject *li) const throw(INTERP_KERNEL::Exception)
      {
        DataArrayInt *ret0=0,*ret1=0,*ret2=0;
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            self->splitByValueRange(tmp,(int *)tmp+size,ret0,ret1,ret2);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            if(!da2)
              throw INTERP_KERNEL::Exception("Not null DataArrayInt instance expected !");
            da2->checkAllocated();
            int size=self->getNumberOfTuples();
            self->splitByValueRange(da2->getConstPointer(),da2->getConstPointer()+size,ret0,ret1,ret2);
          }
        PyObject *ret = PyList_New(3);
        PyList_SetItem(ret,0,SWIG_NewPointerObj(SWIG_as_voidptr(ret0),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 ));
        PyList_SetItem(ret,1,SWIG_NewPointerObj(SWIG_as_voidptr(ret1),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 ));
        PyList_SetItem(ret,2,SWIG_NewPointerObj(SWIG_as_voidptr(ret2),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 ));
        return ret;
      }

      DataArrayInt *transformWithIndArrR(PyObject *li) const
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            return self->transformWithIndArrR(tmp,tmp+size);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            return self->transformWithIndArrR(da2->getConstPointer(),da2->getConstPointer()+da2->getNbOfElems());
          }
      }

      DataArrayInt *renumberAndReduce(PyObject *li, int newNbOfTuple) throw(INTERP_KERNEL::Exception)
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumberAndReduce(tmp,newNbOfTuple);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            if(!da2)
              throw INTERP_KERNEL::Exception("Not null DataArrayInt instance expected !");
            da2->checkAllocated();
            int size=self->getNumberOfTuples();
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumberAndReduce(da2->getConstPointer(),newNbOfTuple);
          }
      }

      DataArrayInt *renumber(PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumber(tmp);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            if(!da2)
              throw INTERP_KERNEL::Exception("Not null DataArrayInt instance expected !");
            da2->checkAllocated();
            int size=self->getNumberOfTuples();
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumber(da2->getConstPointer());
          }
      }

      DataArrayInt *renumberR(PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumberR(tmp);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            if(!da2)
              throw INTERP_KERNEL::Exception("Not null DataArrayInt instance expected !");
            da2->checkAllocated();
            int size=self->getNumberOfTuples();
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumberR(da2->getConstPointer());
          }
      }

      DataArrayInt *selectByTupleId(PyObject *li) const throw(INTERP_KERNEL::Exception)
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            return self->selectByTupleId(tmp,tmp+size);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            if(!da2)
              throw INTERP_KERNEL::Exception("Not null DataArrayInt instance expected !");
            da2->checkAllocated();
            return self->selectByTupleId(da2->getConstPointer(),da2->getConstPointer()+da2->getNbOfElems());
          }
      }

      DataArrayInt *selectByTupleIdSafe(PyObject *li) const throw(INTERP_KERNEL::Exception)
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            return self->selectByTupleIdSafe(tmp,tmp+size);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            if(!da2)
              throw INTERP_KERNEL::Exception("Not null DataArrayInt instance expected !");
            da2->checkAllocated();
            return self->selectByTupleIdSafe(da2->getConstPointer(),da2->getConstPointer()+da2->getNbOfElems());
          }
      }

      DataArrayInt *keepSelectedComponents(PyObject *li) const throw(INTERP_KERNEL::Exception)
      {
        std::vector<int> tmp;
        convertPyToNewIntArr3(li,tmp);
        return self->keepSelectedComponents(tmp);
      }

      void setSelectedComponents(const DataArrayInt *a, PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        std::vector<int> tmp;
        convertPyToNewIntArr3(li,tmp);
        self->setSelectedComponents(a,tmp);
      }

      PyObject *getTuple(int tupleId) throw(INTERP_KERNEL::Exception)
      {
        int sz=self->getNumberOfComponents();
        INTERP_KERNEL::AutoPtr<int> tmp=new int[sz];
        self->getTuple(tupleId,tmp);
        return convertIntArrToPyList(tmp,sz);
      }

      PyObject *changeSurjectiveFormat(int targetNb) const throw(INTERP_KERNEL::Exception)
      {
        DataArrayInt *arr=0;
        DataArrayInt *arrI=0;
        self->changeSurjectiveFormat(targetNb,arr,arrI);
        PyObject *res = PyList_New(2);
        PyList_SetItem(res,0,SWIG_NewPointerObj((void*)arr,SWIGTYPE_p_ParaMEDMEM__DataArrayInt,SWIG_POINTER_OWN | 0));
        PyList_SetItem(res,1,SWIG_NewPointerObj((void*)arrI,SWIGTYPE_p_ParaMEDMEM__DataArrayInt,SWIG_POINTER_OWN | 0));
        return res;
      }

      DataArrayInt *selectByTupleRanges(PyObject *li) const throw(INTERP_KERNEL::Exception)
      {
        std::vector<std::pair<int,int> > ranges;
        convertPyToVectorPairInt(li,ranges);
        return self->selectByTupleRanges(ranges);
      }

      static DataArrayInt *Meld(PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        std::vector<const DataArrayInt *> tmp;
        convertFromPyObjVectorOfObj<const ParaMEDMEM::DataArrayInt *>(li,SWIGTYPE_p_ParaMEDMEM__DataArrayInt,"DataArrayInt",tmp);
        return DataArrayInt::Meld(tmp);
      }

      static DataArrayInt *Aggregate(PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        std::vector<const DataArrayInt *> tmp;
        convertFromPyObjVectorOfObj<const ParaMEDMEM::DataArrayInt *>(li,SWIGTYPE_p_ParaMEDMEM__DataArrayInt,"DataArrayInt",tmp);
        return DataArrayInt::Aggregate(tmp);
      }

      static DataArrayInt *BuildUnion(PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        std::vector<const DataArrayInt *> tmp;
        convertFromPyObjVectorOfObj<const ParaMEDMEM::DataArrayInt *>(li,SWIGTYPE_p_ParaMEDMEM__DataArrayInt,"DataArrayInt",tmp);
        return DataArrayInt::BuildUnion(tmp);
      }

      static DataArrayInt *BuildIntersection(PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        std::vector<const DataArrayInt *> tmp;
        convertFromPyObjVectorOfObj<const ParaMEDMEM::DataArrayInt *>(li,SWIGTYPE_p_ParaMEDMEM__DataArrayInt,"DataArrayInt",tmp);
        return DataArrayInt::BuildIntersection(tmp);
      }

      PyObject *getMaxValue() const throw(INTERP_KERNEL::Exception)
      {
        int tmp;
        int r1=self->getMaxValue(tmp);
        PyObject *ret=PyTuple_New(2);
        PyTuple_SetItem(ret,0,PyInt_FromLong(r1));
        PyTuple_SetItem(ret,1,PyInt_FromLong(tmp));
        return ret;
      }

      PyObject *getMinValue() const throw(INTERP_KERNEL::Exception)
      {
        int tmp;
        int r1=self->getMinValue(tmp);
        PyObject *ret=PyTuple_New(2);
        PyTuple_SetItem(ret,0,PyInt_FromLong(r1));
        PyTuple_SetItem(ret,1,PyInt_FromLong(tmp));
        return ret;
      }

      int index(PyObject *obj) const throw(INTERP_KERNEL::Exception)
      {
        int nbOfCompo=self->getNumberOfComponents();
        switch(nbOfCompo)
          {
          case 1:
            {
              if(PyInt_Check(obj))
                {
                  int val=(int)PyInt_AS_LONG(obj);
                  return self->locateValue(val);
                }
              else
                throw INTERP_KERNEL::Exception("DataArrayInt::index : 'this' contains one component and trying to find an element which is not an integer !");
            }
          default:
            {
              std::vector<int> arr;
              convertPyToNewIntArr3(obj,arr);
              return self->locateTuple(arr);
            }
          }
      }

      bool __contains__(PyObject *obj) const throw(INTERP_KERNEL::Exception)
      {
        int nbOfCompo=self->getNumberOfComponents();
        switch(nbOfCompo)
          {
          case 0:
            return false;
          case 1:
            {
              if(PyInt_Check(obj))
                {
                  int val=(int)PyInt_AS_LONG(obj);
                  return self->presenceOfValue(val);
                }
              else
                throw INTERP_KERNEL::Exception("DataArrayInt::__contains__ : 'this' contains one component and trying to find an element which is not an integer !");
            }
          default:
            {
              std::vector<int> arr;
              convertPyToNewIntArr3(obj,arr);
              return self->presenceOfTuple(arr);
            }
          }
      }

      PyObject *__getitem__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in DataArrayInt::__getitem__ !";
        const char msg2[]="DataArrayInt::__getitem__ : Mismatch of slice values in 2nd parameter (components) !";
        self->checkAllocated();
        int nbOfTuples=self->getNumberOfTuples();
        int nbOfComponents=self->getNumberOfComponents();
        int it1,ic1;
        std::vector<int> vt1,vc1;
        std::pair<int, std::pair<int,int> > pt1,pc1;
        DataArrayInt *dt1=0,*dc1=0;
        int sw;
        convertObjToPossibleCpp3(obj,nbOfTuples,nbOfComponents,sw,it1,ic1,vt1,vc1,pt1,pc1,dt1,dc1);
        MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret;
        switch(sw)
          {
          case 1:
            {
              if(nbOfComponents==1)
                return PyInt_FromLong(self->getIJSafe(it1,0));
              return SWIG_NewPointerObj(SWIG_as_voidptr(self->selectByTupleIdSafe(&it1,&it1+1)),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
            }
          case 2:
            return SWIG_NewPointerObj(SWIG_as_voidptr(self->selectByTupleIdSafe(&vt1[0],&vt1[0]+vt1.size())),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
          case 3:
            return SWIG_NewPointerObj(SWIG_as_voidptr(self->selectByTupleId2(pt1.first,pt1.second.first,pt1.second.second)),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
          case 4:
            return SWIG_NewPointerObj(SWIG_as_voidptr(self->selectByTupleIdSafe(dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems())),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
          case 5:
            return PyInt_FromLong(self->getIJSafe(it1,ic1));
          case 6:
            {
              ret=self->selectByTupleIdSafe(&vt1[0],&vt1[0]+vt1.size());
              std::vector<int> v2(1,ic1);
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(v2)),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
            }
          case 7:
            {
              ret=self->selectByTupleId2(pt1.first,pt1.second.first,pt1.second.second);
              std::vector<int> v2(1,ic1);
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(v2)),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
            }
          case 8:
            {
              ret=self->selectByTupleIdSafe(dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems());
              std::vector<int> v2(1,ic1);
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(v2)),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
            }
          case 9:
            {
              ret=self->selectByTupleIdSafe(&it1,&it1+1);
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(vc1)),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
            }
          case 10:
            {
              ret=self->selectByTupleIdSafe(&vt1[0],&vt1[0]+vt1.size());
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(vc1)),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
            }
          case 11:
            {
              ret=self->selectByTupleId2(pt1.first,pt1.second.first,pt1.second.second);
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(vc1)),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
            }
          case 12:
            {
              ret=self->selectByTupleIdSafe(dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems());
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(vc1)),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
            }
          case 13:
            {
              ret=self->selectByTupleIdSafe(&it1,&it1+1);
              int nbOfComp=DataArray::GetNumberOfItemGivenBESRelative(pc1.first,pc1.second.first,pc1.second.second,msg2);
              std::vector<int> v2(nbOfComp);
              for(int i=0;i<nbOfComp;i++)
                v2[i]=pc1.first+i*pc1.second.second;
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(v2)),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
            }
          case 14:
            {
              ret=self->selectByTupleIdSafe(&vt1[0],&vt1[0]+vt1.size());
              int nbOfComp=DataArray::GetNumberOfItemGivenBESRelative(pc1.first,pc1.second.first,pc1.second.second,msg2);
              std::vector<int> v2(nbOfComp);
              for(int i=0;i<nbOfComp;i++)
                v2[i]=pc1.first+i*pc1.second.second;
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(v2)),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
            }
          case 15:
            {
              ret=self->selectByTupleId2(pt1.first,pt1.second.first,pt1.second.second);
              int nbOfComp=DataArray::GetNumberOfItemGivenBESRelative(pc1.first,pc1.second.first,pc1.second.second,msg2);
              std::vector<int> v2(nbOfComp);
              for(int i=0;i<nbOfComp;i++)
                v2[i]=pc1.first+i*pc1.second.second;
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(v2)),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
            }
          case 16:
            {
              ret=self->selectByTupleIdSafe(dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems());
              int nbOfComp=DataArray::GetNumberOfItemGivenBESRelative(pc1.first,pc1.second.first,pc1.second.second,msg2);
              std::vector<int> v2(nbOfComp);
              for(int i=0;i<nbOfComp;i++)
                v2[i]=pc1.first+i*pc1.second.second;
              return SWIG_NewPointerObj(SWIG_as_voidptr(ret->keepSelectedComponents(v2)),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 );
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayInt *__setitem__(PyObject *obj, PyObject *value) throw(INTERP_KERNEL::Exception)
      {
        self->checkAllocated();
        const char msg[]="Unexpected situation in __setitem__ !";
        int nbOfTuples=self->getNumberOfTuples();
        int nbOfComponents=self->getNumberOfComponents();
        int sw1,sw2;
        int i1;
        std::vector<int> v1;
        DataArrayInt *d1=0;
        DataArrayIntTuple *dd1=0;
        convertObjToPossibleCpp1(value,sw1,i1,v1,d1,dd1);
        int it1,ic1;
        std::vector<int> vt1,vc1;
        std::pair<int, std::pair<int,int> > pt1,pc1;
        DataArrayInt *dt1=0,*dc1=0;
        convertObjToPossibleCpp3(obj,nbOfTuples,nbOfComponents,sw2,it1,ic1,vt1,vc1,pt1,pc1,dt1,dc1);
        MEDCouplingAutoRefCountObjectPtr<DataArrayInt> tmp;
        switch(sw2)
          {
          case 1:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple1(i1,it1,it1+1,1,0,nbOfComponents,1);
                  return self;
                case 2:
                  tmp=DataArrayInt::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues1(tmp,it1,it1+1,1,0,nbOfComponents,1,false);
                  return self;
                case 3:
                  self->setPartOfValues1(d1,it1,it1+1,1,0,nbOfComponents,1);
                  return self;
                case 4:
                  tmp=dd1->buildDAInt(1,self->getNumberOfComponents());
                  self->setPartOfValues1(tmp,it1,it1+1,1,0,nbOfComponents,1);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 2:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple3(i1,&vt1[0],&vt1[0]+vt1.size(),0,nbOfComponents,1);
                  return self;
                case 2:
                  tmp=DataArrayInt::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues3(tmp,&vt1[0],&vt1[0]+vt1.size(),0,nbOfComponents,1,false);
                  return self;
                case 3:
                  self->setPartOfValues3(d1,&vt1[0],&vt1[0]+vt1.size(),0,nbOfComponents,1);
                  return self;
                case 4:
                  tmp=dd1->buildDAInt(1,self->getNumberOfComponents());
                  self->setPartOfValues3(tmp,&vt1[0],&vt1[0]+vt1.size(),0,nbOfComponents,1);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 3:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple1(i1,pt1.first,pt1.second.first,pt1.second.second,0,nbOfComponents,1);
                  return self;
                case 2:
                  tmp=DataArrayInt::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues1(tmp,pt1.first,pt1.second.first,pt1.second.second,0,nbOfComponents,1,false);
                  return self;
                case 3:
                  self->setPartOfValues1(d1,pt1.first,pt1.second.first,pt1.second.second,0,nbOfComponents,1);
                  return self;
                case 4:
                  tmp=dd1->buildDAInt(1,self->getNumberOfComponents());
                  self->setPartOfValues1(tmp,pt1.first,pt1.second.first,pt1.second.second,0,nbOfComponents,1);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 4:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple3(i1,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),0,nbOfComponents,1);
                  return self;
                case 2:
                  tmp=DataArrayInt::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues3(tmp,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),0,nbOfComponents,1,false);
                  return self;
                case 3:
                  self->setPartOfValues3(d1,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),0,nbOfComponents,1);
                  return self;
                case 4:
                  tmp=dd1->buildDAInt(1,self->getNumberOfComponents());
                  self->setPartOfValues3(tmp,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),0,nbOfComponents,1);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 5:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple1(i1,it1,it1+1,1,ic1,ic1+1,1);
                  return self;
                case 2:
                  tmp=DataArrayInt::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues1(tmp,it1,it1+1,1,ic1,ic1+1,1,false);
                  return self;
                case 3:
                  self->setPartOfValues1(d1,it1,it1+1,1,ic1,ic1+1,1);
                  return self;
                case 4:
                  tmp=dd1->buildDAInt(1,self->getNumberOfComponents());
                  self->setPartOfValues1(tmp,it1,it1+1,1,ic1,ic1+1,1);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 6:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple3(i1,&vt1[0],&vt1[0]+vt1.size(),ic1,ic1+1,1);
                  return self;
                case 2:
                  tmp=DataArrayInt::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues3(tmp,&vt1[0],&vt1[0]+vt1.size(),ic1,ic1+1,1,false);
                  return self;
                case 3:
                  self->setPartOfValues3(d1,&vt1[0],&vt1[0]+vt1.size(),ic1,ic1+1,1);
                  return self;
                case 4:
                  tmp=dd1->buildDAInt(1,self->getNumberOfComponents());
                  self->setPartOfValues3(tmp,&vt1[0],&vt1[0]+vt1.size(),ic1,ic1+1,1);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 7:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple1(i1,pt1.first,pt1.second.first,pt1.second.second,ic1,ic1+1,1);
                  return self;
                case 2:
                  tmp=DataArrayInt::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues1(tmp,pt1.first,pt1.second.first,pt1.second.second,ic1,ic1+1,1,false);
                  return self;
                case 3:
                  self->setPartOfValues1(d1,pt1.first,pt1.second.first,pt1.second.second,ic1,ic1+1,1);
                  return self;
                case 4:
                  tmp=dd1->buildDAInt(1,self->getNumberOfComponents());
                  self->setPartOfValues1(tmp,pt1.first,pt1.second.first,pt1.second.second,ic1,ic1+1,1);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 8:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple3(i1,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),ic1,ic1+1,1);
                  return self;
                case 2:
                  tmp=DataArrayInt::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues3(tmp,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),ic1,ic1+1,1,false);
                  return self;
                case 3:
                  self->setPartOfValues3(d1,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),ic1,ic1+1,1);
                  return self;
                case 4:
                  tmp=dd1->buildDAInt(1,self->getNumberOfComponents());
                  self->setPartOfValues3(tmp,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),ic1,ic1+1,1);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 9:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple2(i1,&it1,&it1+1,&vc1[0],&vc1[0]+vc1.size());
                  return self;
                case 2:
                  tmp=DataArrayInt::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues2(tmp,&it1,&it1+1,&vc1[0],&vc1[0]+vc1.size(),false);
                  return self;
                case 3:
                  self->setPartOfValues2(d1,&it1,&it1+1,&vc1[0],&vc1[0]+vc1.size());
                  return self;
                case 4:
                  tmp=dd1->buildDAInt(1,self->getNumberOfComponents());
                  self->setPartOfValues2(tmp,&it1,&it1+1,&vc1[0],&vc1[0]+vc1.size());
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 10:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple2(i1,&vt1[0],&vt1[0]+vt1.size(),&vc1[0],&vc1[0]+vc1.size());
                  return self;
                case 2:
                  tmp=DataArrayInt::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues2(tmp,&vt1[0],&vt1[0]+vt1.size(),&vc1[0],&vc1[0]+vc1.size(),false);
                  return self;
                case 3:
                  self->setPartOfValues2(d1,&vt1[0],&vt1[0]+vt1.size(),&vc1[0],&vc1[0]+vc1.size());
                  return self;
                case 4:
                  tmp=dd1->buildDAInt(1,self->getNumberOfComponents());
                  self->setPartOfValues2(tmp,&vt1[0],&vt1[0]+vt1.size(),&vc1[0],&vc1[0]+vc1.size());
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 11:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple4(i1,pt1.first,pt1.second.first,pt1.second.second,&vc1[0],&vc1[0]+vc1.size());
                  return self;
                case 2:
                  tmp=DataArrayInt::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues4(tmp,pt1.first,pt1.second.first,pt1.second.second,&vc1[0],&vc1[0]+vc1.size(),false);
                  return self;
                case 3:
                  self->setPartOfValues4(d1,pt1.first,pt1.second.first,pt1.second.second,&vc1[0],&vc1[0]+vc1.size());
                  return self;
                case 4:
                  tmp=dd1->buildDAInt(1,self->getNumberOfComponents());
                  self->setPartOfValues4(tmp,pt1.first,pt1.second.first,pt1.second.second,&vc1[0],&vc1[0]+vc1.size());
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 12:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple2(i1,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),&vc1[0],&vc1[0]+vc1.size());
                  return self;
                case 2:
                  tmp=DataArrayInt::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues2(tmp,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),&vc1[0],&vc1[0]+vc1.size(),false);
                  return self;
                case 3:
                  self->setPartOfValues2(d1,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),&vc1[0],&vc1[0]+vc1.size());
                  return self;
                case 4:
                  tmp=dd1->buildDAInt(1,self->getNumberOfComponents());
                  self->setPartOfValues2(tmp,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),&vc1[0],&vc1[0]+vc1.size());
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 13:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple1(i1,it1,it1+1,1,pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                case 2:
                  tmp=DataArrayInt::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues1(tmp,it1,it1+1,1,pc1.first,pc1.second.first,pc1.second.second,false);
                  return self;
                case 3:
                  self->setPartOfValues1(d1,it1,it1+1,1,pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                case 4:
                  tmp=dd1->buildDAInt(1,self->getNumberOfComponents());
                  self->setPartOfValues1(tmp,it1,it1+1,1,pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 14:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple3(i1,&vt1[0],&vt1[0]+vt1.size(),pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                case 2:
                  tmp=DataArrayInt::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues3(tmp,&vt1[0],&vt1[0]+vt1.size(),pc1.first,pc1.second.first,pc1.second.second,false);
                  return self;
                case 3:
                  self->setPartOfValues3(d1,&vt1[0],&vt1[0]+vt1.size(),pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                case 4:
                  tmp=dd1->buildDAInt(1,self->getNumberOfComponents());
                  self->setPartOfValues3(tmp,&vt1[0],&vt1[0]+vt1.size(),pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 15:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple1(i1,pt1.first,pt1.second.first,pt1.second.second,pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                case 2:
                  tmp=DataArrayInt::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues1(tmp,pt1.first,pt1.second.first,pt1.second.second,pc1.first,pc1.second.first,pc1.second.second,false);
                  return self;
                case 3:
                  self->setPartOfValues1(d1,pt1.first,pt1.second.first,pt1.second.second,pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                case 4:
                  tmp=dd1->buildDAInt(1,self->getNumberOfComponents());
                  self->setPartOfValues1(tmp,pt1.first,pt1.second.first,pt1.second.second,pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          case 16:
            {
              switch(sw1)
                {
                case 1:
                  self->setPartOfValuesSimple3(i1,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                case 2:
                  tmp=DataArrayInt::New();
                  tmp->useArray(&v1[0],false,CPP_DEALLOC,1,v1.size());
                  self->setPartOfValues3(tmp,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),pc1.first,pc1.second.first,pc1.second.second,false);
                  return self;
                case 3:
                  self->setPartOfValues3(d1,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                case 4:
                  tmp=dd1->buildDAInt(1,self->getNumberOfComponents());
                  self->setPartOfValues3(tmp,dt1->getConstPointer(),dt1->getConstPointer()+dt1->getNbOfElems(),pc1.first,pc1.second.first,pc1.second.second);
                  return self;
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
              break;
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
        return self;
      }

      DataArrayInt *__neg__() const throw(INTERP_KERNEL::Exception)
      {
        return self->negate();
      }
 
      DataArrayInt *__add__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __add__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=self->deepCpy();
              ret->applyLin(1,val);
              return ret.retn();
            }
          case 2:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=DataArrayInt::New(); aaaa->useArray(&aa[0],false,CPP_DEALLOC,1,(int)aa.size());
              return DataArrayInt::Add(self,aaaa);
            }
          case 3:
            {
              return DataArrayInt::Add(self,a);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              return DataArrayInt::Add(self,aaaa);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayInt *__radd__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __radd__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=self->deepCpy();
              ret->applyLin(1,val);
              return ret.retn();
            }
          case 2:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=DataArrayInt::New(); aaaa->useArray(&aa[0],false,CPP_DEALLOC,1,(int)aa.size());
              return DataArrayInt::Add(self,aaaa);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              return DataArrayInt::Add(self,aaaa);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      PyObject *___iadd___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __iadd__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              self->applyLin(1,val);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 2:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> bb=DataArrayInt::New(); bb->useArray(&aa[0],false,CPP_DEALLOC,1,(int)aa.size());
              self->addEqual(bb);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 3:
            {
              self->addEqual(a);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              self->addEqual(aaaa);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayInt *__sub__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __sub__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=self->deepCpy();
              ret->applyLin(1,-val);
              return ret.retn();
            }
          case 2:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=DataArrayInt::New(); aaaa->useArray(&aa[0],false,CPP_DEALLOC,1,(int)aa.size());
              return DataArrayInt::Substract(self,aaaa);
            }
          case 3:
            {
              return DataArrayInt::Substract(self,a);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              return DataArrayInt::Substract(self,aaaa);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayInt *__rsub__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __rsub__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=self->deepCpy();
              ret->applyLin(-1,val);
              return ret.retn();
            }
          case 2:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=DataArrayInt::New(); aaaa->useArray(&aa[0],false,CPP_DEALLOC,1,(int)aa.size());
              return DataArrayInt::Substract(aaaa,self);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              return DataArrayInt::Substract(aaaa,self);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      PyObject *___isub___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __isub__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              self->applyLin(1,-val);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 2:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> bb=DataArrayInt::New(); bb->useArray(&aa[0],false,CPP_DEALLOC,1,(int)aa.size());
              self->substractEqual(bb);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 3:
            {
              self->substractEqual(a);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              self->substractEqual(aaaa);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayInt *__mul__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __mul__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=self->deepCpy();
              ret->applyLin(val,0);
              return ret.retn();
            }
          case 2:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=DataArrayInt::New(); aaaa->useArray(&aa[0],false,CPP_DEALLOC,1,(int)aa.size());
              return DataArrayInt::Multiply(self,aaaa);
            }
          case 3:
            {
              return DataArrayInt::Multiply(self,a);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              return DataArrayInt::Multiply(self,aaaa);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayInt *__rmul__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __rmul__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=self->deepCpy();
              ret->applyLin(val,0);
              return ret.retn();
            }
          case 2:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=DataArrayInt::New(); aaaa->useArray(&aa[0],false,CPP_DEALLOC,1,(int)aa.size());
              return DataArrayInt::Multiply(self,aaaa);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              return DataArrayInt::Multiply(self,aaaa);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      PyObject *___imul___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __imul__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              self->applyLin(val,0);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 2:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> bb=DataArrayInt::New(); bb->useArray(&aa[0],false,CPP_DEALLOC,1,(int)aa.size());
              self->multiplyEqual(bb);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 3:
            {
              self->multiplyEqual(a);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              self->multiplyEqual(aaaa);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayInt *__div__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __div__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=self->deepCpy();
              ret->applyDivideBy(val);
              return ret.retn();
            }
          case 2:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=DataArrayInt::New(); aaaa->useArray(&aa[0],false,CPP_DEALLOC,1,(int)aa.size());
              return DataArrayInt::Divide(self,aaaa);
            }
          case 3:
            {
              return DataArrayInt::Divide(self,a);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              return DataArrayInt::Divide(self,aaaa);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayInt *__rdiv__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __rdiv__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=self->deepCpy();
              ret->applyInv(val);
              return ret.retn();
            }
          case 2:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=DataArrayInt::New(); aaaa->useArray(&aa[0],false,CPP_DEALLOC,1,(int)aa.size());
              return DataArrayInt::Divide(aaaa,self);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              return DataArrayInt::Divide(aaaa,self);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      PyObject *___idiv___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __idiv__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              self->applyDivideBy(val);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 2:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> bb=DataArrayInt::New(); bb->useArray(&aa[0],false,CPP_DEALLOC,1,(int)aa.size());
              self->divideEqual(bb);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 3:
            {
              self->divideEqual(a);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              self->divideEqual(aaaa);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayInt *__mod__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __mod__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=self->deepCpy();
              ret->applyModulus(val);
              return ret.retn();
            }
          case 2:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=DataArrayInt::New(); aaaa->useArray(&aa[0],false,CPP_DEALLOC,1,(int)aa.size());
              return DataArrayInt::Modulus(self,aaaa);
            }
          case 3:
            {
              return DataArrayInt::Modulus(self,a);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              return DataArrayInt::Modulus(self,aaaa);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayInt *__rmod__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __rmod__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=self->deepCpy();
              ret->applyRModulus(val);
              return ret.retn();
            }
          case 2:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=DataArrayInt::New(); aaaa->useArray(&aa[0],false,CPP_DEALLOC,1,(int)aa.size());
              return DataArrayInt::Modulus(aaaa,self);
            }
          case 3:
            {
              return DataArrayInt::Modulus(a,self);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              return DataArrayInt::Modulus(aaaa,self);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      PyObject *___imod___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __imod__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              self->applyModulus(val);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 3:
            {
              self->modulusEqual(a);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              self->modulusEqual(aaaa);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayInt *__pow__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __pow__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=self->deepCpy();
              ret->applyPow(val);
              return ret.retn();
            }
          case 2:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=DataArrayInt::New(); aaaa->useArray(&aa[0],false,CPP_DEALLOC,1,(int)aa.size());
              return DataArrayInt::Pow(self,aaaa);
            }
          case 3:
            {
              return DataArrayInt::Pow(self,a);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              return DataArrayInt::Pow(self,aaaa);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      DataArrayInt *__rpow__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __rpow__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=self->deepCpy();
              ret->applyRPow(val);
              return ret.retn();
            }
          case 2:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=DataArrayInt::New(); aaaa->useArray(&aa[0],false,CPP_DEALLOC,1,(int)aa.size());
              return DataArrayInt::Pow(aaaa,self);
            }
          case 3:
            {
              return DataArrayInt::Pow(a,self);
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              return DataArrayInt::Pow(aaaa,self);
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }
   
      PyObject *___ipow___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="Unexpected situation in __ipow__ !";
        int val;
        DataArrayInt *a;
        std::vector<int> aa;
        DataArrayIntTuple *aaa;
        int sw;
        convertObjToPossibleCpp1(obj,sw,val,aa,a,aaa);
        switch(sw)
          {
          case 1:
            {
              self->applyPow(val);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 3:
            {
              self->powEqual(a);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          case 4:
            {
              MEDCouplingAutoRefCountObjectPtr<DataArrayInt> aaaa=aaa->buildDAInt(1,self->getNumberOfComponents());
              self->powEqual(aaaa);
              Py_XINCREF(trueSelf);
              return trueSelf;
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }

      std::string __repr__() const throw(INTERP_KERNEL::Exception)
      {
        std::ostringstream oss;
        self->reprQuickOverview(oss);
        return oss.str();
      }
      
      void pushBackValsSilent(PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        int szArr,sw,iTypppArr;
        std::vector<int> stdvecTyyppArr;
        const int *tmp=convertObjToPossibleCpp1_Safe(li,sw,szArr,iTypppArr,stdvecTyyppArr);
        self->pushBackValsSilent(tmp,tmp+szArr);
      }
      
      PyObject *partitionByDifferentValues() const throw(INTERP_KERNEL::Exception)
      {
        std::vector<int> ret1;
        std::vector<DataArrayInt *> ret0=self->partitionByDifferentValues(ret1);
        std::size_t sz=ret0.size();
        PyObject *pyRet=PyTuple_New(2);
        PyObject *pyRet0=PyList_New((int)sz);
        PyObject *pyRet1=PyList_New((int)sz);
        for(std::size_t i=0;i<sz;i++)
          {
            PyList_SetItem(pyRet0,i,SWIG_NewPointerObj(SWIG_as_voidptr(ret0[i]),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 ));
            PyList_SetItem(pyRet1,i,PyInt_FromLong(ret1[i]));
          }
        PyTuple_SetItem(pyRet,0,pyRet0);
        PyTuple_SetItem(pyRet,1,pyRet1);
        return pyRet;
      }
      
      PyObject *searchRangesInListOfIds(const DataArrayInt *listOfIds) const throw(INTERP_KERNEL::Exception)
      {
        DataArrayInt *ret0=0,*ret1=0;
        self->searchRangesInListOfIds(listOfIds,ret0,ret1);
        PyObject *pyRet=PyTuple_New(2);
        PyTuple_SetItem(pyRet,0,SWIG_NewPointerObj(SWIG_as_voidptr(ret0),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 ));
        PyTuple_SetItem(pyRet,1,SWIG_NewPointerObj(SWIG_as_voidptr(ret1),SWIGTYPE_p_ParaMEDMEM__DataArrayInt, SWIG_POINTER_OWN | 0 ));
        return pyRet;
      }
    }
  };

  class DataArrayIntTuple;

  class DataArrayIntIterator
  {
  public:
    DataArrayIntIterator(DataArrayInt *da);
    ~DataArrayIntIterator();
    %extend
    {
      PyObject *next()
      {
        DataArrayIntTuple *ret=self->nextt();
        if(ret)
          return SWIG_NewPointerObj(SWIG_as_voidptr(ret),SWIGTYPE_p_ParaMEDMEM__DataArrayIntTuple,SWIG_POINTER_OWN | 0);
        else
          {
            PyErr_SetString(PyExc_StopIteration,"No more data.");
            return 0;
          }
      }
    }
  };

  class DataArrayIntTuple
  {
  public:
    int getNumberOfCompo() const;
    DataArrayInt *buildDAInt(int nbOfTuples, int nbOfCompo) const throw(INTERP_KERNEL::Exception);
    %extend
    {
      std::string __str__() const throw(INTERP_KERNEL::Exception)
      {
        return self->repr();
      }

      int __int__() const throw(INTERP_KERNEL::Exception)
      {
        return self->intValue();
      }

      DataArrayInt *buildDAInt() throw(INTERP_KERNEL::Exception)
      {
        return self->buildDAInt(1,self->getNumberOfCompo());
      }

      PyObject *___iadd___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=self->buildDAInt(1,self->getNumberOfCompo());
        ParaMEDMEM_DataArrayInt____iadd___(ret,0,obj);
        Py_XINCREF(trueSelf);
        return trueSelf;
      }
  
      PyObject *___isub___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=self->buildDAInt(1,self->getNumberOfCompo());
        ParaMEDMEM_DataArrayInt____isub___(ret,0,obj);
        Py_XINCREF(trueSelf);
        return trueSelf;
      }
  
      PyObject *___imul___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=self->buildDAInt(1,self->getNumberOfCompo());
        ParaMEDMEM_DataArrayInt____imul___(ret,0,obj);
        Py_XINCREF(trueSelf);
        return trueSelf;
      }

      PyObject *___idiv___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=self->buildDAInt(1,self->getNumberOfCompo());
        ParaMEDMEM_DataArrayInt____idiv___(ret,0,obj);
        Py_XINCREF(trueSelf);
        return trueSelf;
      }

      PyObject *___imod___(PyObject *trueSelf, PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        MEDCouplingAutoRefCountObjectPtr<DataArrayInt> ret=self->buildDAInt(1,self->getNumberOfCompo());
        ParaMEDMEM_DataArrayInt____imod___(ret,0,obj);
        Py_XINCREF(trueSelf);
        return trueSelf;
      }
  
      PyObject *__getitem__(PyObject *obj) throw(INTERP_KERNEL::Exception)
      {
        const char msg2[]="DataArrayIntTuple::__getitem__ : Mismatch of slice values in 2nd parameter (components) !";
        int sw;
        int singleVal;
        std::vector<int> multiVal;
        std::pair<int, std::pair<int,int> > slic;
        ParaMEDMEM::DataArrayInt *daIntTyypp=0;
        const int *pt=self->getConstPointer();
        int nbc=self->getNumberOfCompo();
        convertObjToPossibleCpp2(obj,nbc,sw,singleVal,multiVal,slic,daIntTyypp);
        switch(sw)
          {
          case 1:
            {
              if(singleVal>=nbc)
                {
                  std::ostringstream oss;
                  oss << "Requesting for id " << singleVal << " having only " << nbc << " components !";
                  throw INTERP_KERNEL::Exception(oss.str().c_str());
                }
              if(singleVal>=0)
                return PyInt_FromLong(pt[singleVal]);
              else
                {
                  if(nbc+singleVal>0)
                    return PyInt_FromLong(pt[nbc+singleVal]);
                  else
                    {
                      std::ostringstream oss;
                      oss << "Requesting for id " << singleVal << " having only " << nbc << " components !";
                      throw INTERP_KERNEL::Exception(oss.str().c_str());
                    }
                }
            }
          case 2:
            {
              PyObject *t=PyTuple_New(multiVal.size());
              for(int j=0;j<(int)multiVal.size();j++)
                {
                  int cid=multiVal[j];
                  if(cid>=nbc)
                    {
                      std::ostringstream oss;
                      oss << "Requesting for id #" << cid << " having only " << nbc << " components !";
                      throw INTERP_KERNEL::Exception(oss.str().c_str());
                    }
                  PyTuple_SetItem(t,j,PyInt_FromLong(pt[cid]));
                }
              return t;
            }
          case 3:
            {
              int sz=DataArray::GetNumberOfItemGivenBES(slic.first,slic.second.first,slic.second.second,msg2);
              PyObject *t=PyTuple_New(sz);
              for(int j=0;j<sz;j++)
                PyTuple_SetItem(t,j,PyInt_FromLong(pt[slic.first+j*slic.second.second]));
              return t;
            }
          default:
            throw INTERP_KERNEL::Exception("DataArrayIntTuple::__getitem__ : unrecognized type entered !");
          }
      }

      DataArrayIntTuple *__setitem__(PyObject *obj, PyObject *value) throw(INTERP_KERNEL::Exception)
      {
        const char msg[]="DataArrayIntTuple::__setitem__ : unrecognized type entered, int, slice, list<int>, tuple<int> !";
        const char msg2[]="DataArrayIntTuple::__setitem__ : Mismatch of slice values in 2nd parameter (components) !";
        int sw1,sw2;
        int singleValV;
        std::vector<int> multiValV;
        std::pair<int, std::pair<int,int> > slicV;
        ParaMEDMEM::DataArrayIntTuple *daIntTyyppV=0;
        int nbc=self->getNumberOfCompo();
        convertObjToPossibleCpp22(value,nbc,sw1,singleValV,multiValV,slicV,daIntTyyppV);
        int singleVal;
        std::vector<int> multiVal;
        std::pair<int, std::pair<int,int> > slic;
        ParaMEDMEM::DataArrayInt *daIntTyypp=0;
        int *pt=self->getPointer();
        convertObjToPossibleCpp2(obj,nbc,sw2,singleVal,multiVal,slic,daIntTyypp);
        switch(sw2)
          {
          case 1:
            {
              if(singleVal>=nbc)
                {
                  std::ostringstream oss;
                  oss << "Requesting for setting id # " << singleVal << " having only " << nbc << " components !";
                  throw INTERP_KERNEL::Exception(oss.str().c_str());
                }
              switch(sw1)
                {
                case 1:
                  {
                    pt[singleVal]=singleValV;
                    return self;
                  }
                case 2:
                  {
                    if(multiValV.size()!=1)
                      {
                        std::ostringstream oss;
                        oss << "Requesting for setting id # " << singleVal << " with a list or tuple with size != 1 ! ";
                        throw INTERP_KERNEL::Exception(oss.str().c_str());
                      }
                    pt[singleVal]=multiValV[0];
                    return self;
                  }
                case 4:
                  {
                    pt[singleVal]=daIntTyyppV->getConstPointer()[0];
                    return self;
                  }
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
            }
          case 2:
            {
              switch(sw1)
                {
                case 1:
                  {
                    for(std::vector<int>::const_iterator it=multiVal.begin();it!=multiVal.end();it++)
                      {
                        if(*it>=nbc)
                          {
                            std::ostringstream oss;
                            oss << "Requesting for setting id # " << *it << " having only " << nbc << " components !";
                            throw INTERP_KERNEL::Exception(oss.str().c_str());
                          }
                        pt[*it]=singleValV;
                      }
                    return self;
                  }
                case 2:
                  {
                    if(multiVal.size()!=multiValV.size())
                      {
                        std::ostringstream oss;
                        oss << "Mismatch length of during assignment : " << multiValV.size() << " != " << multiVal.size() << " !";
                        throw INTERP_KERNEL::Exception(oss.str().c_str());
                      }
                    for(int i=0;i<(int)multiVal.size();i++)
                      {
                        int pos=multiVal[i];
                        if(pos>=nbc)
                          {
                            std::ostringstream oss;
                            oss << "Requesting for setting id # " << pos << " having only " << nbc << " components !";
                            throw INTERP_KERNEL::Exception(oss.str().c_str());
                          }
                        pt[multiVal[i]]=multiValV[i];
                      }
                    return self;
                  }
                case 4:
                  {
                    const int *ptV=daIntTyyppV->getConstPointer();
                    if(nbc>daIntTyyppV->getNumberOfCompo())
                      {
                        std::ostringstream oss;
                        oss << "Mismatch length of during assignment : " << nbc << " != " << daIntTyyppV->getNumberOfCompo() << " !";
                        throw INTERP_KERNEL::Exception(oss.str().c_str());
                      }
                    std::copy(ptV,ptV+nbc,pt);
                    return self;
                  }
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
            }
          case 3:
            {
              int sz=DataArray::GetNumberOfItemGivenBES(slic.first,slic.second.first,slic.second.second,msg2);
              switch(sw1)
                {
                case 1:
                  {
                    for(int j=0;j<sz;j++)
                      pt[slic.first+j*slic.second.second]=singleValV;
                    return self;
                  }
                case 2:
                  {
                    if(sz!=(int)multiValV.size())
                      {
                        std::ostringstream oss;
                        oss << "Mismatch length of during assignment : " << multiValV.size() << " != " << sz << " !";
                        throw INTERP_KERNEL::Exception(oss.str().c_str());
                      }
                    for(int j=0;j<sz;j++)
                      pt[slic.first+j*slic.second.second]=multiValV[j];
                    return self;
                  }
                case 4:
                  {
                    const int *ptV=daIntTyyppV->getConstPointer();
                    if(sz>daIntTyyppV->getNumberOfCompo())
                      {
                        std::ostringstream oss;
                        oss << "Mismatch length of during assignment : " << nbc << " != " << daIntTyyppV->getNumberOfCompo() << " !";
                        throw INTERP_KERNEL::Exception(oss.str().c_str());
                      }
                    for(int j=0;j<sz;j++)
                      pt[slic.first+j*slic.second.second]=ptV[j];
                    return self;
                  }
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }
    }
  };

  class DataArrayChar : public DataArray
  {
  public:
    virtual DataArrayChar *buildEmptySpecializedDAChar() const throw(INTERP_KERNEL::Exception) = 0;
    void checkAllocated() const throw(INTERP_KERNEL::Exception);
    int getHashCode() const throw(INTERP_KERNEL::Exception);
    bool empty() const throw(INTERP_KERNEL::Exception);
    void cpyFrom(const DataArrayChar& other) throw(INTERP_KERNEL::Exception);
    void reserve(std::size_t nbOfElems) throw(INTERP_KERNEL::Exception);
    void pushBackSilent(char val) throw(INTERP_KERNEL::Exception);
    void pushBackValsSilent(const char *valsBg, const char *valsEnd) throw(INTERP_KERNEL::Exception);
    char popBackSilent() throw(INTERP_KERNEL::Exception);
    void pack() const throw(INTERP_KERNEL::Exception);
    void allocIfNecessary(int nbOfTuple, int nbOfCompo) throw(INTERP_KERNEL::Exception);
    bool isEqual(const DataArrayChar& other) const throw(INTERP_KERNEL::Exception);
    bool isEqualWithoutConsideringStr(const DataArrayChar& other) const throw(INTERP_KERNEL::Exception);
    void reverse() throw(INTERP_KERNEL::Exception);
    void fillWithZero() throw(INTERP_KERNEL::Exception);
    void fillWithValue(char val) throw(INTERP_KERNEL::Exception);
    std::string repr() const throw(INTERP_KERNEL::Exception);
    std::string reprZip() const throw(INTERP_KERNEL::Exception);
    void reAlloc(int nbOfTuples) throw(INTERP_KERNEL::Exception);
    DataArrayInt *convertToIntArr() const throw(INTERP_KERNEL::Exception);
    DataArrayChar *renumber(const int *old2New) const throw(INTERP_KERNEL::Exception);
    DataArrayChar *renumberR(const int *new2Old) const throw(INTERP_KERNEL::Exception);
    DataArrayChar *renumberAndReduce(const int *old2NewBg, int newNbOfTuple) const throw(INTERP_KERNEL::Exception);
    DataArrayChar *selectByTupleIdSafe(const int *new2OldBg, const int *new2OldEnd) const throw(INTERP_KERNEL::Exception);
    DataArrayChar *selectByTupleId2(int bg, int end, int step) const throw(INTERP_KERNEL::Exception);
    bool isUniform(char val) const throw(INTERP_KERNEL::Exception);
    void rearrange(int newNbOfCompo) throw(INTERP_KERNEL::Exception);
    DataArrayChar *substr(int tupleIdBg, int tupleIdEnd=-1) const throw(INTERP_KERNEL::Exception);
    DataArrayChar *changeNbOfComponents(int newNbOfComp, char dftValue) const throw(INTERP_KERNEL::Exception);
    void meldWith(const DataArrayChar *other) throw(INTERP_KERNEL::Exception);
    void setPartOfValuesAdv(const DataArrayChar *a, const DataArrayChar *tuplesSelec) throw(INTERP_KERNEL::Exception);
    char back() const throw(INTERP_KERNEL::Exception);
    void setIJ(int tupleId, int compoId, char newVal);
    void setIJSilent(int tupleId, int compoId, char newVal);
    char *getPointer();
    DataArrayInt *getIdsEqual(char val) const throw(INTERP_KERNEL::Exception);
    DataArrayInt *getIdsNotEqual(char val) const throw(INTERP_KERNEL::Exception);
    int locateTuple(const std::vector<char>& tupl) const throw(INTERP_KERNEL::Exception);
    bool presenceOfTuple(const std::vector<char>& tupl) const throw(INTERP_KERNEL::Exception);
    char getMaxValue(int& tupleId) const throw(INTERP_KERNEL::Exception);
    char getMaxValueInArray() const throw(INTERP_KERNEL::Exception);
    char getMinValue(int& tupleId) const throw(INTERP_KERNEL::Exception);
    char getMinValueInArray() const throw(INTERP_KERNEL::Exception);
    DataArrayInt *getIdsInRange(char vmin, char vmax) const throw(INTERP_KERNEL::Exception);
    static DataArrayChar *Aggregate(const DataArrayChar *a1, const DataArrayChar *a2);
    static DataArrayChar *Aggregate(const std::vector<const DataArrayChar *>& arr) throw(INTERP_KERNEL::Exception);
    static DataArrayChar *Meld(const DataArrayChar *a1, const DataArrayChar *a2) throw(INTERP_KERNEL::Exception);
    static DataArrayChar *Meld(const std::vector<const DataArrayChar *>& arr) throw(INTERP_KERNEL::Exception);
    %extend
    {
      int __len__() const throw(INTERP_KERNEL::Exception)
      {
        if(self->isAllocated())
          {
            return self->getNumberOfTuples();
          }
        else
          {
            throw INTERP_KERNEL::Exception("DataArrayChar::__len__ : Instance is NOT allocated !");
          }
      }
      
      PyObject *isEqualIfNotWhy(const DataArrayChar& other) const throw(INTERP_KERNEL::Exception)
      {
        std::string ret1;
        bool ret0=self->isEqualIfNotWhy(other,ret1);
        PyObject *ret=PyTuple_New(2);
        PyObject *ret0Py=ret0?Py_True:Py_False;
        Py_XINCREF(ret0Py);
        PyTuple_SetItem(ret,0,ret0Py);
        PyTuple_SetItem(ret,1,PyString_FromString(ret1.c_str()));
        return ret;
      }
      
      DataArrayChar *renumber(PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumber(tmp);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            if(!da2)
              throw INTERP_KERNEL::Exception("Not null DataArrayInt instance expected !");
            da2->checkAllocated();
            int size=self->getNumberOfTuples();
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumber(da2->getConstPointer());
          }
      }
      
      DataArrayChar *renumberR(PyObject *li) throw(INTERP_KERNEL::Exception)
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumberR(tmp);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            if(!da2)
              throw INTERP_KERNEL::Exception("Not null DataArrayInt instance expected !");
            da2->checkAllocated();
            int size=self->getNumberOfTuples();
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumberR(da2->getConstPointer());
          }
      }
      
      DataArrayChar *renumberAndReduce(PyObject *li, int newNbOfTuple) throw(INTERP_KERNEL::Exception)
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumberAndReduce(tmp,newNbOfTuple);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            if(!da2)
              throw INTERP_KERNEL::Exception("Not null DataArrayInt instance expected !");
            da2->checkAllocated();
            int size=self->getNumberOfTuples();
            if(size!=self->getNumberOfTuples())
              {
                throw INTERP_KERNEL::Exception("Invalid list length ! Must be equal to number of tuples !");
              }
            return self->renumberAndReduce(da2->getConstPointer(),newNbOfTuple);
          }
      }
      
      DataArrayChar *selectByTupleIdSafe(PyObject *li) const throw(INTERP_KERNEL::Exception)
      {
        void *da=0;
        int res1=SWIG_ConvertPtr(li,&da,SWIGTYPE_p_ParaMEDMEM__DataArrayInt, 0 |  0 );
        if (!SWIG_IsOK(res1))
          {
            int size;
            INTERP_KERNEL::AutoPtr<int> tmp=convertPyToNewIntArr2(li,&size);
            return self->selectByTupleIdSafe(tmp,tmp+size);
          }
        else
          {
            DataArrayInt *da2=reinterpret_cast< DataArrayInt * >(da);
            if(!da2)
              throw INTERP_KERNEL::Exception("Not null DataArrayInt instance expected !");
            da2->checkAllocated();
            return self->selectByTupleIdSafe(da2->getConstPointer(),da2->getConstPointer()+da2->getNbOfElems());
          }
      }
      
      DataArrayChar *keepSelectedComponents(PyObject *li) const throw(INTERP_KERNEL::Exception)
      {
        std::vector<int> tmp;
        convertPyToNewIntArr3(li,tmp);
        return self->keepSelectedComponents(tmp);
      }
      
      static DataArrayChar *Aggregate(PyObject *dachs) throw(INTERP_KERNEL::Exception)
      {
        std::vector<const ParaMEDMEM::DataArrayChar *> tmp;
        convertFromPyObjVectorOfObj<const ParaMEDMEM::DataArrayChar *>(dachs,SWIGTYPE_p_ParaMEDMEM__DataArrayChar,"DataArrayChar",tmp);
        return DataArrayChar::Aggregate(tmp);
      }
      
      static DataArrayChar *Meld(PyObject *dachs) throw(INTERP_KERNEL::Exception)
      {
        std::vector<const ParaMEDMEM::DataArrayChar *> tmp;
        convertFromPyObjVectorOfObj<const ParaMEDMEM::DataArrayChar *>(dachs,SWIGTYPE_p_ParaMEDMEM__DataArrayChar,"DataArrayChar",tmp);
        return DataArrayChar::Meld(tmp);
      }
    }
  };
  
  class DataArrayByteIterator;

  class DataArrayByte : public DataArrayChar
  {
  public:
    static DataArrayByte *New();
    DataArrayChar *buildEmptySpecializedDAChar() const throw(INTERP_KERNEL::Exception);
    DataArrayByteIterator *iterator();
    DataArrayByte *deepCpy() const;
    DataArrayByte *performCpy(bool deepCpy) const;
    char byteValue() const throw(INTERP_KERNEL::Exception);
    %extend
    {
      DataArrayByte() throw(INTERP_KERNEL::Exception)
        {
          return DataArrayByte::New();
        }

      static DataArrayByte *New(PyObject *elt0, PyObject *nbOfTuples=0, PyObject *nbOfComp=0) throw(INTERP_KERNEL::Exception)
      {
        const char *msg="ParaMEDMEM::DataArrayByte::New : Available API are : \n-DataArrayByte.New()\n--DataArrayByte.New([1,3,4])\n-DataArrayByte.New([1,3,4],3)\n-DataArrayByte.New([1,3,4,5],2,2)\n-DataArrayByte.New(5)\n-DataArrayByte.New(5,2) !";
        if(PyList_Check(elt0) || PyTuple_Check(elt0))
          {
            if(nbOfTuples)
              {
                if(PyInt_Check(nbOfTuples))
                  {
                    int nbOfTuples1=PyInt_AS_LONG(nbOfTuples);
                    if(nbOfTuples1<0)
                      throw INTERP_KERNEL::Exception("DataArrayByte::New : should be a positive set of allocated memory !");
                    if(nbOfComp)
                      {
                        if(PyInt_Check(nbOfComp))
                          {//DataArrayByte.New([1,3,4,5],2,2)
                            int nbOfCompo=PyInt_AS_LONG(nbOfComp);
                            if(nbOfCompo<0)
                              throw INTERP_KERNEL::Exception("DataArrayByte::New : should be a positive number of components !");
                            MEDCouplingAutoRefCountObjectPtr<DataArrayByte> ret=DataArrayByte::New();
                            std::vector<int> tmp=fillArrayWithPyListInt2(elt0,nbOfTuples1,nbOfCompo);
                            ret->alloc(nbOfTuples1,nbOfCompo); std::copy(tmp.begin(),tmp.end(),ret->getPointer());
                            return ret.retn();
                          }
                        else
                          throw INTERP_KERNEL::Exception(msg);
                      }
                    else
                      {//DataArrayByte.New([1,3,4],3)
                        MEDCouplingAutoRefCountObjectPtr<DataArrayByte> ret=DataArrayByte::New();
                        int tmpp1=-1;
                        std::vector<int> tmp=fillArrayWithPyListInt2(elt0,nbOfTuples1,tmpp1);
                        ret->alloc(nbOfTuples1,tmpp1); std::copy(tmp.begin(),tmp.end(),ret->getPointer());
                        return ret.retn();
                      }
                  }
                else
                  throw INTERP_KERNEL::Exception(msg);
              }
            else
              {// DataArrayByte.New([1,3,4])
                MEDCouplingAutoRefCountObjectPtr<DataArrayByte> ret=DataArrayByte::New();
                int tmpp1=-1,tmpp2=-1;
                std::vector<int> tmp=fillArrayWithPyListInt2(elt0,tmpp1,tmpp2);
                ret->alloc(tmpp1,tmpp2); std::copy(tmp.begin(),tmp.end(),ret->getPointer());
                return ret.retn();
              }
          }
        else if(PyInt_Check(elt0))
          {
            int nbOfTuples1=PyInt_AS_LONG(elt0);
            if(nbOfTuples1<0)
              throw INTERP_KERNEL::Exception("DataArrayByte::New : should be a positive set of allocated memory !");
            if(nbOfTuples)
              {
                if(!nbOfComp)
                  {
                    if(PyInt_Check(nbOfTuples))
                      {//DataArrayByte.New(5,2)
                        int nbOfCompo=PyInt_AS_LONG(nbOfTuples);
                        if(nbOfCompo<0)
                          throw INTERP_KERNEL::Exception("DataArrayByte::New : should be a positive number of components !");
                        MEDCouplingAutoRefCountObjectPtr<DataArrayByte> ret=DataArrayByte::New();
                        ret->alloc(nbOfTuples1,nbOfCompo);
                        return ret.retn();
                      }
                    else
                      throw INTERP_KERNEL::Exception(msg);
                  }
                else
                  throw INTERP_KERNEL::Exception(msg);
              }
            else
              {//DataArrayByte.New(5)
                MEDCouplingAutoRefCountObjectPtr<DataArrayByte> ret=DataArrayByte::New();
                ret->alloc(nbOfTuples1,1);
                return ret.retn();
              }
          }
        else
          throw INTERP_KERNEL::Exception(msg);
      }

      DataArrayByte(PyObject *elt0, PyObject *nbOfTuples=0, PyObject *nbOfComp=0) throw(INTERP_KERNEL::Exception)
        {
          return ParaMEDMEM_DataArrayByte_New__SWIG_1(elt0,nbOfTuples,nbOfComp);
        }
   
      std::string __repr__() const throw(INTERP_KERNEL::Exception)
      {
        std::ostringstream oss;
        self->reprQuickOverview(oss);
        return oss.str();
      }
  
      int __int__() const throw(INTERP_KERNEL::Exception)
      {
        return (int) self->byteValue();
      }

      DataArrayByteIterator *__iter__() throw(INTERP_KERNEL::Exception)
      {
        return self->iterator();
      }

      int getIJ(int tupleId, int compoId) const
      {
        return (int)self->getIJ(tupleId,compoId);
      }
      
      int getIJSafe(int tupleId, int compoId) const throw(INTERP_KERNEL::Exception)
      {
        return (int)self->getIJSafe(tupleId,compoId);
      }

      std::string __str__() const throw(INTERP_KERNEL::Exception)
      {
        return self->repr();
      }

      PyObject *toStrList() const throw(INTERP_KERNEL::Exception)
      {
        const char *vals=self->getConstPointer();
        int nbOfComp=self->getNumberOfComponents();
        int nbOfTuples=self->getNumberOfTuples();
        return convertCharArrToPyListOfTuple(vals,nbOfComp,nbOfTuples);
      }
   
      bool presenceOfTuple(PyObject *tupl) const throw(INTERP_KERNEL::Exception)
      {
        int sz=-1,sw=-1;
        int ival=-1; std::vector<int> ivval;
        const int *pt=convertObjToPossibleCpp1_Safe(tupl,sw,sz,ival,ivval);
        std::vector<char> vals(sz);
        std::copy(pt,pt+sz,vals.begin());
        return self->presenceOfTuple(vals);
      }

      bool presenceOfValue(PyObject *vals) const throw(INTERP_KERNEL::Exception)
      {
        int sz=-1,sw=-1;
        int ival=-1; std::vector<int> ivval;
        const int *pt=convertObjToPossibleCpp1_Safe(vals,sw,sz,ival,ivval);
        std::vector<char> vals2(sz);
        std::copy(pt,pt+sz,vals2.begin());
        return self->presenceOfValue(vals2);
      }

      int locateValue(PyObject *vals) const throw(INTERP_KERNEL::Exception)
      {
        int sz=-1,sw=-1;
        int ival=-1; std::vector<int> ivval;
        const int *pt=convertObjToPossibleCpp1_Safe(vals,sw,sz,ival,ivval);
        std::vector<char> vals2(sz);
        std::copy(pt,pt+sz,vals2.begin());
        return self->locateValue(vals2);
      }

      int locateTuple(PyObject *tupl) const throw(INTERP_KERNEL::Exception)
      {
        int sz=-1,sw=-1;
        int ival=-1; std::vector<int> ivval;
        const int *pt=convertObjToPossibleCpp1_Safe(tupl,sw,sz,ival,ivval);
        std::vector<char> vals(sz);
        std::copy(pt,pt+sz,vals.begin());
        return self->locateTuple(vals);
      }

      int search(PyObject *strOrListOfInt) const throw(INTERP_KERNEL::Exception)
      {
        int sz=-1,sw=-1;
        int ival=-1; std::vector<int> ivval;
        const int *pt=convertObjToPossibleCpp1_Safe(strOrListOfInt,sw,sz,ival,ivval);
        std::vector<char> vals(sz);
        std::copy(pt,pt+sz,vals.begin());
        return self->search(vals);
      }

      PyObject *getTuple(int tupleId) throw(INTERP_KERNEL::Exception)
      {
        int sz=self->getNumberOfComponents();
        INTERP_KERNEL::AutoPtr<char> tmp=new char[sz];
        self->getTuple(tupleId,tmp);
        PyObject *ret=PyTuple_New(sz);
        for(int i=0;i<sz;i++) PyTuple_SetItem(ret,i,PyInt_FromLong((int)tmp[i]));
        return ret;
      }

      PyObject *getMaxValue() const throw(INTERP_KERNEL::Exception)
      {
        int tmp;
        int r1=(int)self->getMaxValue(tmp);
        PyObject *ret=PyTuple_New(2);
        PyTuple_SetItem(ret,0,PyInt_FromLong(r1));
        PyTuple_SetItem(ret,1,PyInt_FromLong(tmp));
        return ret;
      }

      PyObject *getMinValue() const throw(INTERP_KERNEL::Exception)
      {
        int tmp;
        int r1=(int)self->getMinValue(tmp);
        PyObject *ret=PyTuple_New(2);
        PyTuple_SetItem(ret,0,PyInt_FromLong(r1));
        PyTuple_SetItem(ret,1,PyInt_FromLong(tmp));
        return ret;
      }

      int index(PyObject *obj) const throw(INTERP_KERNEL::Exception)
      {
        int nbOfCompo=self->getNumberOfComponents();
        switch(nbOfCompo)
          {
          case 1:
            {
              if(PyInt_Check(obj))
                {
                  int val=(int)PyInt_AS_LONG(obj);
                  return self->locateValue(val);
                }
              else
                throw INTERP_KERNEL::Exception("DataArrayByte::index : 'this' contains one component and trying to find an element which is not an integer !");
            }
          default:
            return ParaMEDMEM_DataArrayByte_locateTuple(self,obj);
          }
      }

      bool __contains__(PyObject *obj) const throw(INTERP_KERNEL::Exception)
      {
        int nbOfCompo=self->getNumberOfComponents();
        switch(nbOfCompo)
          {
          case 0:
            return false;
          case 1:
            {
              if(PyInt_Check(obj))
                {
                  int val=(int)PyInt_AS_LONG(obj);
                  return self->presenceOfValue(val);
                }
              else
                throw INTERP_KERNEL::Exception("DataArrayByte::__contains__ : 'this' contains one component and trying to find an element which is not an integer !");
            }
          default:
            return ParaMEDMEM_DataArrayByte_presenceOfTuple(self,obj);
          }
      }
    }
  };

  class DataArrayByteTuple;

  class DataArrayByteIterator
  {
  public:
    DataArrayByteIterator(DataArrayByte *da);
    ~DataArrayByteIterator();
  };

  class DataArrayByteTuple
  {
  public:
    std::string repr() const throw(INTERP_KERNEL::Exception);
    DataArrayByte *buildDAByte(int nbOfTuples, int nbOfCompo) const throw(INTERP_KERNEL::Exception);
    %extend
    {
      std::string __str__() const
      {
        return self->repr();
      }
      
      char __int__() const throw(INTERP_KERNEL::Exception)
      {
        return self->byteValue();
      }
      
      DataArrayByte *buildDAByte()
      {
        return self->buildDAByte(1,self->getNumberOfCompo());
      }
    }
  };
  
  class DataArrayAsciiCharIterator;
  
  class DataArrayAsciiChar : public DataArrayChar
  {
  public:
    static DataArrayAsciiChar *New();
    DataArrayChar *buildEmptySpecializedDAChar() const throw(INTERP_KERNEL::Exception);
    DataArrayAsciiCharIterator *iterator();
    DataArrayAsciiChar *deepCpy() const;
    DataArrayAsciiChar *performCpy(bool deepCpy) const;
    char asciiCharValue() const throw(INTERP_KERNEL::Exception);
    %extend
    {
      DataArrayAsciiChar() throw(INTERP_KERNEL::Exception)
        {
          return DataArrayAsciiChar::New();
        }

      static DataArrayAsciiChar *New(PyObject *elt0, PyObject *nbOfTuples=0, PyObject *nbOfComp=0) throw(INTERP_KERNEL::Exception)
      {
        const char *msg="ParaMEDMEM::DataArrayAsciiChar::New : Available API are : \n-DataArrayAsciiChar.New()\n-DataArrayAsciiChar.New([1,3,4])\n-DataArrayAsciiChar.New([\"abc\",\"de\",\"fghi\"])\n-DataArrayAsciiChar.New([\"abc\",\"de\",\"fghi\"],\"t\")\n-DataArrayAsciiChar.New([1,3,4],3)\n-DataArrayAsciiChar.New([1,3,4,5],2,2)\n-DataArrayAsciiChar.New(5)\n-DataArrayAsciiChar.New(5,2) !";
        if(PyList_Check(elt0) || PyTuple_Check(elt0))
          {
            if(nbOfTuples)
              {
                if(PyInt_Check(nbOfTuples))
                  {
                    int nbOfTuples1=PyInt_AS_LONG(nbOfTuples);
                    if(nbOfTuples1<0)
                      throw INTERP_KERNEL::Exception("DataArrayAsciiChar::New : should be a positive set of allocated memory !");
                    if(nbOfComp)
                      {
                        if(PyInt_Check(nbOfComp))
                          {//DataArrayAsciiChar.New([1,3,4,5],2,2)
                            int nbOfCompo=PyInt_AS_LONG(nbOfComp);
                            if(nbOfCompo<0)
                              throw INTERP_KERNEL::Exception("DataArrayAsciiChar::New : should be a positive number of components !");
                            MEDCouplingAutoRefCountObjectPtr<DataArrayAsciiChar> ret=DataArrayAsciiChar::New();
                            std::vector<int> tmp=fillArrayWithPyListInt2(elt0,nbOfTuples1,nbOfCompo);
                            ret->alloc(nbOfTuples1,nbOfCompo); std::copy(tmp.begin(),tmp.end(),ret->getPointer());
                            return ret.retn();
                          }
                        else
                          throw INTERP_KERNEL::Exception(msg);
                      }
                    else
                      {//DataArrayAsciiChar.New([1,3,4],3)
                        MEDCouplingAutoRefCountObjectPtr<DataArrayAsciiChar> ret=DataArrayAsciiChar::New();
                        int tmpp1=-1;
                        std::vector<int> tmp=fillArrayWithPyListInt2(elt0,nbOfTuples1,tmpp1);
                        ret->alloc(nbOfTuples1,tmpp1); std::copy(tmp.begin(),tmp.end(),ret->getPointer());
                        return ret.retn();
                      }
                  }
                else if(PyString_Check(nbOfTuples))
                  {
                    if(PyString_Size(nbOfTuples)!=1)
                      throw INTERP_KERNEL::Exception(msg);
                    //DataArrayAsciiChar.New(["abc","de","fghi"],"t")
                    std::vector<std::string> tmp;
                    if(fillStringVector(elt0,tmp))
                      return DataArrayAsciiChar::New(tmp,PyString_AsString(nbOfTuples)[0]);
                    else
                      throw INTERP_KERNEL::Exception(msg);
                  }
                else
                  throw INTERP_KERNEL::Exception(msg);
              }
            else
              {
                std::vector<std::string> tmmp;
                if(fillStringVector(elt0,tmmp))
                  //DataArrayAsciiChar.New(["abc","de","fghi"])
                  return DataArrayAsciiChar::New(tmmp,' ');
                else
                  {
                    // DataArrayAsciiChar.New([1,3,4])
                    MEDCouplingAutoRefCountObjectPtr<DataArrayAsciiChar> ret=DataArrayAsciiChar::New();
                    int tmpp1=-1,tmpp2=-1;
                    std::vector<int> tmp=fillArrayWithPyListInt2(elt0,tmpp1,tmpp2);
                    ret->alloc(tmpp1,tmpp2); std::copy(tmp.begin(),tmp.end(),ret->getPointer());
                    return ret.retn();
                  }
              }
          }
        else if(PyInt_Check(elt0))
          {
            int nbOfTuples1=PyInt_AS_LONG(elt0);
            if(nbOfTuples1<0)
              throw INTERP_KERNEL::Exception("DataArrayAsciiChar::New : should be a positive set of allocated memory !");
            if(nbOfTuples)
              {
                if(!nbOfComp)
                  {
                    if(PyInt_Check(nbOfTuples))
                      {//DataArrayAsciiChar.New(5,2)
                        int nbOfCompo=PyInt_AS_LONG(nbOfTuples);
                        if(nbOfCompo<0)
                          throw INTERP_KERNEL::Exception("DataArrayAsciiChar::New : should be a positive number of components !");
                        MEDCouplingAutoRefCountObjectPtr<DataArrayAsciiChar> ret=DataArrayAsciiChar::New();
                        ret->alloc(nbOfTuples1,nbOfCompo);
                        return ret.retn();
                      }
                    else
                      throw INTERP_KERNEL::Exception(msg);
                  }
                else
                  throw INTERP_KERNEL::Exception(msg);
              }
            else
              {//DataArrayAsciiChar.New(5)
                MEDCouplingAutoRefCountObjectPtr<DataArrayAsciiChar> ret=DataArrayAsciiChar::New();
                ret->alloc(nbOfTuples1,1);
                return ret.retn();
              }
          }
        else
          throw INTERP_KERNEL::Exception(msg);
      }

      DataArrayAsciiChar(PyObject *elt0, PyObject *nbOfTuples=0, PyObject *nbOfComp=0) throw(INTERP_KERNEL::Exception)
        {
          return ParaMEDMEM_DataArrayAsciiChar_New__SWIG_1(elt0,nbOfTuples,nbOfComp);
        }

      std::string __repr__() const throw(INTERP_KERNEL::Exception)
      {
        std::ostringstream oss;
        self->reprQuickOverview(oss);
        return oss.str();
      }

      DataArrayAsciiCharIterator *__iter__() throw(INTERP_KERNEL::Exception)
      {
        return self->iterator();
      }

      std::string getIJ(int tupleId, int compoId) const throw(INTERP_KERNEL::Exception)
      {
        char tmp[2]; tmp[1]='\0';
        tmp[0]=self->getIJ(tupleId,compoId);
        return std::string(tmp);
      }
   
      std::string getIJSafe(int tupleId, int compoId) const throw(INTERP_KERNEL::Exception)
      {
        char tmp[2]; tmp[1]='\0';
        tmp[0]=self->getIJSafe(tupleId,compoId);
        return std::string(tmp);
      }

      std::string __str__() const throw(INTERP_KERNEL::Exception)
      {
        return self->repr();
      }

      PyObject *toStrList() const throw(INTERP_KERNEL::Exception)
      {
        const char *vals=self->getConstPointer();
        int nbOfComp=self->getNumberOfComponents();
        int nbOfTuples=self->getNumberOfTuples();
        return convertCharArrToPyListOfTuple(vals,nbOfComp,nbOfTuples);
      }

      bool presenceOfTuple(PyObject *tupl) const throw(INTERP_KERNEL::Exception)
      {
        if(PyString_Check(tupl))
          {
            Py_ssize_t sz=PyString_Size(tupl);
            std::vector<char> vals(sz);
            std::copy(PyString_AsString(tupl),PyString_AsString(tupl)+sz,vals.begin());
            return self->presenceOfTuple(vals);
          }
        else
          throw INTERP_KERNEL::Exception("DataArrayAsciiChar::presenceOfTuple : only strings in input supported !");
      }
   
      bool presenceOfValue(PyObject *vals) const throw(INTERP_KERNEL::Exception)
      {
        if(PyString_Check(vals))
          {
            Py_ssize_t sz=PyString_Size(vals);
            std::vector<char> vals2(sz);
            std::copy(PyString_AsString(vals),PyString_AsString(vals)+sz,vals2.begin());
            return self->presenceOfValue(vals2);
          }
        else
          throw INTERP_KERNEL::Exception("DataArrayAsciiChar::presenceOfValue : only strings in input supported !");
      }

      int locateValue(PyObject *vals) const throw(INTERP_KERNEL::Exception)
      {
        if(PyString_Check(vals))
          {
            Py_ssize_t sz=PyString_Size(vals);
            std::vector<char> vals2(sz);
            std::copy(PyString_AsString(vals),PyString_AsString(vals)+sz,vals2.begin());
            return self->locateValue(vals2);
          }
        else
          throw INTERP_KERNEL::Exception("DataArrayAsciiChar::locateValue : only strings in input supported !");
      }

      int locateTuple(PyObject *tupl) const throw(INTERP_KERNEL::Exception)
      {
        if(PyString_Check(tupl))
          {
            Py_ssize_t sz=PyString_Size(tupl);
            std::vector<char> vals(sz);
            std::copy(PyString_AsString(tupl),PyString_AsString(tupl)+sz,vals.begin());
            return self->locateTuple(vals);
          }
        else
          throw INTERP_KERNEL::Exception("DataArrayAsciiChar::locateTuple : only strings in input supported !");
      }

      int search(PyObject *strOrListOfInt) const throw(INTERP_KERNEL::Exception)
      {
        if(PyString_Check(strOrListOfInt))
          {
            Py_ssize_t sz=PyString_Size(strOrListOfInt);
            std::vector<char> vals(sz);
            std::copy(PyString_AsString(strOrListOfInt),PyString_AsString(strOrListOfInt)+sz,vals.begin());
            return self->search(vals);
          }
        else
          throw INTERP_KERNEL::Exception("DataArrayAsciiChar::search : only strings in input supported !");
      }
   
      PyObject *getTuple(int tupleId) const throw(INTERP_KERNEL::Exception)
      {
        int sz=self->getNumberOfComponents();
        INTERP_KERNEL::AutoPtr<char> tmp=new char[sz+1]; tmp[sz]='\0';
        self->getTuple(tupleId,tmp);
        return PyString_FromString(tmp);
      }

      PyObject *getMaxValue() const throw(INTERP_KERNEL::Exception)
      {
        int tmp;
        char tmp2[2]; tmp2[1]='\0';
        tmp2[0]=self->getMaxValue(tmp);
        PyObject *ret=PyTuple_New(2);
        PyTuple_SetItem(ret,0,PyString_FromString(tmp2));
        PyTuple_SetItem(ret,1,PyInt_FromLong(tmp));
        return ret;
      }

      PyObject *getMinValue() const throw(INTERP_KERNEL::Exception)
      {
        int tmp;
        char tmp2[2]; tmp2[1]='\0';
        tmp2[0]=self->getMinValue(tmp);
        PyObject *ret=PyTuple_New(2);
        PyTuple_SetItem(ret,0,PyString_FromString(tmp2));
        PyTuple_SetItem(ret,1,PyInt_FromLong(tmp));
        return ret;
      }

      int index(PyObject *obj) const throw(INTERP_KERNEL::Exception)
      {
        int nbOfCompo=self->getNumberOfComponents();
        switch(nbOfCompo)
          {
          case 1:
            {
              if(PyString_Check(obj))
                {
                  Py_ssize_t sz=PyString_Size(obj);
                  char *pt=PyString_AsString(obj);
                  if(sz==1)
                    return self->locateValue(pt[0]);
                  else
                    throw INTERP_KERNEL::Exception("DataArrayAsciiChar::index : 'this' contains one component and trying to find a string with size different from 1 !");
                }
              else
                throw INTERP_KERNEL::Exception("DataArrayAsciiChar::index : 'this' contains one component and trying to find an element which is not an integer !");
            }
          default:
            return ParaMEDMEM_DataArrayAsciiChar_locateTuple(self,obj);
          }
      }

      bool __contains__(PyObject *obj) const throw(INTERP_KERNEL::Exception)
      {
        int nbOfCompo=self->getNumberOfComponents();
        switch(nbOfCompo)
          {
          case 0:
            return false;
          case 1:
            {
              if(PyString_Check(obj))
                {
                  Py_ssize_t sz=PyString_Size(obj);
                  char *pt=PyString_AsString(obj);
                  if(sz==1)
                    return self->presenceOfValue(pt[0]);
                  else
                    throw INTERP_KERNEL::Exception("DataArrayAsciiChar::__contains__ : 'this' contains one component and trying to find a string with size different from 1 !");
                }
              else
                throw INTERP_KERNEL::Exception("DataArrayAsciiChar::__contains__ : 'this' contains one component and trying to find an element which is not an integer !");
            }
          default:
            return ParaMEDMEM_DataArrayAsciiChar_presenceOfTuple(self,obj);
          }
      }

      PyObject *__getitem__(PyObject *obj) const throw(INTERP_KERNEL::Exception)
      {
        int sw,iTypppArr;
        std::vector<int> stdvecTyyppArr;
        std::pair<int, std::pair<int,int> > sTyyppArr;
        ParaMEDMEM::DataArrayInt *daIntTyypp=0;
        convertObjToPossibleCpp2(obj,self->getNumberOfTuples(),sw,iTypppArr,stdvecTyyppArr,sTyyppArr,daIntTyypp);
        switch(sw)
          {
          case 1:
            return ParaMEDMEM_DataArrayAsciiChar_getTuple(self,iTypppArr);
          case 2:
            return convertDataArrayChar(self->selectByTupleIdSafe(&stdvecTyyppArr[0],&stdvecTyyppArr[0]+stdvecTyyppArr.size()), SWIG_POINTER_OWN | 0 );
          case 3:
            return convertDataArrayChar(self->selectByTupleId2(sTyyppArr.first,sTyyppArr.second.first,sTyyppArr.second.second), SWIG_POINTER_OWN | 0 );
          case 4:
            return convertDataArrayChar(self->selectByTupleIdSafe(daIntTyypp->begin(),daIntTyypp->end()), SWIG_POINTER_OWN | 0 );
          default:
            throw INTERP_KERNEL::Exception("DataArrayAsciiChar::__getitem__ : supporting int, list of int, tuple of int, DataArrayInt and slice in input !");
          }
      }

      DataArrayAsciiChar *__setitem__(PyObject *obj, PyObject *value) throw(INTERP_KERNEL::Exception)
      {
        static const char msg[]="DataArrayAsciiChar::__setitem__ : supporting int, list of int, tuple of int, DataArrayInt and slice in input, and 4 types accepted in value : string, list or tuple of strings having same size, not null DataArrayChar instance.";
        int sw1,iTypppArr;
        std::vector<int> stdvecTyyppArr;
        std::pair<int, std::pair<int,int> > sTyyppArr;
        ParaMEDMEM::DataArrayInt *daIntTyypp=0;
        int nbOfCompo=self->getNumberOfComponents();
        int nbOfTuples=self->getNumberOfTuples();
        convertObjToPossibleCpp2(obj,nbOfTuples,sw1,iTypppArr,stdvecTyyppArr,sTyyppArr,daIntTyypp);
        int sw2;
        char vc; std::string sc; std::vector<std::string> vsc; DataArrayChar *dacc=0;
        convertObjToPossibleCpp6(value,sw2,vc,sc,vsc,dacc);
        switch(sw1)
          {
          case 1:
            {//obj int
              switch(sw2)
                {//value char
                case 1:
                  {
                    self->setPartOfValuesSimple3(vc,&iTypppArr,&iTypppArr+1,0,nbOfCompo,1);
                    return self;
                  }
                  //value string
                case 2:
                  {
                    MEDCouplingAutoRefCountObjectPtr<DataArrayAsciiChar> tmp=DataArrayAsciiChar::New(sc);
                    self->setPartOfValues3(tmp,&iTypppArr,&iTypppArr+1,0,nbOfCompo,1,false);
                    return self;
                  }
                  //value vector<string>
                case 3:
                  {
                    MEDCouplingAutoRefCountObjectPtr<DataArrayAsciiChar> tmp=DataArrayAsciiChar::New(vsc,' ');
                    self->setPartOfValues3(tmp,&iTypppArr,&iTypppArr+1,0,nbOfCompo,1,false);
                    return self;
                  }
                  //value DataArrayChar
                case 4:
                  {
                    self->setPartOfValues3(dacc,&iTypppArr,&iTypppArr+1,0,nbOfCompo,1,false);
                    return self;
                  }
                default:
                  throw INTERP_KERNEL::Exception(msg);
                }
            }
          case 2:
            {//obj list-tuple[int]
              switch(sw2)
                {
                  {//value char
                  case 1:
                    {
                      self->setPartOfValuesSimple3(vc,&stdvecTyyppArr[0],&stdvecTyyppArr[0]+stdvecTyyppArr.size(),0,nbOfCompo,1);
                      return self;
                    }
                    //value string
                  case 2:
                    {
                      MEDCouplingAutoRefCountObjectPtr<DataArrayAsciiChar> tmp=DataArrayAsciiChar::New(sc);
                      self->setPartOfValues3(tmp,&stdvecTyyppArr[0],&stdvecTyyppArr[0]+stdvecTyyppArr.size(),0,nbOfCompo,1,false);
                      return self;
                    }
                    //value vector<string>
                  case 3:
                    {
                      MEDCouplingAutoRefCountObjectPtr<DataArrayAsciiChar> tmp=DataArrayAsciiChar::New(vsc,' ');
                      self->setPartOfValues3(tmp,&stdvecTyyppArr[0],&stdvecTyyppArr[0]+stdvecTyyppArr.size(),0,nbOfCompo,1,false);
                      return self;
                    }
                    //value DataArrayChar
                  case 4:
                    {
                      self->setPartOfValues3(dacc,&stdvecTyyppArr[0],&stdvecTyyppArr[0]+stdvecTyyppArr.size(),0,nbOfCompo,1,false);
                      return self;
                    }
                  default:
                    throw INTERP_KERNEL::Exception(msg);
                  }
                }
            }
          case 3:
            {//slice
              switch(sw2)
                {
                  {//value char
                  case 1:
                    {
                      self->setPartOfValuesSimple1(vc,sTyyppArr.first,sTyyppArr.second.first,sTyyppArr.second.second,0,nbOfCompo,1);
                      return self;
                    }
                    //value string
                  case 2:
                    {
                      MEDCouplingAutoRefCountObjectPtr<DataArrayAsciiChar> tmp=DataArrayAsciiChar::New(sc);
                      self->setPartOfValues1(tmp,sTyyppArr.first,sTyyppArr.second.first,sTyyppArr.second.second,0,nbOfCompo,1,false);
                      return self;
                    }
                    //value vector<string>
                  case 3:
                    {
                      MEDCouplingAutoRefCountObjectPtr<DataArrayAsciiChar> tmp=DataArrayAsciiChar::New(vsc,' ');
                      self->setPartOfValues1(tmp,sTyyppArr.first,sTyyppArr.second.first,sTyyppArr.second.second,0,nbOfCompo,1,false);
                      return self;
                    }
                    //value DataArrayChar
                  case 4:
                    {
                      self->setPartOfValues1(dacc,sTyyppArr.first,sTyyppArr.second.first,sTyyppArr.second.second,0,nbOfCompo,1,false);
                      return self;
                    }
                  default:
                    throw INTERP_KERNEL::Exception(msg);
                  }
                }
            }
          case 4:
            {//DataArrayInt
              switch(sw2)
                {
                  {//value char
                  case 1:
                    {
                      self->setPartOfValuesSimple3(vc,daIntTyypp->begin(),daIntTyypp->end(),0,nbOfCompo,1);
                      return self;
                    }
                    //value string
                  case 2:
                    {
                      MEDCouplingAutoRefCountObjectPtr<DataArrayAsciiChar> tmp=DataArrayAsciiChar::New(sc);
                      self->setPartOfValues3(tmp,daIntTyypp->begin(),daIntTyypp->end(),0,nbOfCompo,1,false);
                      return self;
                    }
                    //value vector<string>
                  case 3:
                    {
                      MEDCouplingAutoRefCountObjectPtr<DataArrayAsciiChar> tmp=DataArrayAsciiChar::New(vsc,' ');
                      self->setPartOfValues3(tmp,daIntTyypp->begin(),daIntTyypp->end(),0,nbOfCompo,1,false);
                      return self;
                    }
                    //value DataArrayChar
                  case 4:
                    {
                      self->setPartOfValues3(dacc,daIntTyypp->begin(),daIntTyypp->end(),0,nbOfCompo,1,false);
                      return self;
                    }
                  default:
                    throw INTERP_KERNEL::Exception(msg);
                  }
                }
            }
          default:
            throw INTERP_KERNEL::Exception(msg);
          }
      }
    }
  };

  class DataArrayAsciiCharTuple;

  class DataArrayAsciiCharIterator
  {
  public:
    DataArrayAsciiCharIterator(DataArrayAsciiChar *da);
    ~DataArrayAsciiCharIterator();
    %extend
    {
      PyObject *next()
      {
        DataArrayAsciiCharTuple *ret=self->nextt();
        if(ret)
          return SWIG_NewPointerObj(SWIG_as_voidptr(ret),SWIGTYPE_p_ParaMEDMEM__DataArrayAsciiCharTuple,SWIG_POINTER_OWN | 0);
        else
          {
            PyErr_SetString(PyExc_StopIteration,"No more data.");
            return 0;
          }
      }
    }
  };

  class DataArrayAsciiCharTuple
  {
  public:
    int getNumberOfCompo() const;
    DataArrayAsciiChar *buildDAAsciiChar(int nbOfTuples, int nbOfCompo) const throw(INTERP_KERNEL::Exception);
    %extend
    {
      std::string __str__() const
      {
        return self->repr();
      }
      
      DataArrayAsciiChar *buildDAAsciiChar()
      {
        return self->buildDAAsciiChar(1,self->getNumberOfCompo());
      }
    }
  };
}
