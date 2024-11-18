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
    int open(const std::string& path) override;
    int open(const QString& path);
    int play() override;
    int pause() override;
    int playOrPause() override;
    int stop() override;
    int setVolume(float volume) override;
signals:
    void error(Error err);
private slots:
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onMediaError(QMediaPlayer::Error error, const QString &errorString);
private:
    QMediaPlayer* player;
    QAudioOutput* audioOutput;
};

#endif // QAUDIOPLAYER_H
