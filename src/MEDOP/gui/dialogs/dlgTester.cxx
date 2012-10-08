//
// This program can be used for unit test of dialog box.
// You just have to include the interface file (*.h) and
// use the dialog class as illustrated in the functions TEST_*.
// (gboulant - 12/10/2010)
//
#include <QApplication>
#include <QtGui>

#include "DlgAlias.hxx"

#include <iostream>
using std::cout;
using std::endl;
#define LOG(msg) cout << "[XMED - dlgTester]::"<< msg << endl;



void TEST_dlgAlias_show(int argc, char *argv[]) { 
  QApplication app(argc,argv);

  DlgAlias dialog;
  dialog.setAlias("vitesse");
  dialog.show();

  app.exec();
}

void TEST_dlgAlias_exec(int argc, char *argv[]) {
  QApplication app(argc,argv);

  DlgAlias dialog;
  QString alias = "vitesse";
  dialog.setAlias(alias);
  int choice = dialog.exec();
  if ( choice == QDialog::Accepted ) {
    LOG("Clic on OK");
  }
  else {
    LOG("Clic on Cancel");
  }

  //app.exec();
}


int main(int argc, char *argv[])
{
  
  //TEST_dlgAlias_show(argc, argv);
  TEST_dlgAlias_exec(argc, argv);
  
}
