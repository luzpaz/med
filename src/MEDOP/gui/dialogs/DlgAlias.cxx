#include "DlgAlias.hxx"

#include <iostream>
using std::cout;
using std::endl;
#define LOG(msg) cout << "[XMED - DlgAlias]::"<< msg << endl;


DlgAlias::DlgAlias(QDialog *parent) : QDialog(parent)
{
  ui.setupUi(this); // A faire en premier
  
  /*
    Personnalisez vos widgets ici si nécessaire
    Réalisez des connexions supplémentaires entre signaux et slots
  */
  
  // The slots accept() and reject() are already connected to the
  // buttonbox (inherited features) 
}

void DlgAlias::accept() {
  if ( this->getAlias().isEmpty() ) {
    LOG("accept(): the alias can't be void");
  }
  else {
    QDialog::accept();
  }
}

//void DlgAlias::reject() {
//  LOG("reject() is not implemented yet");
//}

void DlgAlias::setAlias(QString alias) {
  ui.txtAlias->setText(alias);
}

QString DlgAlias::getAlias() {
  return ui.txtAlias->text().trimmed();
}
