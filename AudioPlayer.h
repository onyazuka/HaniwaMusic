#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <string>
#include <functional>

class AudioPlayer {
public:
    enum class Error {
        InvalidMedia
    };
    enum class PlayState {
        Play,
        Pause,
        Stop
    };

    using OnErrorCb = std::function<void(Error)>;
    using OnProgressCb = std::function<void(float)>;
    using OnAudioEndCb = std::function<void()>;
    using OnPlayStateChangeCb = std::function<void(PlayState)>;
    virtual ~AudioPlayer() {};
    virtual int init() = 0;
    virtual int open(const std::string& path) = 0;
    virtual int close() = 0;
    virtual int play() = 0;
    virtual int pause() = 0;
    virtual int playOrPause() = 0;
    virtual int stop() = 0;
    virtual int setVolume(float volume) = 0;
    virtual int duration() = 0;
    // pos: in percents
    virtual int setPosition(float pos) = 0;
    virtual void setOnErrorCb(OnErrorCb cb) = 0;
    virtual void setOnProgressCb(OnProgressCb cb) = 0;
    virtual void setOnAudioEndCb(OnAudioEndCb cb) = 0;
    virtual void setOnPlayStateChangeCb(OnPlayStateChangeCb cb) = 0;
};

#endif // AUDIOPLAYER_H
