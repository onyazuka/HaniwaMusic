#include "HaniwaMusic.h"
#ifdef __unix__
#include "QMediaKeysDBusAdaptor.hpp"
#endif
#ifdef _MSC_VER
#include "QMediaKeysWinapiAdaptor.hpp"
#endif

#include <QApplication>
#include <QFontDatabase>
#include "QOptionsDlg.hpp"

int main(int argc, char *argv[])
{
    //std::ifstream ifs("/home/onyazuka/playlist.m3u");
    //m3u::M3UPlaylist playlist = m3u::M3UPlaylist::fromStream(ifs);

    srand(time(0));
    QApplication a(argc, argv);

    QSettings settings(ORGANIZATION_NAME, APP_NAME);
    QString language = settings.value("Language", QOptionsDlg::defaultLanguage()).toString();
    QColor mainColor = settings.value("MainColor", QColor()).value<QColor>();
    QTranslator Translator;
    if (language == "rus") {
        if (Translator.load("HaniwaMusic_ru.qm")) {
            QApplication::installTranslator(&Translator);
        }
        else {
            qWarning() << "couldn't apply russian localization";
        }
    }
    if (mainColor.isValid()) {
        QPalette pal = a.palette();
        pal.setColor(QPalette::ColorRole::Highlight, mainColor);
        a.setPalette(pal);
    }
    //if (Translator.load("HaniwaMusic_ru.qm")) {
    //    a.installTranslator(&Translator);
    //}
    if (QFontDatabase::addApplicationFont(":/fonts/Seven Segment.ttf") < 0) {
        qDebug() << "Couldn't register fonts";
    }

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
