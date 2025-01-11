#ifndef QMEDIAKEYSDBUSADAPTOR_H
#define QMEDIAKEYSDBUSADAPTOR_H

#include <QtDBus/QtDBus>
#include <QtCore/QString>
#include "HaniwaMusic.h"

#define SERVICE_NAME "org.mpris.MediaPlayer2.HaniwaMusic"
#define OBJECT_PATH "/org/mpris/MediaPlayer2"
#define INTERFACE_NAME "org.mpris.MediaPlayer2.Player"

class QMediaKeysDBusAdaptor : public QDBusAbstractAdaptor, public MainWindow {
    Q_OBJECT;
    Q_CLASSINFO("D-Bus Interface", INTERFACE_NAME);
public:
    QMediaKeysDBusAdaptor(QObject *parent);
    ~QMediaKeysDBusAdaptor();
public slots:
    void Next();
    void Previous();
};


#endif // QMEDIAKEYSDBUSADAPTOR_H
