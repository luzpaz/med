#ifndef _XMEDDATAMODEL_H_
#define _XMEDDATAMODEL_H_

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(MEDDataManager)

#include "MEDOPGUI.hxx"

//
// =================================================================
// Definition of an atom in the data model as an implementation of
// the virtual class DataObject
// =================================================================
//
#include "DataObject.hxx"
class MEDOPGUI_EXPORT XmedDataObject: public DataObject {
public:
  XmedDataObject();
  virtual string getPath();

  void setFieldHandler(MEDOP::FieldHandler fieldHandler);
  MEDOP::FieldHandler * getFieldHandler();

  static const int NB_TYPE_OF_FIELDS = 4;
  static const char *  mapTypeOfFieldLabel[NB_TYPE_OF_FIELDS];

private:
  MEDOP::FieldHandler _fieldHandler;
  string _getTypedFieldLabel();
};

//
// =================================================================
// Definition of the data model as an implementation of the virtual
// class DataModel. It implements the DataObject factory.
// =================================================================
//
#include "DataModel.hxx"
class MEDOPGUI_EXPORT XmedDataModel: public DataModel {
public:
  XmedDataModel():DataModel() {};
  virtual DataObject * newDataObject();
};

//
// =================================================================
// Base class for data processing
// =================================================================
// Howto use it? 
//
// 1) Create an instance, call the "process" function,
// then get the result you are interested in, using the corresponding
// "result*" function.
//
// 2) Alternativly, you can just call directly the suitable master
// "get*" function to process and get the result you are interested
// in.
//
#include "DataProcessor.hxx"

class MEDOPGUI_EXPORT XmedDataProcessor : public DataProcessor {
public:
  XmedDataProcessor(DataModel * dataModel):DataProcessor(dataModel) {};

  MEDOP::FieldHandlerList * getResultingFieldHandlerList();
  MEDOP::FieldIdList * getResultingFieldIdList();

protected:
  virtual void preprocess(QStringList itemNameIdList);
  virtual void processDataObject(DataObject * dataObject);

private:
  MEDOP::FieldHandlerList_var _resultingFieldHandlerList;
  MEDOP::FieldIdList_var _resultingFieldIdList;
  int _objectIndex;

};


#endif // _XMEDDATAMODEL_H_
