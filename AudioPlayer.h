#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <string>

class AudioPlayer {
public:
    enum class Error {
        InvalidMedia
    };
    virtual ~AudioPlayer() {};
    virtual int open(const std::string& path) = 0;
    virtual int play() = 0;
    virtual int pause() = 0;
    virtual int playOrPause() = 0;
    virtual int stop() = 0;
    virtual int setVolume(float volume) = 0;
};

#endif // AUDIOPLAYER_H
