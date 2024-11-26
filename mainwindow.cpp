#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAudioPlayer.h>
#include <QKeyEvent>

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
    chRandom = new QCheckBox("Random", this);
    chRepeat = new QCheckBox("Repeat", this);
    btnOpen = new QPushButton("Open", this);
    btnOpenDir = new QPushButton("Open directory", this);
    lFileName = new QLabelElide("", this);
    //lFileName->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    sldVolume = new QSlider(Qt::Orientation::Vertical, this);
    sldProgress = new QClickableSlider(Qt::Orientation::Horizontal, this);
    sldProgress->setRange(1, 10000);
    sldProgress->setTickInterval(1);
    lProgress = new QLabel("0:00", this);
    sldVolume->setMaximumHeight(sldProgress->height() * 2);
    playlist = new QPlaylist(this);
    setWindowTitle("Haniwa Music");
    changeFileNameLabel("File not selected", Qt::red);
    loadSettings();
    configureAudio();
    configureLayout();
    playlist->select(appSettings.lastTrackNumber);
    connect(btnPlay, &QPushButton::released, this, &MainWindow::onStartPress);
    connect(btnStop, &QPushButton::released, this, &MainWindow::onStopPress);
    connect(btnNext, &QPushButton::released, this, &MainWindow::onNext);
    connect(btnPrev, &QPushButton::released, playlist, &QPlaylist::prev);
    connect(btnOpen, &QPushButton::released, this, &MainWindow::onOpenPress);
    connect(btnOpenDir, &QPushButton::released, this, &MainWindow::onOpenDirPress);
    connect(sldVolume, &QAbstractSlider::valueChanged, this, &MainWindow::onVolumeSliderChanged);
    connect(sldProgress, &QClickableSlider::sliderReleased, this, [this]() {
        player->setPosition((float)sldProgress->value() / ((float)sldProgress->maximum() - (float)sldProgress->minimum()));
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
    QHBoxLayout* l3 = new QHBoxLayout();
    QVBoxLayout* l4 = new QVBoxLayout();
    l1->addWidget(btnOpen);
    l1->addWidget(btnOpenDir);
    l1->addWidget(lFileName);
    l2->addWidget(btnPlay);
    l2->addWidget(btnStop);
    l2->addWidget(btnPrev);
    l2->addWidget(btnNext);
    l2->addWidget(chRandom);
    l2->addWidget(chRepeat);
    l4->addWidget(sldProgress);
    l4->addWidget(lProgress);
    l3->addLayout(l4);
    l3->addWidget(sldVolume);
    layout->addLayout(l1);
    layout->addLayout(l2);
    layout->addLayout(l3);
    layout->addWidget(playlist);
    centralWidget->setLayout(layout);
    if (!appSettings.windowRect.isEmpty()) {
        setGeometry(appSettings.windowRect);
    }
}

void MainWindow::configureAudio() {
    player.reset(new QAudioPlayer());
    player->init();
    player->setOnErrorCb([this](AudioPlayer::Error error){ onAudioError(error); });
    player->setOnProgressCb([this](float pos){
        if (!sldProgress->isSliderDown()) {
            sldProgress->setValue(1 + pos * 10000);
        }
        lProgress->setText(QTime(0,0,0).addMSecs(pos * player->duration()).toString("m:ss") + "/" + QTime(0,0,0).addMSecs(player->duration()).toString("m:ss"));
    });
    player->setOnAudioEndCb([this](){ onNext(); });
    player->setOnPlayStateChangeCb([this](AudioPlayer::PlayState state){
        if (state == AudioPlayer::PlayState::Play) {
            btnPlay->setText("Pause");
        }
        else if (state == AudioPlayer::PlayState::Pause) {
            btnPlay->setText("Play");
        }
    });
    player->setVolume(appSettings.volume);
    sldVolume->setValue(appSettings.volume * 100);
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
    lFileName->elideText(QFileInfo(text).completeBaseName());
    QPalette palette = lFileName->palette();
    palette.setColor(lFileName->foregroundRole(), color);
    lFileName->setPalette(palette);
}

void MainWindow::loadSettings() {
    QSettings settings(ORGANIZATION_NAME, APP_NAME);
    appSettings.lastDir = settings.value("LastDir", "/home").toString();
    QDir dir(appSettings.lastDir);
    if (!dir.exists()) {
        appSettings.lastDir = QString(DEFAULT_PATH.data());
    }
    QJsonArray lastPlaylist = settings.value("Playlist", QJsonArray()).toJsonArray();
    playlist->addFilesFromJson(lastPlaylist);
    appSettings.volume = settings.value("Volume", DEFAULT_VOLUME).toFloat();
    appSettings.windowRect = settings.value("WindowRect", QRect()).toRect();
    appSettings.lastTrackNumber = settings.value("LastTrackNumber", -1).toInt();
    chRandom->setChecked(settings.value("Random", false).toBool());
    chRepeat->setChecked(settings.value("Repeat", false).toBool());
}

void MainWindow::saveSettings() {
    QSettings settings(ORGANIZATION_NAME, APP_NAME);
    settings.setValue("LastDir", appSettings.lastDir);
    settings.setValue("Playlist", playlist->toJson());
    settings.setValue("Volume", (float)sldVolume->value() / 100.0f);
    settings.setValue("WindowRect", geometry());
    settings.setValue("LastTrackNumber", playlist->currentTrackNumber());
    settings.setValue("Random", chRandom->isChecked());
    settings.setValue("Repeat", chRepeat->isChecked());
}

void MainWindow::onNext() {
    if (chRepeat->isChecked()) {
        player->stop();
        player->play();
    }
    else if (chRandom->isChecked()) {
        playlist->nextRandom();
    }
    else {
        playlist->next();
    }
}

void MainWindow::onOpenPress() {
    QString songPath = QFileDialog::getOpenFileName(this, "Select audio file", appSettings.lastDir, "Audio (*.mp3 *.wav *.flac)");
    if (checkFile(songPath)) {
        appSettings.lastDir = QFileInfo(songPath).absolutePath();
        playlist->clear();
        playlist->addFile(songPath);
    }
}

void MainWindow::onOpenDirPress() {
    auto dir = QFileDialog::getExistingDirectory(this, "Open directory", appSettings.lastDir);
    if (checkDir(dir)) {
        appSettings.lastDir = dir;
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
