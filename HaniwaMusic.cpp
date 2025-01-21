#include "HaniwaMusic.h"
#include "ui_HaniwaMusic.h"
#include <QAudioPlayer.h>
#include <QKeyEvent>
#include <QInputDialog>

static const std::string DEFAULT_PATH = "/home";
static constexpr float DEFAULT_VOLUME = 0.5;

HaniwaMusic::HaniwaMusic(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    centralWidget = new QWidget();
    setCentralWidget(centralWidget);
    btnPlay = new QMLControlButton("⏵", this);
    btnStop = new QMLControlButton("⏹", this);
    btnNext = new QMLControlButton("⏭", this);
    btnPrev = new QMLControlButton("⏮", this);
    chRandom = new QMLControlCheckbox("qrc:/icons/random.svg", false, this);
    chRepeat = new QMLControlCheckbox("qrc:/icons/repeat.svg", false, this);
    //qmlSlider = new QMLSlider(Qt::Orientation::Vertical, this);
    btnOpen = new QPushButton("Open", this);
    btnOpenDir = new QPushButton("Open directory", this);
    lFileName = new QLabelElide("", this);
    //lFileName->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    sldVolume = new QMLSlider(Qt::Orientation::Vertical, this);
    sldProgress = new QMLSlider(Qt::Orientation::Horizontal, this);
    //sldProgress->setRange(1, 10000);
    //sldProgress->setTickInterval(1);
    lProgress = new QLabel("0:00", this);
    sldVolume->setMaximumHeight(sldProgress->height() * 3);
    tabPlaylists = new QTabWidget(this);
    // hide tab bar if less than 2 tabs
    tabPlaylists->setTabBarAutoHide(true);
    // setting document mode true to get rid of double border
    tabPlaylists->setDocumentMode(true);
    btnPlaylistsMenu = new QPushButton("Playlists", this);
    lnSearch = new QLineEdit(this);
    btnSearch = new QPushButton("Find",this);
    setWindowTitle("Haniwa Music");
    changeFileNameLabel("File not selected", Qt::red);
    loadSettings();
    if (tabPlaylists->count() == 0) {
        tabPlaylists->addTab(new QPlaylist(tabPlaylists), "Playlist");
        playlist = (QPlaylist*)tabPlaylists->widget(0);
    }
    configureAudio();
    configureLayout();
    if (!currentPlaylist()->select(appSettings.lastTrackNumber)) {
        currentPlaylist()->select(0);
    }

    initPlaylistsMenu();

    {
        QFont font = lProgress->font();
        font.setFamily("Seven Segment");
        font.setPointSize(font.pointSize() * 1.8);
        lProgress->setFont(font);
    }

    {
        QFont font = lFileName->font();
        font.setFamily("Monospace");
        font.setPointSize(font.pointSize() * 1.35);
        lFileName->setFont(font);
    }

    connect((QObject*)btnPlay->base(), SIGNAL(released()), this, SLOT(onPlayPausePress()));
    connect((QObject*)btnStop->base(), SIGNAL(released()), this, SLOT(onStopPress()));
    connect((QObject*)btnNext->base(), SIGNAL(released()), this, SLOT(onNext()));
    connect((QObject*)btnPrev->base(), SIGNAL(released()), this, SLOT(onPrev()));
    connect(btnOpen, &QPushButton::released, this, &HaniwaMusic::onOpenPress);
    connect(btnOpenDir, &QPushButton::released, this, &HaniwaMusic::onOpenDirPress);
    sldVolume->value.connectNotifySignal(this, SLOT(onVolumeSliderChanged()));
    sldProgress->pressed.connectNotifySignal(this, SLOT(updatePlayerPosition()));
    connect(currentPlaylist(), &QPlaylist::fileChanged, this, &HaniwaMusic::onFileChanged);
    connect(tabPlaylists, &QTabWidget::currentChanged, this, &HaniwaMusic::onPlaylistChange);
    connect(btnPlaylistsMenu, &QPushButton::released, this, &HaniwaMusic::onPlaylistsMenuClicked);
    connect(btnSearch, &QPushButton::released, this, &HaniwaMusic::onSearchNext);
}

