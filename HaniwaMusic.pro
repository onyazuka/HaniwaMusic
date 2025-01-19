QT       += core gui multimedia testlib dbus qml quick quickwidgets

QMAKE_PROJECT_DEPTH = 0

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20 console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    HaniwaMusic.cpp \
    M3UPlaylist.cpp \
    MetaTagsParser/FlacTagParser.cpp \
    MetaTagsParser/ID3V2Parser.cpp \
    MetaTagsParser/Mp3FrameParser.cpp \
    MetaTagsParser/Tag.cpp \
    MetaTagsParser/TagScout.cpp \
    MetaTagsParser/util.cpp \
    QAudioPlayer.cpp \
    QClickableSlider.cpp \
    QLabelElide.cpp \
    QMLControlButton.cpp \
    QMLControlCheckbox.cpp \
    QMediaKeysDBusAdaptor.cpp \
    QMediaKeysWinapiAdaptor.cpp \
    QMetadataDlg.cpp \
    QPlaylist.cpp \
    QPlaylistView.cpp \
    main.cpp \
    utils/UtilsString.cpp

HEADERS += \
    AudioPlayer.h \
    HaniwaMusic.h \
    M3UPlaylist.hpp \
    MetaTagsParser/FlacTagParser.hpp \
    MetaTagsParser/ID3V2Parser.hpp \
    MetaTagsParser/Mp3FrameParser.hpp \
    MetaTagsParser/Tag.hpp \
    MetaTagsParser/TagScout.hpp \
    MetaTagsParser/util.hpp \
    QAudioPlayer.h \
    QClickableSlider.h \
    QLabelElide.h \
    QMLControlButton.hpp \
    QMLControlCheckbox.hpp \
    QMediaKeysDBusAdaptor.hpp \
    QMediaKeysWinapiAdaptor.hpp \
    QMetadataDlg.hpp \
    QPlaylist.h \
    QPlaylistView.hpp \
    utils/UtilsString.hpp

FORMS += \
    HaniwaMusic.ui

win32 {
    LIBS += -luser32
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

SUBDIRS += \
    MetaTagsParser/ID3V2Parser.pro

DISTFILES += \
    QML/ControlButton.qml \
    QML/ControlCheckBox.qml

RESOURCES += \
    resources.qrc
