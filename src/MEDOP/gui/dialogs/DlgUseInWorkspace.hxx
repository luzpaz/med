#ifndef _DlgUseInWorkspace_HXX
#define _DlgUseInWorkspace_HXX

#include <QtGui>
#include "ui_DlgUseInWorkspace.h"
#include "GenericDialog.hxx"

class DlgUseInWorkspace : public GenericDialog
{
  Q_OBJECT
    
 public:
  DlgUseInWorkspace(QDialog *parent = 0);
  
 private:
  Ui_DlgUseInWorkspace ui; // instance of the class defined in ui_DlgUseInWorkspace.hxx
};


#endif // _DlgUseInWorkspace_HXX
