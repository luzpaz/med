#include "DlgChangeUnderlyingMesh.hxx"
#include "DatasourceConstants.hxx"

#include <QString>
#include <QMessageBox>

DlgChangeUnderlyingMesh::DlgChangeUnderlyingMesh(SALOME_AppStudyEditor * studyEditor,
             QDialog * parent)
  : GenericDialog(parent)
{
  ui.setupUi(this->getPanel());
  _meshId=-1;
  _studyEditor = studyEditor;
  connect(this->ui.btnSelectMesh, SIGNAL(clicked()), this, SLOT(OnSelectMesh()));
  this->setWindowTitle("Remplacement du maillage support");
}

/** This reset the dialog for a new selection */
void DlgChangeUnderlyingMesh::setFieldId(int fieldId) {
  _fieldId = fieldId;
  _meshId=-1;
  this->ui.txtMesh->setText(QString(""));
}
int DlgChangeUnderlyingMesh::getFieldId() {
  return _fieldId;
}

int DlgChangeUnderlyingMesh::getMeshId() {
  return _meshId;
}

void DlgChangeUnderlyingMesh::accept() {
  if ( _meshId == -1 ) {
    QMessageBox::warning(this,
       tr("Data verification"),
       tr("You must select a mesh in the explorer and clic the button Mesh"));
  }
  else {
    GenericDialog::accept();
    emit inputValidated();
  }
}

void DlgChangeUnderlyingMesh::OnSelectMesh() {
  SALOME_StudyEditor::SObjectList * listOfSObject = _studyEditor->getSelectedObjects();
  if ( listOfSObject->size() > 0 ) {
    SALOMEDS::SObject_var soMesh = listOfSObject->at(0);
    // _GBO_ TODO: we should test here if it is a mesh (attribute in
    // the sobject)
    _meshId = _studyEditor->getParameterInt(soMesh,OBJECT_ID);
    const char * meshname = _studyEditor->getName(soMesh);
    this->ui.txtMesh->setText(QString(meshname));
  }

}
