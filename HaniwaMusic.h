#ifndef HANIWAMUSIC_H
#define HANIWAMUSIC_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QFileInfo>
#include <QLineEdit>
#include <QCheckBox>
#include <QTabWidget>
#include "QTabWidgetCustom.hpp"

#include "AudioPlayer.h"
#include "QLabelElide.h"
#include "QPlaylist.h"
#include "QMLWidgets.hpp"

#define ORGANIZATION_NAME "HaniwaSofuto"
#define APP_NAME "HaniwaMusic"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class HaniwaMusic : public QMainWindow
{
    Q_OBJECT

public:
    HaniwaMusic(QWidget *parent = nullptr);
    ~HaniwaMusic();
    QPlaylist* currentPlaylist();
public slots:
    void onNext();
    void onPrev();
    void onPlayPausePress();
    void onPlayPress();
    void onPausePress();
    void onStopPress();
    void onClose();
    void addPlaylist(QString title = "");
    void removePlaylist();
    void clearPlaylist();
    void sortPlaylistByTitle();
    void sortPlaylistByArtist();
    void sortPlaylistByDuration();
    void sortPlaylistByPath();
    void importPlaylist();
    void exportPlaylist();
protected slots:
    void onOpenPress();
    void onOpenDirPress();
    void onPlaylistChange(int n);
    void onMute();
    void onVolumeSliderChanged();
    void updatePlayerPosition();
    bool onFileChanged(QString newFile);
    void onAudioError(AudioPlayer::Error error);
    void onPlaylistsMenuClicked();
    void onSearchNext();
    void onOptions();
protected:
    struct Settings {
        QString lastDir;
        float volume = 0.0f;
        QPoint pos;
        QRect windowRect;
        int lastTrackNumber = 0;
        QString language;
        QColor mainColor;
    } appSettings;

    bool checkFile(const QString& file);
    bool checkDir(const QString& file);
    void setAudio(const QString& file);
    void configureLayout();
    void configureAudio();
    void changeFileNameLabel(const QString& text, Qt::GlobalColor color);
    void loadSettings();
    void saveSettings();
    void initPlaylistsMenu();

    Ui::MainWindow *ui;

    QWidget* centralWidget;
    QMLControlButton* btnPlay;
    QMLControlButton* btnStop;
    QMLControlButton* btnNext;
    QMLControlButton* btnPrev;
    QMLControlCheckbox* chRandom;
    QMLControlCheckbox* chRepeat;
    QLabelElide* lFileName;
    QVBoxLayout* layout;
    QMLIcon* iVolume;
    QMLSlider* sldVolume;
    QMLSlider* sldProgress;
    QLabel* lProgress;
    // REQUIREMENT: should have at least 1 playlist, so don't allow to close last playlist
    QTabWidgetCustom* tabPlaylists;
    QPlaylist* playlist;
    QMLMenuButton* btnOpen;
    QMLMenuButton* btnOpenDir;
    QMLMenuButton* btnPlaylistsMenu;
    QMLMenuButton* btnOptions;
    QLineEdit* lnSearch;
    QMLMenuButton* btnSearch;

    QMenu* playlistsMenu = nullptr;
    QList<QAction*> playlistsMenuActions;
    float memoVolume = 0.0f;

    std::shared_ptr<AudioPlayer> player;
};
#endif // HANIWAMUSIC_H
