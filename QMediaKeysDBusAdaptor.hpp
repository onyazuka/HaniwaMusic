#ifndef QMEDIAKEYSDBUSADAPTOR_H
#define QMEDIAKEYSDBUSADAPTOR_H

#include <QtDBus/QtDBus>
#include <QtCore/QString>
#include "HaniwaMusic.h"

#define SERVICE_NAME "org.mpris.MediaPlayer2.HaniwaMusic"
#define OBJECT_PATH "/org/mpris/MediaPlayer2"
#define INTERFACE_NAME "org.mpris.MediaPlayer2.Player"

class QMediaKeysDBusAdaptor : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", INTERFACE_NAME)
    //Q_PROPERTY(bool CanGoNext READ CanGoNext CONSTANT)
public:
    QMediaKeysDBusAdaptor(QObject *parent, HaniwaMusic* receiver);
    //inline bool CanGoNext() const { return _CanGoNext; }
public slots:
    void Next();
    void Previous();
    void PlayPause();
    void Play();
    void Pause();
    void Stop();
signals:
    void NextSignal();
    void PreviousSignal();
    void PlayPauseSignal();
    void PlaySignal();
    void PauseSignal();
    void StopSignal();

private:
    //const bool _CanGoNext = false;
};


#endif // QMEDIAKEYSDBUSADAPTOR_H
