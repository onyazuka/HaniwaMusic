Minimalistic audio player, written in C++ and Qt.

## Compilation
cmake

## Features
- Can play .mp3, .flac audio
- Multiplatform (thanks Qt)
- Basic functionality (random, repeat, search by name, playlists, seek etc)
- Media keys support 
- Multiple playlists support
- Import/export playlists
- Play queue
- Localization (English and Russian)
- Styling customization

## Ambitions
- Maybe try ffmpeg or even own lib, instead of QMediaPlayer, to play audio (?)

Uses own library for parsing .mp3 and .flac metadata tags and audio duration. 
Uses QMediaPlayer for playing audio.



![111](https://github.com/user-attachments/assets/37c6c975-2ca9-45da-a5ef-da6b61e59bb4)

