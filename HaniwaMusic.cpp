#include "HaniwaMusic.h"
#include "ui_HaniwaMusic.h"
#include <QAudioPlayer.h>
#include <QKeyEvent>

static const std::string DEFAULT_PATH = "/home";
static constexpr float DEFAULT_VOLUME = 0.5;

HaniwaMusic::HaniwaMusic(QWidget *parent)
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
    lnSearch = new QLineEdit(this);
    btnSearch = new QPushButton("Find",this);
    setWindowTitle("Haniwa Music");
    changeFileNameLabel("File not selected", Qt::red);
    loadSettings();
    configureAudio();
    configureLayout();
    playlist->select(appSettings.lastTrackNumber);
    connect(btnPlay, &QPushButton::released, this, &HaniwaMusic::onPlayPausePress);
    connect(btnStop, &QPushButton::released, this, &HaniwaMusic::onStopPress);
    connect(btnNext, &QPushButton::released, this, &HaniwaMusic::onNext);
    connect(btnPrev, &QPushButton::released, this, &HaniwaMusic::onPrev);
    connect(btnOpen, &QPushButton::released, this, &HaniwaMusic::onOpenPress);
    connect(btnOpenDir, &QPushButton::released, this, &HaniwaMusic::onOpenDirPress);
    connect(sldVolume, &QAbstractSlider::valueChanged, this, &HaniwaMusic::onVolumeSliderChanged);
    connect(sldProgress, &QClickableSlider::sliderReleased, this, [this]() {
        player->setPosition((float)sldProgress->value() / ((float)sldProgress->maximum() - (float)sldProgress->minimum()));
    });
    connect(playlist, &QPlaylist::fileChanged, this, &HaniwaMusic::onFileChanged);
    connect(btnSearch, &QPushButton::released, this, &HaniwaMusic::onSearchNext);
}


HaniwaMusic::~HaniwaMusic()
{
    player->stop();
    saveSettings();
    delete ui;
}

void HaniwaMusic::configureLayout(){
    layout = new QVBoxLayout(centralWidget);
    QHBoxLayout* l1 = new QHBoxLayout();
    QHBoxLayout* l2 = new QHBoxLayout();
    QHBoxLayout* l3 = new QHBoxLayout();
    QVBoxLayout* l4 = new QVBoxLayout();
    QHBoxLayout* l5 = new QHBoxLayout();
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
    l5->addWidget(lnSearch, 1, Qt::AlignRight);
    l5->addWidget(btnSearch);
    l5->setAlignment(Qt::AlignRight);
    layout->addLayout(l1);
    layout->addLayout(l2);
    layout->addLayout(l3);
    layout->addWidget(playlist);
    layout->addLayout(l5);
    centralWidget->setLayout(layout);
    if (!appSettings.windowRect.isEmpty()) {
        setGeometry(appSettings.windowRect);
    }
}

void HaniwaMusic::configureAudio() {
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

bool HaniwaMusic::onFileChanged(QString newFile) {
    bool ok = checkFile(newFile);
    if (!ok) {
        return false;
    }
    onStopPress();
    setAudio(newFile);

    changeFileNameLabel(newFile, Qt::black);
    onPlayPausePress();
    return ok;
}

void HaniwaMusic::onAudioError(AudioPlayer::Error error) {
    if (error == AudioPlayer::Error::InvalidMedia) {
        changeFileNameLabel("File not selected", Qt::red);
        QMessageBox::warning(this, "Error", "Invalid media file");
    }
    else {
        QMessageBox::critical(this, "Error", "Unknown error");
    }
}

bool HaniwaMusic::checkFile(const QString& path) {
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

bool HaniwaMusic::checkDir(const QString& path) {
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


void HaniwaMusic::setAudio(const QString& file) {
    player->open(file.toStdString());
    // error handling in slot;
}

void HaniwaMusic::changeFileNameLabel(const QString& text, Qt::GlobalColor color) {
    //QFontMetrics metrics(lFileName->font());
    //QString elidedText = metrics.elidedText(text, Qt::ElideRight, lFileName->width());
    lFileName->elideText(QFileInfo(text).completeBaseName());
    QPalette palette = lFileName->palette();
    palette.setColor(lFileName->foregroundRole(), color);
    lFileName->setPalette(palette);
}

void HaniwaMusic::loadSettings() {
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

void HaniwaMusic::saveSettings() {
    QSettings settings(ORGANIZATION_NAME, APP_NAME);
    settings.setValue("LastDir", appSettings.lastDir);
    settings.setValue("Playlist", playlist->toJson());
    settings.setValue("Volume", (float)sldVolume->value() / 100.0f);
    settings.setValue("WindowRect", geometry());
    settings.setValue("LastTrackNumber", playlist->currentTrackNumber());
    settings.setValue("Random", chRandom->isChecked());
    settings.setValue("Repeat", chRepeat->isChecked());
}

void HaniwaMusic::onNext() {
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

void HaniwaMusic::onPrev() {
    if (chRandom->isChecked()) {
        playlist->prevRandom();
    }
    else {
        playlist->prev();
    }
}

void HaniwaMusic::onSearchNext() {
    playlist->findNext(lnSearch->text());
}

void HaniwaMusic::onOpenPress() {
    QString songPath = QFileDialog::getOpenFileName(this, "Select audio file", appSettings.lastDir, "Audio (*.mp3 *.flac)");
    if (checkFile(songPath)) {
        appSettings.lastDir = QFileInfo(songPath).absolutePath();
        playlist->clear();
        playlist->addFile(songPath);
    }
}

void HaniwaMusic::onOpenDirPress() {
    auto dir = QFileDialog::getExistingDirectory(this, "Open directory", appSettings.lastDir);
    if (checkDir(dir)) {
        appSettings.lastDir = dir;
        playlist->clear();
        playlist->addFolder(dir);
    }
}

void HaniwaMusic::onPlayPausePress() {
    playlist->current();
    player->playOrPause();
}

void HaniwaMusic::onPlayPress() {
    playlist->current();
    player->play();
}

void HaniwaMusic::onPausePress() {
    player->pause();
}

void HaniwaMusic::onStopPress() {
    player->stop();
}

void HaniwaMusic::onVolumeSliderChanged(int newValue) {
    player->setVolume((float)newValue / 100.0f);
}
