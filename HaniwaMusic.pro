QT       += core gui multimedia testlib dbus

QMAKE_PROJECT_DEPTH = 0

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20 console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    HaniwaMusic.cpp \
    MetaTagsParser/FlacTagParser.cpp \
    MetaTagsParser/ID3V2Parser.cpp \
    MetaTagsParser/Mp3FrameParser.cpp \
    MetaTagsParser/Tag.cpp \
    MetaTagsParser/TagScout.cpp \
    MetaTagsParser/util.cpp \
    QAudioPlayer.cpp \
    QClickableSlider.cpp \
    QLabelElide.cpp \
    QMediaKeysDBusAdaptor.cpp \
    QMetadataDlg.cpp \
    QPlaylist.cpp \
    main.cpp

HEADERS += \
    AudioPlayer.h \
    HaniwaMusic.h \
    MetaTagsParser/FlacTagParser.hpp \
    MetaTagsParser/ID3V2Parser.hpp \
    MetaTagsParser/Mp3FrameParser.hpp \
    MetaTagsParser/Tag.hpp \
    MetaTagsParser/TagScout.hpp \
    MetaTagsParser/util.hpp \
    QAudioPlayer.h \
    QClickableSlider.h \
    QLabelElide.h \
    QMediaKeysDBusAdaptor.hpp \
    QMetadataDlg.hpp \
    QPlaylist.h

FORMS += \
    HaniwaMusic.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

SUBDIRS += \
    MetaTagsParser/ID3V2Parser.pro
