#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    srand(time(0));
    QApplication a(argc, argv);
    MainWindow w;
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName(APP_NAME);
    w.show();
    return a.exec();
}
