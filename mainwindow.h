#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPushButton>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QLabel>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QSettings>
#include <QFileInfo>
#include <QLineEdit>

#include "QLabelElide.h"
#include "QPlaylist.h"

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
    void onVolumeSliderChanged(int newValue);
    bool onFileChanged(QString newFile);
private:
    void configureLayout();
    void configureAudio();
    void changeFileNameLabel(const QString& text, Qt::GlobalColor color);
    void loadSettings();
    void saveSettings();

    Ui::MainWindow *ui;

    QWidget* centralWidget;
    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    QPushButton* btnPlay;
    QPushButton* btnStop;
    QPushButton* btnOpen;
    QLabelElide* lFileName;
    QVBoxLayout* layout;
    QSlider* sldVolume;
    QPlaylist* playlist;

    QString lastDir;
    QString songPath;
};
#endif // MAINWINDOW_H
