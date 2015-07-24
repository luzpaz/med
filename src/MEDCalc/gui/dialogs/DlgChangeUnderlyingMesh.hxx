#ifndef _DlgChangeUnderlyingMesh_HXX
#define _DlgChangeUnderlyingMesh_HXX

#include <QtGui>
#include "ui_DlgChangeUnderlyingMesh.h"
#include "GenericDialog.hxx"

#include <SALOME_AppStudyEditor.hxx>
#include "MEDCALCGUIdialogs.hxx"

class MEDCALCGUI_DIALOGS_EXPORT DlgChangeUnderlyingMesh : public GenericDialog
{
  Q_OBJECT

public:
  DlgChangeUnderlyingMesh(SALOME_AppStudyEditor * studyEditor, QDialog *parent = 0);

  void setFieldId(int fieldId);
  int getFieldId();
  int getMeshId();

signals:
  void inputValidated();

protected slots:
  void OnSelectMesh();

 private:
  void accept();
  int _meshId;

  SALOME_AppStudyEditor * _studyEditor;
  int _fieldId;

  Ui_DlgChangeUnderlyingMesh ui; // instance of the class defined in ui_DlgChangeUnderlyingMesh.hxx
};


#endif // _DlgChangeUnderlyingMesh_HXX
