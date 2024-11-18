#include "QAudioPlayer.h"

QAudioPlayer::QAudioPlayer() {
    player = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);

    connect(player, &QMediaPlayer::mediaStatusChanged, this, &QAudioPlayer::onMediaStatusChanged);
    connect(player, &QMediaPlayer::errorOccurred, this, &QAudioPlayer::onMediaError);
    connect(player, &QMediaPlayer::positionChanged, this, [this](qint64 pos){
        if (onProgress) {
            onProgress((float)pos / (float)player->duration());
        }
    });
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
    default:
        break;
    }
}

void QAudioPlayer::onMediaError(QMediaPlayer::Error, const QString&) {
    if (onError) onError(Error::InvalidMedia);
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
