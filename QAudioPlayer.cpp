#include "QAudioPlayer.h"

QAudioPlayer::QAudioPlayer() {
    player = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);

    connect(player, &QMediaPlayer::mediaStatusChanged, this, &QAudioPlayer::onMediaStatusChanged);
    connect(player, &QMediaPlayer::errorOccurred, this, &QAudioPlayer::onMediaError);
}

QAudioPlayer::~QAudioPlayer() {
    delete audioOutput;
    delete player;
}

void QAudioPlayer::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    switch (status) {
    case QMediaPlayer::MediaStatus::InvalidMedia:
        emit error(Error::InvalidMedia);
        break;
    default:
        break;
    }
}

void QAudioPlayer::onMediaError(QMediaPlayer::Error, const QString&) {
    emit error(Error::InvalidMedia);
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
