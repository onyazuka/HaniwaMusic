#include "QAudioPlayer.h"
#include <QDebug>
#include <iostream>

QAudioPlayer::QAudioPlayer() {
    player = new QMediaPlayer;

    connect(player, &QMediaPlayer::mediaStatusChanged, this, &QAudioPlayer::onMediaStatusChanged);
    connect(player, &QMediaPlayer::errorOccurred, this, &QAudioPlayer::onMediaError);
    connect(player, &QMediaPlayer::positionChanged, this, [this](qint64 pos){
        if (onProgress) {
            onProgress((float)pos / (float)player->duration());
        }
    });
    connect(player, &QMediaPlayer::durationChanged, this, &QAudioPlayer::onAudioDurationChanged);
}

QAudioPlayer::~QAudioPlayer() {
    delete audioOutput;
    delete player;
}

void QAudioPlayer::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    switch (status) {
    case QMediaPlayer::MediaStatus::InvalidMedia:
        if (onError) onError(Error::InvalidMedia);
        break;
    case QMediaPlayer::MediaStatus::EndOfMedia:
        if (onAudioEnd) onAudioEnd();
    default:
        break;
    }
}

void QAudioPlayer::onMediaError(QMediaPlayer::Error, const QString&) {
    if (onError) onError(Error::InvalidMedia);
}

void QAudioPlayer::onAudioDurationChanged(qint64 duration) {
    qDebug() << QString("Duration changed: %1").arg(duration);
}

int QAudioPlayer::init() {
    audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    return 0;
}

int QAudioPlayer::open(const std::string& path) {
    return open(QString(path.data()));
}

int QAudioPlayer::open(const QString& path) {
    player->setSource(QUrl::fromLocalFile(path));
    return 0;
}

int QAudioPlayer::play() {
    player->play();
    return 0;
}

int QAudioPlayer::pause() {
    player->pause();
    return 0;
}

int QAudioPlayer::playOrPause() {
    if (player->isPlaying()) {
        player->pause();
    }
    else {
        player->play();
    }
    return 0;
}

int QAudioPlayer::stop() {
    player->stop();
    return 0;
}

int QAudioPlayer::setVolume(float volume) {
    audioOutput->setVolume(volume);
    return 0;
}

int QAudioPlayer::setPosition(float pos) {
    player->setPosition(player->duration() * pos);
    return 0;
}

void QAudioPlayer::setOnErrorCb(OnErrorCb cb) {
    onError = cb;
}

void QAudioPlayer::setOnProgressCb(OnProgressCb cb) {
    onProgress = cb;
}

void QAudioPlayer::setOnAudioEndCb(OnAudioEndCb cb) {
    onAudioEnd = cb;
}
