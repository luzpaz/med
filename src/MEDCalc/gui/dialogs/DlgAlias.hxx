#ifndef DLGALIAS_H
#define DLGALIAS_H

#include "ui_DlgAlias.h"
#include "MEDCALCGUIdialogs.hxx"

class MEDCALCGUI_DIALOGS_EXPORT DlgAlias : public QDialog
{
  Q_OBJECT

 public:
  DlgAlias(QDialog *parent = 0);

  void setAlias(QString alias);
  QString getAlias();

  private slots:
    void accept();
  //void reject();

  private:
    Ui_DlgAlias ui; // instance of the class defined in ui_DlgAlias.h
};


#endif // DLGALIAS_H
