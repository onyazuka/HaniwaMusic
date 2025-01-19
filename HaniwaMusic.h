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

#include "AudioPlayer.h"
#include "QLabelElide.h"
#include "QPlaylist.h"
#include "QClickableSlider.h"
#include "QMLControlButton.hpp"
#include "QMLControlCheckbox.hpp"
#include "QMLSlider.hpp"

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
protected slots:
    void onOpenPress();
    void onOpenDirPress();
    void onPlaylistChange(int n);
    void onVolumeSliderChanged(int newValue);
    bool onFileChanged(QString newFile);
    void onAudioError(AudioPlayer::Error error);
    void onPlaylistsMenuClicked();
protected:
    struct Settings {
        QString lastDir;
        float volume = 0.0f;
        QPoint pos;
        QRect windowRect;
        int lastTrackNumber = 0;
    } appSettings;

    bool checkFile(const QString& file);
    bool checkDir(const QString& file);
    void setAudio(const QString& file);
    void configureLayout();
    void configureAudio();
    void changeFileNameLabel(const QString& text, Qt::GlobalColor color);
    void loadSettings();
    void saveSettings();
    void onSearchNext();
    void initPlaylistsMenu();

    Ui::MainWindow *ui;

    QWidget* centralWidget;
    QMLControlButton* btnPlay;
    QMLControlButton* btnStop;
    QMLControlButton* btnNext;
    QMLControlButton* btnPrev;
    QMLControlCheckbox* chRandom;
    QMLControlCheckbox* chRepeat;
    QMLSlider* qmlSlider;
    QPushButton* btnOpen;
    QPushButton* btnOpenDir;
    QLabelElide* lFileName;
    QVBoxLayout* layout;
    QSlider* sldVolume;
    QClickableSlider* sldProgress;
    QLabel* lProgress;
    // REQUIREMENT: should have at least 1 playlist, so don't allow to close last playlist
    QTabWidget* tabPlaylists;
    QPlaylist* playlist;
    QPushButton* btnPlaylistsMenu;
    QLineEdit* lnSearch;
    QPushButton* btnSearch;

    QMenu* playlistsMenu = nullptr;
    QList<QAction*> playlistsMenuActions;

    std::shared_ptr<AudioPlayer> player;
};
#endif // HANIWAMUSIC_H
