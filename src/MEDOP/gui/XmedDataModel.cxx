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

#include "XmedDataModel.hxx"
#include <Basics_Utils.hxx>

//
// =================================================================
// XmedDataObject implementation
// =================================================================
//

XmedDataObject::XmedDataObject() : DataObject() {
}

/*! This function specified the localization of the object in the
 * hierarchical organization
 */
string XmedDataObject::getPath() {
  string path = 
    string(_fieldHandler.meshname) + pathsep +
    _getTypedFieldLabel();
  return path;
}

void XmedDataObject::setFieldHandler(MEDOP::FieldHandler fieldHandler) {
  _fieldHandler = fieldHandler;
  this->setLabel(string("it = ")+ToString(_fieldHandler.iteration));
}
MEDOP::FieldHandler * XmedDataObject::getFieldHandler() {
  return &_fieldHandler;
}

const char * XmedDataObject::mapTypeOfFieldLabel[XmedDataObject::NB_TYPE_OF_FIELDS] = 
  {"ON_CELLS", "ON_NODES", "ON_GAUSS_PT", "ON_GAUSS_NE" };
/*!
 * This function implements the convention for displaying a fieldname
 * characterized by its spatial discretisation type.
 */
string XmedDataObject::_getTypedFieldLabel() {
  // A field name could identify several MEDCoupling fields, that
  // differ by their spatial discretization on the mesh (values on
  // cells, values on nodes, ...). This spatial discretization is
  // specified by the TypeOfField that is an integer value in this
  // list:
  // 0 = ON_CELLS
  // 1 = ON_NODES
  // 2 = ON_GAUSS_PT
  // 3 = ON_GAUSS_NE
  if ( _fieldHandler.type < 0 || 
       _fieldHandler.type >= XmedDataObject::NB_TYPE_OF_FIELDS ) {
    return string("UNKNOWN");
  }
  string label(_fieldHandler.fieldname);
  label +=" ("+string(mapTypeOfFieldLabel[_fieldHandler.type])+")";
  return label;
}

//
// =================================================================
// XmedDataModel implementation
// =================================================================
//

/*!
 * This function creates a XmedDataObject instance of DataObject and
 * add the newly created instance to the model.
 */
DataObject * XmedDataModel::newDataObject() {
  XmedDataObject * dataObject = new XmedDataObject();
  this->addDataObject(dataObject);
  return dataObject;
}

//
// =================================================================
// XmedDataProcessor implementation
// =================================================================
//
void XmedDataProcessor::preprocess(QStringList itemNameIdList) {
  int maxsize = itemNameIdList.size();
  _resultingFieldHandlerList = new MEDOP::FieldHandlerList(maxsize);
  _resultingFieldIdList = new MEDOP::FieldIdList(maxsize);
  _objectIndex = 0;
}

MEDOP::FieldHandlerList * XmedDataProcessor::getResultingFieldHandlerList() {
  return _resultingFieldHandlerList._retn();
}

MEDOP::FieldIdList * XmedDataProcessor::getResultingFieldIdList() {
  return _resultingFieldIdList._retn();
}

void XmedDataProcessor::processDataObject(DataObject * dataObject) {
  XmedDataObject * xmedDataObject = (XmedDataObject *)dataObject;
  MEDOP::FieldHandler fieldHandler = *(xmedDataObject->getFieldHandler());
  int fieldId = fieldHandler.id;

  int newsize = _objectIndex+1;
  _resultingFieldHandlerList->length(newsize);
  _resultingFieldIdList->length(newsize);

  _resultingFieldHandlerList[_objectIndex] = fieldHandler;
  _resultingFieldIdList[_objectIndex]      = fieldId;
  _objectIndex++;

  // >>>>>
  // Example (if needed) for creating the list of id from the lsite of handler 
  /*
  MEDOP::FieldIdList_var fieldIdList = new MEDOP::FieldIdList();
  fieldIdList->length(fieldHandlerList->length());
  for (int i=0; i<fieldHandlerList->length(); i++) {
    MEDOP::FieldHandler fieldHandler = (*fieldHandlerList)[i];
    fieldIdList[i] = fieldHandler.id;
  }
  */
  // <<<<<
}
