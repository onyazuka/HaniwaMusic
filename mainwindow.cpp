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
    setWindowTitle("Hello Qt");
    changeFileNameLabel("File not selected", Qt::red);
    loadSettings();
    configureAudio();
    configureLayout();
    connect(btnPlay, &QPushButton::released, this, &MainWindow::onStartPress);
    connect(btnStop, &QPushButton::released, this, &MainWindow::onStopPress);
    connect(btnOpen, &QPushButton::released, this, &MainWindow::onOpenPress);
    connect(sldVolume, &QAbstractSlider::valueChanged, this, &MainWindow::onVolumeSliderChanged);
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
    centralWidget->setLayout(layout);
}

void MainWindow::configureAudio() {
    //songPath = "/media/onyazuka/New SSD/music/虹のコンキスタドール/HIGH FLYER.mp3";
    player = new QMediaPlayer;
    audioOutput = new QAudioOutput;
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(DEFAULT_VOLUME);
    sldVolume->setValue(DEFAULT_VOLUME * 100);
    //player->setSource(QUrl::fromLocalFile(songPath));
}

bool MainWindow::onFileChanged(const QString& newFile) {
    if (newFile.isEmpty()) {
        QMessageBox msg;
        msg.warning(this, "Error", "Empty file name");
        return false;
    }
    else if (!QFile::exists(newFile)) {
        QMessageBox msg;
        msg.warning(this, "Error", "Invalid file name");
        return false;
    }
    player->setSource(QUrl::fromLocalFile(newFile));
    if (player->error()) {
        QMessageBox msg;
        msg.warning(this, "Error", "Invalid or unsupported audio file");
        return false;
    }
    return true;
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
    songPath = QFileDialog::getOpenFileName(this, "Select audio file", lastDir, "Audio (*.mp3 *.wav *.flac)");
    if (onFileChanged(songPath)) {
        changeFileNameLabel(songPath, Qt::black);
        lastDir = QFileInfo(songPath).absolutePath();
    }
    else {
        songPath = "";
        changeFileNameLabel("File not selected", Qt::red);
    }
}

void MainWindow::onStartPress() {
    if (!player->hasAudio()) {
        return;
    }
    if (player->isPlaying()) {
        player->pause();
    }
    else {
        player->play();
    }
}

void MainWindow::onStopPress() {
    if (!player->hasAudio()) {
        return;
    }
    player->stop();
}

void MainWindow::onVolumeSliderChanged(int newValue) {
    audioOutput->setVolume((float)newValue / 100.0f);
}