QPlaylist* HaniwaMusic::currentPlaylist() {
    if (tabPlaylists->count() == 0) {
        return nullptr;
    }
    return (QPlaylist*)tabPlaylists->currentWidget();
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
    l2->addWidget(btnPlay);
    l2->addWidget(btnStop);
    l2->addWidget(btnPrev);
    l2->addWidget(btnNext);
    l2->addWidget(chRandom);
    l2->addWidget(chRepeat);
    l2->addStretch();
    l1->addWidget(lFileName);
    l4->addWidget(sldProgress);
    l4->addWidget(lProgress);
    l3->addLayout(l4);
    l3->addWidget(sldVolume);
    l5->addWidget(btnOpen, 1, Qt::AlignLeft);
    l5->addWidget(btnOpenDir, 1, Qt::AlignLeft);
    l5->addWidget(btnPlaylistsMenu, 1, Qt::AlignLeft);
    l5->addWidget(lnSearch, 10, Qt::AlignRight);
    l5->addWidget(btnSearch, 1, Qt::AlignRight);
    l5->setAlignment(Qt::AlignRight);
    layout->addLayout(l1);
    layout->addLayout(l3);
    layout->addLayout(l2);
    layout->addSpacing(5);
    layout->addWidget(tabPlaylists);
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
        if (!sldProgress->pressed.read().toBool()) {
            //sldProgress->setValue(1 + pos * 10000);
            sldProgress->value.write(pos);
        }
        lProgress->setText(QTime(0,0,0).addMSecs(pos * player->duration()).toString("m:ss") + "/" + QTime(0,0,0).addMSecs(player->duration()).toString("m:ss"));
    });
    player->setOnAudioEndCb([this](){ onNext(); });
    player->setOnPlayStateChangeCb([this](AudioPlayer::PlayState state){
        if (state == AudioPlayer::PlayState::Play) {
            btnPlay->text.write("⏸");
        }
        else if (state == AudioPlayer::PlayState::Pause) {
            btnPlay->text.write("⏵");
        }
    });
    player->setVolume(appSettings.volume);
    sldVolume->value.write(appSettings.volume);
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
    QJsonArray playlists = settings.value("Playlists", QJsonArray()).toJsonArray();
    for (const auto& item : playlists) {
        QJsonObject oItem = item.toObject();
        QString playlistTitle = oItem.value("Title").toString("");
        QJsonArray playlist = oItem.value("Playlist").toArray({});
        QPlaylist* newPlaylist = new QPlaylist(tabPlaylists);
        tabPlaylists->addTab(newPlaylist, playlistTitle);
        newPlaylist->addFilesFromJson(playlist);
    }
    //QJsonArray lastPlaylist = settings.value("Playlist", QJsonArray()).toJsonArray();
    //playlist->addFilesFromJson(lastPlaylist);
    if (tabPlaylists->count()) {
        int idx = settings.value("LastPlaylist", 0).toInt();
        tabPlaylists->setCurrentIndex(idx);
        playlist = (QPlaylist*)tabPlaylists->widget(idx);
    }
    appSettings.volume = settings.value("Volume", DEFAULT_VOLUME).toFloat();
    appSettings.windowRect = settings.value("WindowRect", QRect()).toRect();
    appSettings.lastTrackNumber = settings.value("LastTrackNumber", -1).toInt();
    chRandom->checked.write(settings.value("Random", false).toBool());
    chRepeat->checked.write(settings.value("Repeat", false).toBool());
}

void HaniwaMusic::saveSettings() {
    QSettings settings(ORGANIZATION_NAME, APP_NAME);
    settings.setValue("LastDir", appSettings.lastDir);
    QJsonArray jPlaylists;
    for (int i = 0; i < tabPlaylists->count(); ++i) {
        QJsonObject jPlaylist;
        jPlaylist.insert("Title", tabPlaylists->tabText(i));
        QPlaylist* curPlaylist = (QPlaylist*)tabPlaylists->widget(i);
        jPlaylist.insert("Playlist", curPlaylist->toJson());
        jPlaylists.push_back(jPlaylist);
    }
    settings.setValue("Playlists", jPlaylists);
    if (playlist) {
        settings.setValue("LastTrackNumber", playlist->activeRowNumber());
        settings.setValue("LastPlaylist", tabPlaylists->currentIndex());
    }
    settings.setValue("Volume", sldVolume->value.read() );
    settings.setValue("WindowRect", geometry());
    settings.setValue("Random", chRandom->checked.read());
    settings.setValue("Repeat", chRepeat->checked.read());
}

