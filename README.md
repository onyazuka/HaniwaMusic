Minimalistic audio player, written in C++ and Qt.

## Compilation
qmake

## Features
- Can play .mp3, .flac audio
- Multiplatform (thanks Qt)
- Basic functionality (random, repeat, search by name, playlists, seek etc)
- Media keys support (for now only on Linux Gnome)

## Will be done in nearest future
- Media keys for Windows
- Multiple playlists support
- Import/export playlists
- Some design

## Ambitions
- Maybe try ffmpeg or even own lib, instead of QMediaPlayer, to play audio (?)

Uses own library for parsing .mp3 and .flac metadata tags and audio duration. 
Uses QMediaPlayer for playing audio.



![Снимок экрана от 2025-01-11 12-31-59](https://github.com/user-attachments/assets/ed86af27-6204-41c8-8c1e-90d38771c828)
