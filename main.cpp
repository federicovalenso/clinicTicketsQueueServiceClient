#include "dlgsignin.h"
#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
  using namespace vvf;
  QApplication app(argc, argv);
  app.setOrganizationName("MANO \'MDC\'");
  app.setApplicationName("Queue registrator");

  RequestsProcessor rp(&app);
  DlgSignIn signIn(nullptr, &rp);
  MainWindow mainWindow(nullptr, &rp);
  QObject::connect(&signIn, SIGNAL(rejected()), &mainWindow, SLOT(close()));
  QObject::connect(&signIn, SIGNAL(accepted()), &mainWindow, SLOT(show()));
  signIn.show();

  return app.exec();
}
