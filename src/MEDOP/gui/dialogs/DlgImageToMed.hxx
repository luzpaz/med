#ifndef _DlgImageToMed_HXX
#define _DlgImageToMed_HXX

#include <QtGui>
#include "ui_DlgImageToMed.h"
#include "GenericDialog.hxx"

#include "MEDOPGUIdialogs.hxx"

class MEDOPGUI_DIALOGS_EXPORT DlgImageToMed : public GenericDialog
{
  Q_OBJECT
    
 public:
  DlgImageToMed(QDialog *parent = 0);
  
  QString getImageFilepath();
  QString getMedFilepath();
  bool    isAutoLoaded();
  void    setAutoLoaded(bool autoloaded);

 protected slots:
  void OnBtnImageFileChooser();
  void OnBtnMedFileChooser();

 private:
  Ui_DlgImageToMed ui; // instance of the class defined in ui_DlgImageToMed.hxx
};


#endif // _DlgImageToMed_HXX
