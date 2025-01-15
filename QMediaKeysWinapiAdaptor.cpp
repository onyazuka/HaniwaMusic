#ifdef _MSC_VER

#include "QMediaKeysWinapiAdaptor.hpp"
#include <Windows.h>

enum Hotkeys {
    PlayPause,
    Stop,
    Next,
    Prev
};

QMediaKeysWinapiAdaptor::QMediaKeysWinapiAdaptor(QWidget* parent, HaniwaMusic* receiver)
    : QWidget(parent)
{
    HWND hwnd = (HWND)winId();
    if (!RegisterHotKey(hwnd, Hotkeys::PlayPause, 0, VK_MEDIA_PLAY_PAUSE)) {
        qDebug() << "Failed to register Play/Pause hotkey!";
    }
    if (!RegisterHotKey(hwnd, Hotkeys::Stop, 0, VK_MEDIA_STOP)) {
        qDebug() << "Failed to register Play/Pause hotkey!";
    }
    if (!RegisterHotKey(hwnd, Hotkeys::Next, 0, VK_MEDIA_NEXT_TRACK)) {
        qDebug() << "Failed to register Next Track hotkey!";
    }
    if (!RegisterHotKey(hwnd, Hotkeys::Prev, 0, VK_MEDIA_PREV_TRACK)) {
        qDebug() << "Failed to register Previous Track hotkey!";
    }

    connect(this, &QMediaKeysWinapiAdaptor::NextSignal, receiver, &HaniwaMusic::onNext);
    connect(this, &QMediaKeysWinapiAdaptor::PreviousSignal, receiver, &HaniwaMusic::onPrev);
    connect(this, &QMediaKeysWinapiAdaptor::PlayPauseSignal, receiver, &HaniwaMusic::onPlayPausePress);
    connect(this, &QMediaKeysWinapiAdaptor::PlaySignal, receiver, &HaniwaMusic::onPlayPress);
    connect(this, &QMediaKeysWinapiAdaptor::PauseSignal, receiver, &HaniwaMusic::onPausePress);
    connect(this, &QMediaKeysWinapiAdaptor::StopSignal, receiver, &HaniwaMusic::onStopPress);
 }

void QMediaKeysWinapiAdaptor::Next()
{
    emit NextSignal();
}

void QMediaKeysWinapiAdaptor::Previous() {
    emit PreviousSignal();
}

void QMediaKeysWinapiAdaptor::PlayPause() {
    emit PlayPauseSignal();
}

void QMediaKeysWinapiAdaptor::Play() {
    emit PlaySignal();
}

void QMediaKeysWinapiAdaptor::Pause() {
    emit PauseSignal();
}

void QMediaKeysWinapiAdaptor::Stop() {
    emit StopSignal();
}

QMediaKeysWinapiAdaptor::~QMediaKeysWinapiAdaptor() {
    HWND hwnd = (HWND)winId();
    UnregisterHotKey(hwnd, Hotkeys::PlayPause);
    UnregisterHotKey(hwnd, Hotkeys::Stop);
    UnregisterHotKey(hwnd, Hotkeys::Next);
    UnregisterHotKey(hwnd, Hotkeys::Prev);
}

bool QMediaKeysWinapiAdaptor::nativeEvent(const QByteArray &eventType, void *message, qintptr *result) {
    MSG* msg = static_cast<MSG*>(message);
    if (msg->message == WM_HOTKEY)
    {
        switch (msg->wParam) {
        case Hotkeys::PlayPause:
            emit PlayPauseSignal();
            break;
        case Hotkeys::Stop:
            emit StopSignal();
            break;
        case Hotkeys::Next:
            emit NextSignal();
            break;
        case Hotkeys::Prev:
            emit PreviousSignal();
            break;
        }
    }
    return false;
}

#endif