void HaniwaMusic::onNext() {
    if (!playlist) {
        return;
    }
    if (chRepeat->checked.read().toBool()) {
        player->stop();
        player->play();
    }
    else if (chRandom->checked.read().toBool()) {
        playlist->nextRandom();
    }
    else {
        playlist->next();
    }
}

void HaniwaMusic::onPrev() {
    if (!playlist) {
        return;
    }
    if (chRandom->checked.read().toBool()) {
        playlist->prevRandom();
    }
    else {
        playlist->prev();
    }
}

void HaniwaMusic::onSearchNext() {
    if (!playlist) {
        return;
    }
    playlist->findNext(lnSearch->text());
}

void HaniwaMusic::onOpenPress() {
    if (!playlist) {
        return;
    }
    QString songPath = QFileDialog::getOpenFileName(this, "Select audio file", appSettings.lastDir, "Audio (*.mp3 *.flac)");
    if (checkFile(songPath)) {
        appSettings.lastDir = QFileInfo(songPath).absolutePath();
        playlist->clear();
        playlist->addFile(songPath);
    }
}

void HaniwaMusic::onOpenDirPress() {
    if (!playlist) {
        return;
    }
    auto dir = QFileDialog::getExistingDirectory(this, "Open directory", appSettings.lastDir);
    if (checkDir(dir)) {
        appSettings.lastDir = dir;
        playlist->clear();
        playlist->addFolder(dir);
    }
}

void HaniwaMusic::onPlaylistChange(int n) {
    if (playlist) {
        disconnect(playlist, &QPlaylist::fileChanged, this, &HaniwaMusic::onFileChanged);
    }
    if (n >= 0) {
        playlist = (QPlaylist*)tabPlaylists->widget(n);
        connect(playlist, &QPlaylist::fileChanged, this, &HaniwaMusic::onFileChanged);
    }
    else {
        playlist = nullptr;
    }
}

void HaniwaMusic::onPlayPausePress() {
    if (!playlist) {
        return;
    }
    playlist->current();
    player->playOrPause();
}

void HaniwaMusic::onPlayPress() {
    if (!playlist) {
        return;
    }
    playlist->current();
    player->play();
}

void HaniwaMusic::onPausePress() {
    player->pause();
}

void HaniwaMusic::onStopPress() {
    player->stop();
}

void HaniwaMusic::onClose() {
    player->stop();
    changeFileNameLabel("File not selected", Qt::red);
}

void HaniwaMusic::onVolumeSliderChanged() {
    player->setVolume(sldVolume->value.read().toFloat());
}

void HaniwaMusic::onVolumeSliderChanged(int newValue) {
    player->setVolume((float)newValue / 100.0f);
}

void HaniwaMusic::updatePlayerPosition() {
    // on released
    if (!sldProgress->pressed.read().toBool()) {
        player->setPosition(sldProgress->value.read().toFloat());
    }
}

void HaniwaMusic::onPlaylistsMenuClicked() {
    playlistsMenu->exec(cursor().pos());
}

void HaniwaMusic::initPlaylistsMenu() {
    playlistsMenu = new QMenu();
    QAction* playlistsAddPlaylistAction = new QAction("Add playlist", this);
    QAction* playlistsRemovePlaylistAction = new QAction("Remove playlist", this);
    QAction* playlistsClearPlaylistAction = new QAction("Clear playlist", this);
    playlistsMenuActions.push_back(playlistsAddPlaylistAction);
    playlistsMenuActions.push_back(playlistsRemovePlaylistAction);
    playlistsMenuActions.push_back(playlistsClearPlaylistAction);
    connect(playlistsAddPlaylistAction, &QAction::triggered, this, [this](){
        QString title = QInputDialog::getText(this, "Add new playlist", "Title: ", QLineEdit::Normal);
        tabPlaylists->setCurrentIndex(tabPlaylists->addTab(new QPlaylist(tabPlaylists), title));
    });
    connect(playlistsRemovePlaylistAction, &QAction::triggered, this, [this](){
        tabPlaylists->removeTab(tabPlaylists->currentIndex());
        onClose();
    });
    connect(playlistsClearPlaylistAction, &QAction::triggered, this, [this](){
        onStopPress();
        playlist->clear();
        onClose();
    });
    playlistsMenu->addAction(playlistsAddPlaylistAction);
    playlistsMenu->addAction(playlistsRemovePlaylistAction);
    playlistsMenu->addAction(playlistsClearPlaylistAction);
}
