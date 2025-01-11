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

#include "AudioPlayer.h"
#include "QLabelElide.h"
#include "QPlaylist.h"
#include "QClickableSlider.h"

#define ORGANIZATION_NAME "HaniwaSofuto"
#define APP_NAME "HelloQt"

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
public slots:
    void onNext();
    void onPrev();
    void onPlayPausePress();
    void onPlayPress();
    void onPausePress();
    void onStopPress();

protected slots:
    void onOpenPress();
    void onOpenDirPress();
    void onVolumeSliderChanged(int newValue);
    bool onFileChanged(QString newFile);
    void onAudioError(AudioPlayer::Error error);
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

    Ui::MainWindow *ui;

    QWidget* centralWidget;
    QPushButton* btnPlay;
    QPushButton* btnStop;
    QPushButton* btnNext;
    QPushButton* btnPrev;
    QCheckBox* chRandom;
    QCheckBox* chRepeat;
    QPushButton* btnOpen;
    QPushButton* btnOpenDir;
    QLabelElide* lFileName;
    QVBoxLayout* layout;
    QSlider* sldVolume;
    QClickableSlider* sldProgress;
    QLabel* lProgress;
    QPlaylist* playlist;
    QLineEdit* lnSearch;
    QPushButton* btnSearch;
    std::shared_ptr<AudioPlayer> player;

    QString songPath;
};
#endif // HANIWAMUSIC_H