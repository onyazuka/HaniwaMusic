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
    btnPlay = new QMLControlButton("qrc:/icons/play1.svg", this);
    btnStop = new QMLControlButton("qrc:/icons/stop1.svg", this);
    btnNext = new QMLControlButton("qrc:/icons/next1.svg", this);
    btnPrev = new QMLControlButton("qrc:/icons/prev1.svg", this);
    chRandom = new QMLControlCheckbox("qrc:/icons/random.svg", false, this);
    chRepeat = new QMLControlCheckbox("qrc:/icons/repeat.svg", false, this);
    //qmlSlider = new QMLSlider(Qt::Orientation::Vertical, this);
    lFileName = new QLabelElide("", this);
    lFileName->setMoving(true);
    //lFileName->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    iVolume = new QMLIcon("qrc:/icons/volume.svg", this);
    sldVolume = new QMLSlider(Qt::Orientation::Vertical, this);
    sldProgress = new QMLSlider(Qt::Orientation::Horizontal, this);
    //sldProgress->setRange(1, 10000);
    //sldProgress->setTickInterval(1);
    lProgress = new QLabel("0:00", this);
    sldVolume->setMaximumHeight(sldProgress->height() * 2);
    tabPlaylists = new QTabWidgetCustom(this);
    // hide tab bar if less than 2 tabs
    tabPlaylists->setTabBarAutoHide(true);
    // setting document mode true to get rid of double border
    tabPlaylists->setDocumentMode(true);
    btnOpen = new QMLMenuButton("qrc:/icons/openFile.svg", this);
    btnOpenDir = new QMLMenuButton("qrc:/icons/openFolder.svg", this);
    btnPlaylistsMenu = new QMLMenuButton("qrc:/icons/playlist.svg", this);
    lnSearch = new QLineEdit(this);
    //lnSearch->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    btnSearch = new QMLMenuButton("qrc:/icons/search.svg",this);
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
    connect((QObject*)btnOpen->base(), SIGNAL(released()), this, SLOT(onOpenPress()));
    connect((QObject*)btnOpenDir->base(), SIGNAL(released()), this, SLOT(onOpenDirPress()));
    sldVolume->value.connectNotifySignal(this, SLOT(onVolumeSliderChanged()));
    sldProgress->pressed.connectNotifySignal(this, SLOT(updatePlayerPosition()));
    connect((QObject*)iVolume->base(), SIGNAL(released()), this, SLOT(onMute()));
    connect(currentPlaylist(), &QPlaylist::fileChanged, this, &HaniwaMusic::onFileChanged);
    connect(tabPlaylists, &QTabWidget::currentChanged, this, &HaniwaMusic::onPlaylistChange);
    connect((QObject*)btnPlaylistsMenu->base(), SIGNAL(released()), this, SLOT(onPlaylistsMenuClicked()));
    connect((QObject*)btnSearch->base(), SIGNAL(released()), this, SLOT(onSearchNext()));
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
    QVBoxLayout* l6 = new QVBoxLayout();
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
    l6->addWidget(sldVolume);
    l6->addWidget(iVolume, 0, Qt::AlignHCenter);
    l3->addLayout(l6);
    l5->addWidget(btnOpen);
    l5->addWidget(btnOpenDir);
    l5->addWidget(btnPlaylistsMenu);
    l5->addWidget(lnSearch);
    l5->addWidget(btnSearch);
    //l5->setAlignment(Qt::AlignRight);
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
            btnPlay->icon_source.write("qrc:/icons/pause1.svg");
        }
        else if (state == AudioPlayer::PlayState::Pause) {
            btnPlay->icon_source.write("qrc:/icons/play1.svg");
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

void HaniwaMusic::addPlaylist(QString title) {
    if (title.isEmpty()) {
        title = QInputDialog::getText(this, "Add new playlist", "Title: ", QLineEdit::Normal);
    }
    tabPlaylists->setCurrentIndex(tabPlaylists->addTab(new QPlaylist(tabPlaylists), title));
}

void HaniwaMusic::removePlaylist() {
    tabPlaylists->removeTab(tabPlaylists->currentIndex());
    onClose();
}

void HaniwaMusic::clearPlaylist() {
    onStopPress();
    playlist->clear();
    onClose();
}

void HaniwaMusic::importPlaylist() {
    if (!playlist) {
        return;
    }
    QString playlistPath = QFileDialog::getOpenFileName(this, "Select playlist file", appSettings.lastDir, "Playlist (*.m3u *.m3u8)");
    if (!playlistPath.isEmpty()) {
        std::ifstream ifs(playlistPath.toStdString());
        if (!ifs) {
            QMessageBox msg;
            msg.warning(this, "Error", "Couldn't import playlist");
            return;
        }
        m3u::M3UPlaylist playlist = m3u::M3UPlaylist::fromStream(ifs);
        std::string stitle = playlist.title();
        QString title = QString::fromStdString(stitle);
        if (title.isEmpty()) {
            title = QFileInfo(playlistPath).baseName();
        }
        addPlaylist(title);
        this->playlist->addFilesFromM3uPlaylist(playlist);
    }
}

void HaniwaMusic::exportPlaylist() {
    if (!playlist || playlist->empty()) {
        return;
    }
    QString playlistPath = QFileDialog::getSaveFileName(this, "Select playlist file", appSettings.lastDir, "Playlist (*.m3u *.m3u8)");
    if (!playlistPath.endsWith(".m3u") && !playlistPath.endsWith(".m3u8")) {
        playlistPath += ".m3u";
    }
    if (!playlistPath.isEmpty()) {
        if (!playlist->exportTo(tabPlaylists->tabText(tabPlaylists->currentIndex()), playlistPath)) {
            QMessageBox msg;
            msg.warning(this, "Error", "Couldn't export playlist");
        }
    }
}

void HaniwaMusic::onMute() {
    auto oldVal = player->volume();
    if (oldVal) {
        memoVolume = oldVal;
        player->setVolume(0);
        sldVolume->value.write(0.0f);
        iVolume->icon_source.write("qrc:/icons/mute.svg");
    }
    else {
        memoVolume = memoVolume ? memoVolume : 0.5f;
        player->setVolume(memoVolume);
        sldVolume->value.write(memoVolume);
        iVolume->icon_source.write("qrc:/icons/volume.svg");
    }
}

void HaniwaMusic::onVolumeSliderChanged() {
    auto newVal = sldVolume->value.read().toFloat();
    auto oldVal = player->volume();
    player->setVolume(newVal);
    if (newVal && !oldVal) {
        iVolume->icon_source.write("qrc:/icons/volume.svg");
    }
    else if (!newVal && oldVal) {
        iVolume->icon_source.write("qrc:/icons/mute.svg");
    }
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
    QAction* playlistsImportPlaylistAction = new QAction("Import playlist", this);
    QAction* playlistsExportPlaylistAction = new QAction("Export playlist", this);
    playlistsMenuActions.push_back(playlistsAddPlaylistAction);
    playlistsMenuActions.push_back(playlistsRemovePlaylistAction);
    playlistsMenuActions.push_back(playlistsClearPlaylistAction);
    playlistsMenuActions.push_back(playlistsImportPlaylistAction);
    playlistsMenuActions.push_back(playlistsExportPlaylistAction);
    connect(playlistsAddPlaylistAction, &QAction::triggered, this, [this](){
        addPlaylist();
    });
    connect(playlistsRemovePlaylistAction, &QAction::triggered, this, [this](){
        removePlaylist();
    });
    connect(playlistsClearPlaylistAction, &QAction::triggered, this, [this](){
        clearPlaylist();
    });
    connect(playlistsImportPlaylistAction, &QAction::triggered, this, [this](){
        importPlaylist();
    });
    connect(playlistsExportPlaylistAction, &QAction::triggered, this, [this](){
        exportPlaylist();
    });
    playlistsMenu->addAction(playlistsAddPlaylistAction);
    playlistsMenu->addAction(playlistsRemovePlaylistAction);
    playlistsMenu->addAction(playlistsClearPlaylistAction);
    playlistsMenu->addSeparator();
    playlistsMenu->addAction(playlistsImportPlaylistAction);
    playlistsMenu->addAction(playlistsExportPlaylistAction);
}
