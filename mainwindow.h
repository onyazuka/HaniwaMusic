#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onStartPress();
    void onStopPress();
    void onOpenPress();
    void onOpenDirPress();
    void onVolumeSliderChanged(int newValue);
    bool onFileChanged(QString newFile);
    void onAudioError(AudioPlayer::Error error);
private:
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
    void onNext();
    void onPrev();

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
    std::shared_ptr<AudioPlayer> player;

    QString songPath;
};
#endif // MAINWINDOW_H
