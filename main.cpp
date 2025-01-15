#include "HaniwaMusic.h"
#ifdef __unix__
#include "QMediaKeysDBusAdaptor.hpp"
#endif
#ifdef _MSC_VER
#include "QMediaKeysWinapiAdaptor.hpp"
#endif

#include <QApplication>

int main(int argc, char *argv[])
{
    srand(time(0));
    QApplication a(argc, argv);
    HaniwaMusic w;
    QCoreApplication::setOrganizationName(ORGANIZATION_NAME);
    QCoreApplication::setApplicationName(APP_NAME);

#ifdef _MSC_VER
    QMediaKeysWinapiAdaptor media(0, &w);

#endif

#ifdef __unix__
    // TODO: add process 'gsd-media-keys' check
    QObject obj;
    QMediaKeysDBusAdaptor* dbusMediaKeys = new QMediaKeysDBusAdaptor(&obj, &w);

    QDBusConnection dbus = QDBusConnection::sessionBus();
    if (!dbus.registerObject(OBJECT_PATH, &obj)) {
        qDebug("object was NOT registered\n");
        qDebug("%s\n", qPrintable(dbus.lastError().message()));
        exit(1);
    }
    if (!dbus.registerService(SERVICE_NAME)) {
        qDebug("service was NOT registered\n");
        qDebug("%s\n", qPrintable(dbus.lastError().message()));
        exit(1);
    }
#endif

    w.show();
    return a.exec();
}
