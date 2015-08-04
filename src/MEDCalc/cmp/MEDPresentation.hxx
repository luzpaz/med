// Copyright (C) 2011-2015  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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
// Authors: A Bruneton (CEA), C Aguerre (EdF)

#ifndef SRC_MEDCALC_CMP_MEDPRESENTATION_HXX_
#define SRC_MEDCALC_CMP_MEDPRESENTATION_HXX_

#include <Python.h>
#include "MEDCALC.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MEDDataManager)
#include CORBA_SERVER_HEADER(MEDPresentationManager)

#include <vector>
#include <map>
#include <string>

class MEDCALC_EXPORT MEDPresentation
{
  friend class MEDPresentationManager_i;

public:

  virtual ~MEDPresentation() {}

  void setProperty(const std::string& propName, const std::string& propValue);
  const std::string getProperty(const std::string& propName);
  std::string getFieldTypeString();

protected:

  MEDPresentation(MEDCALC::FieldHandler* fieldHdl):
    _fieldHandler(fieldHdl), _pipeline(0), _display(0), _properties()
  {}

  void generatePipeline();
  virtual void internalGeneratePipeline() = 0;
  PyObject * getPythonObjectFromMain(const char * var);
  void pushInternal(PyObject * obj, PyObject * disp = NULL);

protected:

  ///! field reference - borrowed.
  MEDCALC::FieldHandler* _fieldHandler;

  ///! Pipeline elements
  std::vector< PyObject * > _pipeline;

  ///! Corresponding display object, if any:
  std::vector< PyObject * > _display;

  ///! Presentation properties <key,value>
  std::map<std::string, std::string> _properties;
};

class MEDCALC_EXPORT MEDPresentationScalarMap :  public MEDPresentation
{
public:
  MEDPresentationScalarMap(MEDCALC::FieldHandler* fieldHdl, bool wireframe) :
    MEDPresentation(fieldHdl),
    _isWireframe(wireframe)
  {}
  virtual ~MEDPresentationScalarMap() {}

protected:
  virtual void internalGeneratePipeline();

private:
  bool _isWireframe;
};

#endif /* SRC_MEDCALC_CMP_MEDPRESENTATION_HXX_ */
