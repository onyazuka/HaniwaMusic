Minimalistic audio player, written in C++ and Qt.

## Compilation
cmake

## License 
MIT

## Features
- Can play .mp3, .flac, .wav audio
- Multiplatform (thanks Qt)
- Basic functionality (random, repeat, search by name, playlists, seek etc)
- Media keys support 
- Multiple playlists support
- Import/export playlists
- Play queue
- Localization (English and Russian)
- Styling customization
- Playlist sorting

## Ambitions
- Maybe try ffmpeg or even own lib, instead of QMediaPlayer, to play audio (?)

Uses own library for parsing .mp3, .flac and .wav metadata tags and audio duration. 
Uses QMediaPlayer for playing audio.


![Снимок экрана от 2025-02-19 20-03-48](https://github.com/user-attachments/assets/bb6a1f04-490d-4855-993f-35045424d6df)



