#ifndef QAUDIOPLAYER_H
#define QAUDIOPLAYER_H

#include "AudioPlayer.h"

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QObject>

class QAudioPlayer : public QObject, public AudioPlayer {
    Q_OBJECT
public:
    QAudioPlayer();
    ~QAudioPlayer();
public slots:
    int init() override;
    int open(const std::string& path) override;
    int open(const QString& path);
    int close() override;
    int play() override;
    int pause() override;
    int playOrPause() override;
    int stop() override;
    int duration() override;
    float volume() override;
    int setVolume(float volume) override;
    int setPosition(float pos) override;
    void setOnErrorCb(OnErrorCb cb) override;
    void setOnProgressCb(OnProgressCb cb) override;
    void setOnAudioEndCb(OnAudioEndCb cb) override;
    void setOnPlayStateChangeCb(OnPlayStateChangeCb cb) override;
    inline QMediaPlayer* __getPlayer() { return player; }
private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onMediaError(QMediaPlayer::Error error, const QString &errorString);
    void onAudioDurationChanged(qint64 duration);
    void onPlaybackStateChanged(QMediaPlayer::PlaybackState newState);
private:
    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    OnErrorCb onError = nullptr;
    OnProgressCb onProgress = nullptr;
    OnAudioEndCb onAudioEnd = nullptr;
    OnPlayStateChangeCb onPlayStateChange = nullptr;
};

#endif // QAUDIOPLAYER_H
