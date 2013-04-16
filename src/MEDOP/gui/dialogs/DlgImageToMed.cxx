#include "DlgImageToMed.hxx"
#include "SALOME_GuiServices.hxx"

#include <SUIT_FileDlg.h>
#include <SUIT_Desktop.h>

#include <QString>
#include <QStringList>
#include <QPixmap>
#include <QIcon>

DlgImageToMed::DlgImageToMed(QDialog *parent) : GenericDialog(parent)
{
  ui.setupUi(this->getPanel());

  QString moduleName = "MED";
  QPixmap aPixmap = GUI::getResourcesManager()->loadPixmap( moduleName, tr("ICO_FOLDER") );
  QIcon icon(aPixmap);
  ui.btnImageFileChooser->setIcon(icon);
  ui.btnMedFileChooser->setIcon(icon);

  connect(ui.btnImageFileChooser,SIGNAL(clicked()),
          this, SLOT(OnBtnImageFileChooser()));
  connect(ui.btnMedFileChooser,SIGNAL(clicked()),
          this, SLOT(OnBtnMedFileChooser()));

  this->setWindowTitle("Créer un champ à partir d'une image");
}

QString DlgImageToMed::getImageFilepath() {
  return ui.txtImageFile->text().trimmed();
}

QString DlgImageToMed::getMedFilepath() {
  return ui.txtMedFile->text().trimmed();
}

void DlgImageToMed::setAutoLoaded(bool autoloaded) {
  if ( autoloaded ) {
    ui.chkAutoLoad->setCheckState(Qt::Checked);
  }
  else {
    ui.chkAutoLoad->setCheckState(Qt::Unchecked);
  }
}

bool DlgImageToMed::isAutoLoaded() {
  Qt::CheckState state = ui.chkAutoLoad->checkState();
  if ( state == Qt::Checked ) {
    return true;
  }
  return false;
}

void DlgImageToMed::OnBtnImageFileChooser() {
  QStringList filter;
  filter.append(tr("FILE_FILTER_PNG"));
  filter.append(tr("FILE_FILTER_JPG"));
  filter.append(tr("FILE_FILTER_PGM"));
  filter.append(tr("FILE_FILTER_ALL"));
  QString filename = SUIT_FileDlg::getFileName(ui.btnImageFileChooser,
                                               "",
                                               filter,
                                               tr("SELECT_IMAGE_FILE"),
                                               true);
  if ( filename.isEmpty() ) return;
  ui.txtImageFile->setText(filename);

  QString medFilename = (filename.remove(filename.size()-3,3))+"med";
  ui.txtMedFile->setText(medFilename);
}

void DlgImageToMed::OnBtnMedFileChooser() {
  QStringList filter;
  filter.append(tr("FILE_FILTER_MED"));
  QString filename = SUIT_FileDlg::getFileName(ui.btnMedFileChooser,
                                               "",
                                               filter,
                                               tr("SPECIFY_MED_FILE"),
                                               true);
  if ( filename.isEmpty() ) return;
  ui.txtMedFile->setText(filename);
}
