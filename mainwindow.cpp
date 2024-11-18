#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAudioPlayer.h>

static constexpr std::string DEFAULT_PATH = "/home";
static constexpr float DEFAULT_VOLUME = 0.5;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    centralWidget = new QWidget();
    setCentralWidget(centralWidget);
    btnPlay = new QPushButton("Play", this);
    btnStop = new QPushButton("Stop", this);
    btnNext = new QPushButton("Next", this);
    btnPrev = new QPushButton("Prev", this);
    btnOpen = new QPushButton("Open", this);
    btnOpenDir = new QPushButton("Open directory", this);
    lFileName = new QLabelElide("", this);
    //lFileName->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    sldVolume = new QSlider(Qt::Orientation::Horizontal, this);
    sldProgress = new QSlider(Qt::Orientation::Horizontal, this);
    sldProgress->setRange(1, 10000);
    sldProgress->setTickInterval(1);
    playlist = new QPlaylist();
    setWindowTitle("Haniwa Music");
    changeFileNameLabel("File not selected", Qt::red);
    loadSettings();
    configureAudio();
    configureLayout();
    connect(btnPlay, &QPushButton::released, this, &MainWindow::onStartPress);
    connect(btnStop, &QPushButton::released, this, &MainWindow::onStopPress);
    connect(btnNext, &QPushButton::released, playlist, &QPlaylist::next);
    connect(btnPrev, &QPushButton::released, playlist, &QPlaylist::prev);
    connect(btnOpen, &QPushButton::released, this, &MainWindow::onOpenPress);
    connect(btnOpenDir, &QPushButton::released, this, &MainWindow::onOpenDirPress);
    connect(sldVolume, &QAbstractSlider::valueChanged, this, &MainWindow::onVolumeSliderChanged);
    connect(sldProgress, &QAbstractSlider::sliderReleased, this, [this]() {
        player->setPosition((float)sldProgress->value() / (float)sldProgress->maximum());
    });
    connect(playlist, &QPlaylist::fileChanged, this, &MainWindow::onFileChanged);    
}


MainWindow::~MainWindow()
{
    player->stop();
    saveSettings();
    delete ui;
}

void MainWindow::configureLayout(){
    layout = new QVBoxLayout(centralWidget);
    QHBoxLayout* l1 = new QHBoxLayout();
    QHBoxLayout* l2 = new QHBoxLayout();
    l1->addWidget(btnOpen);
    l1->addWidget(btnOpenDir);
    l1->addWidget(lFileName);
    l2->addWidget(btnPlay);
    l2->addWidget(btnStop);
    l2->addWidget(btnNext);
    l2->addWidget(btnPrev);
    layout->addLayout(l1);
    layout->addLayout(l2);
    layout->addWidget(sldVolume);
    layout->addWidget(sldProgress);
    layout->addWidget(playlist);
    centralWidget->setLayout(layout);
}

void MainWindow::configureAudio() {
    player.reset(new QAudioPlayer());
    player->setOnErrorCb([this](AudioPlayer::Error error){ onAudioError(error); });
    player->setOnProgressCb([this](float pos){ if(!sldProgress->isSliderDown()) sldProgress->setValue(1 + pos * 10000); });
    player->setVolume(DEFAULT_VOLUME);
    sldVolume->setValue(DEFAULT_VOLUME * 100);
}

bool MainWindow::onFileChanged(QString newFile) {
    bool ok = checkFile(newFile);
    if (!ok) {
        return false;
    }
    onStopPress();
    songPath = newFile;
    setAudio(newFile);
    changeFileNameLabel(songPath, Qt::black);
    onStartPress();
    return ok;
}

void MainWindow::onAudioError(AudioPlayer::Error error) {
    if (error == AudioPlayer::Error::InvalidMedia) {
        changeFileNameLabel("File not selected", Qt::red);
        QMessageBox::warning(this, "Error", "Invalid media file");
    }
    else {
        QMessageBox::critical(this, "Error", "Unknown error");
    }
}

bool MainWindow::checkFile(const QString& path) {
    if (path.isEmpty()) {
        QMessageBox msg;
        msg.warning(this, "Error", "Empty file path");
        return false;
    }
    if (!QFile::exists(path)) {
        QMessageBox msg;
        msg.warning(this, "Error", "Invalid file path");
        return false;
    }
    return true;
}

bool MainWindow::checkDir(const QString& path) {
    if (path.isEmpty()) {
        QMessageBox msg;
        msg.warning(this, "Error", "Empty file path");
        return false;
    }
    if (!QFile::exists(path)) {
        QMessageBox msg;
        msg.warning(this, "Error", "Invalid file path");
        return false;
    }
    return true;
}


void MainWindow::setAudio(const QString& file) {
    player->open(file.toStdString());
    // error handling in slot;
}

void MainWindow::changeFileNameLabel(const QString& text, Qt::GlobalColor color) {
    //QFontMetrics metrics(lFileName->font());
    //QString elidedText = metrics.elidedText(text, Qt::ElideRight, lFileName->width());
    lFileName->elideText(text);
    QPalette palette = lFileName->palette();
    palette.setColor(lFileName->foregroundRole(), color);
    lFileName->setPalette(palette);
}

void MainWindow::loadSettings() {
    QSettings settings(ORGANIZATION_NAME, APP_NAME);
    lastDir = settings.value("LastDir", "/home").toString();
    QDir dir(lastDir);
    if (!dir.exists()) {
        lastDir = QString(DEFAULT_PATH.data());
    }
    QStringList lastPlaylist = settings.value("Playlist", QStringList()).toStringList();
    playlist->addFiles(lastPlaylist);
}

void MainWindow::saveSettings() {
    QSettings settings(ORGANIZATION_NAME, APP_NAME);
    settings.setValue("LastDir", lastDir);
    settings.setValue("Playlist", playlist->toStringList());
}

void MainWindow::onOpenPress() {
    QString songPath = QFileDialog::getOpenFileName(this, "Select audio file", lastDir, "Audio (*.mp3 *.wav *.flac)");
    if (checkFile(songPath)) {
        lastDir = QFileInfo(songPath).absolutePath();
        playlist->clear();
        playlist->addFile(songPath);
    }
}

void MainWindow::onOpenDirPress() {
    auto dir = QFileDialog::getExistingDirectory(this, "Open directory", lastDir);
    if (checkDir(dir)) {
        lastDir = dir;
        playlist->clear();
        playlist->addFolder(dir);
    }
}

void MainWindow::onStartPress() {
    player->playOrPause();
}

void MainWindow::onStopPress() {
    player->stop();
}

void MainWindow::onVolumeSliderChanged(int newValue) {
    player->setVolume((float)newValue / 100.0f);
}
