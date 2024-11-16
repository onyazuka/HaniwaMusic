#include "mainwindow.h"
#include "ui_mainwindow.h"

static constexpr float DEFAULT_VOLUME = 0.5;
static constexpr std::string DEFAULT_PATH = "/home";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    centralWidget = new QWidget();
    setCentralWidget(centralWidget);
    btnPlay = new QPushButton("Play", this);
    btnStop = new QPushButton("Stop", this);
    btnOpen = new QPushButton("Open", this);
    lFileName = new QLabelElide("", this);
    //lFileName->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    sldVolume = new QSlider(Qt::Orientation::Horizontal, this);
    playlist = new QPlaylist();
    setWindowTitle("Hello Qt");
    changeFileNameLabel("File not selected", Qt::red);
    loadSettings();
    configureAudio();
    configureLayout();
    connect(btnPlay, &QPushButton::released, this, &MainWindow::onStartPress);
    connect(btnStop, &QPushButton::released, this, &MainWindow::onStopPress);
    connect(btnOpen, &QPushButton::released, this, &MainWindow::onOpenPress);
    connect(sldVolume, &QAbstractSlider::valueChanged, this, &MainWindow::onVolumeSliderChanged);
    connect(playlist, &QPlaylist::fileChanged, this, &MainWindow::onFileChanged);
    connect(player, &QMediaPlayer::mediaStatusChanged, this, &MainWindow::onMediaStatusChanged);
    connect(player, &QMediaPlayer::errorOccurred, this, &MainWindow::onMediaError);
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
    l1->addWidget(lFileName);
    l2->addWidget(btnPlay);
    l2->addWidget(btnStop);
    layout->addLayout(l1);
    layout->addLayout(l2);
    layout->addWidget(sldVolume);
    layout->addWidget(playlist);
    centralWidget->setLayout(layout);
}

void MainWindow::configureAudio() {
    player = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(DEFAULT_VOLUME);
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

void MainWindow::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    switch (status) {
    case QMediaPlayer::MediaStatus::InvalidMedia:
        changeFileNameLabel("File not selected", Qt::red);
        break;
    default:
        break;
    }
}

void MainWindow::onMediaError(QMediaPlayer::Error, const QString&) {
    changeFileNameLabel("File not selected", Qt::red);
}

bool MainWindow::checkFile(const QString& file) {
    if (file.isEmpty()) {
        QMessageBox msg;
        msg.warning(this, "Error", "Empty file name");
        return false;
    }
    else if (!QFile::exists(file)) {
        QMessageBox msg;
        msg.warning(this, "Error", "Invalid file name");
        return false;
    }
    return true;
}

void MainWindow::setAudio(const QString& file) {
    player->setSource(QUrl::fromLocalFile(file));
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
}

void MainWindow::saveSettings() {
    QSettings settings(ORGANIZATION_NAME, APP_NAME);
    settings.setValue("LastDir", lastDir);
}

void MainWindow::onOpenPress() {
    QString songPath = QFileDialog::getOpenFileName(this, "Select audio file", lastDir, "Audio (*.mp3 *.wav *.flac)");
    if (checkFile(songPath)) {
        lastDir = QFileInfo(songPath).absolutePath();
        playlist->clear();
        playlist->addFile(songPath);
    }
}

void MainWindow::onStartPress() {
    if (player->isPlaying()) {
        player->pause();
    }
    else {
        player->play();
    }
}

void MainWindow::onStopPress() {
    player->stop();
}

void MainWindow::onVolumeSliderChanged(int newValue) {
    audioOutput->setVolume((float)newValue / 100.0f);
}
