#ifndef _DlgInterpolateField_HXX
#define _DlgInterpolateField_HXX

#include "ui_DlgInterpolateField.h"
#include "GenericDialog.hxx"

#include <SALOME_AppStudyEditor.hxx>
#include "MEDCALCGUIdialogs.hxx"

class MEDCALCGUI_DIALOGS_EXPORT DlgInterpolateField : public GenericDialog
{
  Q_OBJECT

public:
  DlgInterpolateField(SALOME_AppStudyEditor * studyEditor, QDialog *parent = 0);

  void setFieldId(int fieldId);
  int getFieldId();
  int getMeshId();

  double getPrecision();
  double getDefaultValue();
  bool getReverse();
  std::string getIntersectionType();
  std::string getMethod();
  std::string getFieldNature();

signals:
  void inputValidated();

protected slots:
  void OnSelectMesh();

 private:
  void accept();
  int _meshId;

  SALOME_AppStudyEditor * _studyEditor;
  int _fieldId;

  Ui_DlgInterpolateField ui; // instance of the class defined in ui_dlgInterpolateField.hxx
};


#endif // _DlgInterpolateField_HXX
