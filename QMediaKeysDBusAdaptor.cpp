#include "QMediaKeysDBusAdaptor.hpp"

QMediaKeysDBusAdaptor::QMediaKeysDBusAdaptor(QObject *parent, HaniwaMusic* receiver)
    : QDBusAbstractAdaptor(parent)
{
    connect(this, &QMediaKeysDBusAdaptor::NextSignal, receiver, &HaniwaMusic::onNext);
    connect(this, &QMediaKeysDBusAdaptor::PreviousSignal, receiver, &HaniwaMusic::onPrev);
    connect(this, &QMediaKeysDBusAdaptor::PlayPauseSignal, receiver, &HaniwaMusic::onPlayPausePress);
    connect(this, &QMediaKeysDBusAdaptor::PlaySignal, receiver, &HaniwaMusic::onPlayPress);
    connect(this, &QMediaKeysDBusAdaptor::PauseSignal, receiver, &HaniwaMusic::onPausePress);
    connect(this, &QMediaKeysDBusAdaptor::StopSignal, receiver, &HaniwaMusic::onStopPress);
}

void QMediaKeysDBusAdaptor::Next()
{
    emit NextSignal();
}

void QMediaKeysDBusAdaptor::Previous() {
    emit PreviousSignal();
}

void QMediaKeysDBusAdaptor::PlayPause() {
    emit PlayPauseSignal();
}

void QMediaKeysDBusAdaptor::QMediaKeysDBusAdaptor::Play() {
    emit PlaySignal();
}

void QMediaKeysDBusAdaptor::QMediaKeysDBusAdaptor::Pause() {
    emit PauseSignal();
}

void QMediaKeysDBusAdaptor::Stop() {
    emit StopSignal();
}
