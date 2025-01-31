# Generated from HaniwaMusic.pro.

cmake_minimum_required(VERSION 3.16)
project(HaniwaMusic VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS DBus Gui Multimedia Qml Quick QuickWidgets Test LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Widgets)

add_subdirectory(MetaTagsParser)

set(SOURCES
    AudioPlayer.h
    HaniwaMusic.cpp HaniwaMusic.h HaniwaMusic.ui
    M3UPlaylist.cpp M3UPlaylist.hpp
    QAudioPlayer.cpp QAudioPlayer.h
    QClickableSlider.cpp QClickableSlider.h
    QLabelElide.cpp QLabelElide.h
    QMLControlButton.cpp QMLControlButton.hpp
    QMLControlCheckbox.cpp QMLControlCheckbox.hpp
    QMLIcon.cpp QMLIcon.hpp
    QMLMenuButton.cpp QMLMenuButton.hpp
    QMLSlider.cpp QMLSlider.hpp
    QMLWidgetBase.cpp QMLWidgetBase.hpp
    QMLWidgets.hpp
    QMetadataDlg.cpp QMetadataDlg.hpp
    QPlaylist.cpp QPlaylist.h
    QPlaylistView.cpp QPlaylistView.hpp
    QTabWidgetCustom.cpp QTabWidgetCustom.hpp
    main.cpp
    utils/UtilsString.cpp utils/UtilsString.hpp
    resources.qrc
)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(STATUS "Using GCC compiler")
    list(APPEND SOURCES QMediaKeysDBusAdaptor.cpp QMediaKeysDBusAdaptor.hpp)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Using MSVC compiler")
    list(APPEND SOURCES QMediaKeysWinapiAdaptor.cpp QMediaKeysWinapiAdaptor.hpp)
else()
    message(FATAL_ERROR "Using unsupported compiler: only GCC and MSVC are supported! Exiting...")
endif()

qt_standard_project_setup(
    I18N_SOURCE_LANGUAGE en         # optional - this is the default
    I18N_TRANSLATED_LANGUAGES ru
)

qt_add_executable(HaniwaMusic MANUAL_FINALIZATION ${SOURCES})

qt_add_qml_module(HaniwaMusic
    URI HaniwaMusicQml
    VERSION ${PROJECT_VERSION}
    QML_FILES
        QML/ControlButton.qml
        QML/ControlCheckBox.qml
        QML/Slider.qml
        QML/Icon.qml
        QML/MenuButton.qml
    RESOURCES
        icons/random.svg
        icons/repeat.svg
        "fonts/Seven Segment.ttf"
        icons/mute.svg
        icons/volume.svg
        icons/openFile.svg
        icons/openFolder.svg
        icons/playlist.svg
        icons/search.svg
        qtquickcontrols2.conf
        icons/next.svg
        icons/pause.svg
        icons/play.svg
        icons/previous.svg
        icons/stop.svg
        icons/next1.svg
        icons/pause1.svg
        icons/play1.svg
        icons/prev1.svg
        icons/stop1.svg
    NO_RESOURCE_TARGET_PATH
)

qt6_add_translations(HaniwaMusic TS_FILE_DIR translations QM_FILES_OUTPUT_VARIABLE qm_files)

add_dependencies(HaniwaMusic HaniwaMusic_lupdate)
add_dependencies(HaniwaMusic HaniwaMusic_lrelease)
add_dependencies(HaniwaMusic update_translations)

target_link_libraries(HaniwaMusic PUBLIC
    Qt::Core
    Qt::DBus
    Qt::Gui
    Qt::Multimedia
    Qt::Qml
    Qt::Quick
    Qt::QuickWidgets
    Qt::Test
    MetaTagsParser
)

if((QT_VERSION_MAJOR GREATER 4))
    target_link_libraries(HaniwaMusic PUBLIC
        Qt::Widgets
    )
endif()

if(WIN32)
    target_link_libraries(HaniwaMusic PUBLIC
        user32
    )
endif()

set_target_properties(HaniwaMusic PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_include_directories(HaniwaMusic PUBLIC "${PROJECT_SOURCE_DIR}/MetaTagsParser")

include(GNUInstallDirs)
install(TARGETS HaniwaMusic
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
install(FILES ${qm_files} DESTINATION "${CMAKE_BINARY_DIR}/translations")

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(HaniwaMusic)
endif()

