#include "mainwindow.h"
#include "dlgsignin.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    using namespace vvf;
    QApplication app(argc, argv);
    app.setOrganizationName("MANO \'MDC\'");
    app.setApplicationName("Queue registrator");

    DlgSignIn signIn;
    MainWindow mainWindow;
    QObject::connect(&signIn, SIGNAL(rejected()), &mainWindow, SLOT(close()));
    QObject::connect(&signIn, SIGNAL(accepted()), &mainWindow, SLOT(show()));
    QObject::connect(&signIn, SIGNAL(loginAccepted(QString)), &mainWindow, SLOT(setUserName(QString)));
    signIn.show();

    return app.exec();
}
