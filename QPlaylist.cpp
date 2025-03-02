#include "QPlaylist.h"
#include <QResizeEvent>
#include <QMenu>
#include <QDirIterator>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include "MetaTagsParser/TagScout.hpp"
#include "QMetadataDlg.hpp"


DurationGatherer2::DurationGatherer2() {
    ;
}

DurationGatherer2::~DurationGatherer2() {
    ;
}

void DurationGatherer2::onAddFile(const QString& path, QTableWidgetItem* it) {
    //taskQueue.append({path,row,0});
    //cnd.notify_one();
    static GetMetaInfoConfig config;
    config.textual = true;
    config.duration = true;
    config.images = false;
    Metainfo mt;
    try {
        auto metainfo = getMetainfo(path.toStdString(), config);
        mt.durationMs = (qint64)(QString::fromStdString(std::get<std::string>(metainfo["durationMs"])).toULongLong());
        if (mt.durationMs < 0) {
            mt.durationMs = 0;
        }
        if (auto iter = metainfo.find("title"); iter != metainfo.end()) {
            mt.title = QString::fromStdString(std::get<std::string>(metainfo["title"]));
        }
        if (auto iter = metainfo.find("artist"); iter != metainfo.end()) {
            mt.artist = QString::fromStdString(std::get<std::string>(metainfo["artist"]));
        }
        emit gotMetainfo(mt, it);
    }
    catch (...) {
        emit gotMetainfo(mt, it);
    }
}

QPlaylist::QPlaylist(QWidget* parent)
    : QPlaylistView(parent)
{
    initMenu();
    durationGatherer = new DurationGatherer2();
    durationGatherer->moveToThread(&durationGathererThread);
    connect(this, &QTableWidget::cellDoubleClicked, this, &QPlaylist::onCellDoubleClicked);
    connect(&durationGathererThread, &QThread::finished, durationGatherer, &QObject::deleteLater);
    connect(this, &QPlaylist::fileAddedWithUnknownDuration, durationGatherer, &DurationGatherer2::onAddFile);
    connect(durationGatherer, &DurationGatherer2::gotMetainfo, this, &QPlaylist::onUpdateMetainfo);
    connect(this, &QWidget::customContextMenuRequested, this, &QPlaylist::handleContextMenu);
    durationGathererThread.start();
}

QPlaylist::~QPlaylist() {
    durationGathererThread.quit();
    durationGathererThread.wait();
}

// 'duration' = 0 means unknown duration AFTER reading, -1 - default arg
void QPlaylist::addFile(const QString& path, const Metainfo& metainfo) {
    int row = appendTrack(Track(path, metainfo));
    // unknown duration - sending to duration gatherer
    if (metainfo.durationMs < 0) {
        emit fileAddedWithUnknownDuration(path, item(row, Column::Title));
    }
}

void QPlaylist::addFileFromJson(const QJsonObject& obj) {
    QString path = obj.value("path").toString();
    int durationMs = obj.value("duration").toInt();
    QString title = obj.value("title").toString();
    QString artist = obj.value("artist").toString();
    addFile(path, Metainfo(title, artist, durationMs));
}

void QPlaylist::addFiles(const QStringList& files) {
    for (const auto& file : files) {
        addFile(file);
    }
}

void QPlaylist::addFolder(const QString& path) {
    QDirIterator it(path, QStringList({"*.mp3", "*.flac", "*.wav"}), QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        addFile(it.nextFileInfo().absoluteFilePath());
    }
}

/*
    [
        {"path": "...", "duration": "..."},
        ...
    ]
*/
void QPlaylist::addFilesFromJson(const QJsonArray& json) {
    for (auto obj: json) {
        addFileFromJson(obj.toObject());
    }
}

void QPlaylist::addFilesFromM3uPlaylist(const m3u::M3UPlaylist& playlist) {
    const auto& entries = playlist.entries();
    for (const auto& entry: entries) {
        const std::string& path = entry.path();
        size_t duration = entry.duration() * 1000;
        QString title = QString::fromStdString(entry.title());
        QString artist = QString::fromStdString(entry.artist());
        addFile(QString::fromStdString(path), Metainfo(title, artist, duration));
    }
}

QStringList QPlaylist::toStringList() const {
    QStringList res;
    for(int i = 0; i < rowCount(); ++i) {
        Track tr = track(i);
        res.append(tr.path);
    }
    return res;
}

QJsonArray QPlaylist::toJson() const {
    QJsonArray jArr;
    for(int i = 0; i < rowCount(); ++i) {
        Track tr = track(i);
        jArr.append(QJsonObject({
            {"path", tr.path},
            {"duration", tr.metainfo.durationMs},
            {"title", tr.metainfo.title},
            {"artist", tr.metainfo.artist}
        }));
    }
    return jArr;
}

m3u::M3UPlaylist QPlaylist::toM3UPlaylist(const QString& title) const {
    m3u::M3UWriter writer;
    writer.writeParam({"PLAYLIST", title.toStdString()});
    for(int i = 0; i < rowCount(); ++i) {
        Track tr = track(i);
        writer.writeExtinf(std::pair<size_t, std::string>{(size_t)tr.metainfo.durationMs / 1000, tr.metainfo.title.toStdString()});
        writer.writeParam(std::pair<std::string, std::string>{"EXTART", tr.metainfo.artist.toStdString()});
        writer.writePath(tr.path.toStdString());
    }
    //writer.dumpToFile("/home/onyazuka/playlist.m3u");
    //return "";
    return writer.playlist();
}

bool QPlaylist::exportTo(const QString& title, const QString& path) {
    auto m3uPlaylist = toM3UPlaylist(title);
    std::ofstream ofs(path.toStdString());
    if (!ofs) {
        return false;
    }
    m3uPlaylist.dump(ofs);
    return true;
}

bool QPlaylist::importFrom(const QString& path) {
    std::ifstream ifs(path.toStdString());
    if (!ifs) {
        return false;
    }
    m3u::M3UPlaylist playlist = m3u::M3UPlaylist::fromStream(ifs);
    addFilesFromM3uPlaylist(playlist);
    return true;
}

bool QPlaylist::findNext(QString str) {
    str = str.toLower();
    if (str.isEmpty()) {
        return false;
    }
    if ((str != searchCtx.lastStr) || searchCtx.finish) {
        searchCtx = SearchCtx();
        searchCtx.lastStr = str;
    }
    if ((searchCtx.lastRow + 1) >= rowCount()) {
        searchCtx.lastRow = 0;
    }
    for (int i = searchCtx.lastRow + 1; i < rowCount(); ++i) {
        auto it = item(i, Column::Title);
        if (it->text().toLower().contains(str)) {
            searchCtx.lastRow = i;
            selectRow(i);
            return true;
        }
    }
    searchCtx.finish = true;
    return false;
}

/*
    selects current row as active item and plays it
*/
void QPlaylist::current() {
    if (empty()) {
        return;
    }
    if (activeItem()) {
        return;
    }
    onCellDoubleClicked(currentRow(), Column::Title);
}

void QPlaylist::next() {
    if (empty()) {
        return;
    }
    onNext();
    int row = nextRow(activeRowNumber());
    if (!playQueue.empty()) {
        row = playQueue.front()->row();
        updatePlayQueueItems();
    }
    onCellDoubleClicked(row, Column::Title);
}

void QPlaylist::nextRandom() {
    if (empty()) {
        return;
    }
    onNext();
    int row = rand() % rowCount();
    if (!playQueue.empty()) {
        row = playQueue.front()->row();
        updatePlayQueueItems();
    }
    onCellDoubleClicked(row, Column::Title);
}

void QPlaylist::onNext() {
    while ((size_t)playHistory.size() >= MaxRandomHistorySize) {
        playHistory.pop_back();
    }
    int activeRow = activeRowNumber();
    if (activeRow >= 0) {
        playHistory.push_back(activeItem());
    }
}

void QPlaylist::onAddToPlayQueue() {
    playQueue.append(currentItem());
    int row = currentRow();
    auto metainfo = getMetainfo(row);
    setTitle(currentRow(), getPath(row), metainfo.title, metainfo.artist, playQueue.size());
}

void QPlaylist::prev() {
    if (empty()) {
        return;
    }
    onCellDoubleClicked(prevRow(activeRowNumber()), Column::Title);
}

void QPlaylist::prevRandom() {
    if (empty()) {
        return;
    }
    while (!playHistory.empty()) {
        QTableWidgetItem* it = playHistory.back();
        playHistory.pop_back();
        if (it) {
            onCellDoubleClicked(it->row(), Column::Title);
            return;;
        }
    }
    prev();
}

bool QPlaylist::select(int row) {
    if (row < 0 || row >= rowCount()) {
        return false;
    }
    setCurrentCell(row, Column::Title);
    return true;
}

bool QPlaylist::empty() const {
    return rowCount() == 0;
}

void QPlaylist::onCellDoubleClicked(int row, int col) {
    if (row < 0 || row >= rowCount()) {
        return;
    }
    QPlaylistView::onCellDoubleClicked(row, col);
    Track tr = track(row);
    emit fileChanged(tr.path);
}

// got 'duration' in ms
void QPlaylist::onUpdateMetainfo(Metainfo metainfo, QTableWidgetItem* it) {
    if (!it) {
        return;
    }
    setDuration(it->row(), metainfo.durationMs);
    setTitle(it->row(), getPath(it->row()), metainfo.title, metainfo.artist);
    setMetainfo(it->row(), metainfo);
}

void QPlaylist::handleContextMenu(const QPoint& pos) {
    QTableWidgetItem *item = itemAt(pos);
    if (item) {
        // do what you want with the item.
    }
}

void QPlaylist::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MouseButton::RightButton) {
        QTableWidgetItem* it = itemAt(event->position().toPoint());
        if (it) {
            int row = it->row();
            selectRow(row);
            itemRemoveAction->setData(row);
            itemRightClickMenu->exec(event->globalPosition().toPoint());
        }
    }
    QPlaylistView::mousePressEvent(event);
}

void QPlaylist::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return) {
        onCellDoubleClicked(currentRow(), Column::Title);
    }
    else if (event->key() == Qt::Key_Delete) {
        itemRemoveAction->setData(currentRow());
        itemRemoveAction->trigger();
    }
    else if (event->key() == Qt::Key_Q) {
        onAddToPlayQueue();
    }
    else {
        QTableWidget::keyPressEvent(event);
    }
}

void QPlaylist::initMenu() {
    itemRemoveAction = new QAction(tr("Remove from playlist"), this);
    itemShowMetadata = new QAction(tr("Show metadata"), this);
    itemAddToPlayQueue = new QAction(tr("Add to queue"), this);
    itemAddToPlayQueue->setShortcut(QKeySequence(Qt::Key_Q));
    itemRemoveAction->setShortcut(QKeySequence(Qt::Key_Delete));
    connect(itemRemoveAction, &QAction::triggered, this, [this](){
        int row = itemRemoveAction->data().toInt();
        if (getDuration(row) == -1) {
            return;
        }
        onRemoveTableWidgetItem(row);
        removeRow(row);
    });
    connect(itemShowMetadata, &QAction::triggered, this, [this](){
        QMetadataDlg dlg(getPath(currentRow()), this);
        dlg.exec();
    });
    connect(itemAddToPlayQueue, &QAction::triggered, this, [this](){
        onAddToPlayQueue();
    });
    itemRightClickMenu = new QMenu();
    itemRightClickMenu->addAction(itemShowMetadata);
    itemRightClickMenu->addAction(itemAddToPlayQueue);
    itemRightClickMenu->addSeparator();
    itemRightClickMenu->addAction(itemRemoveAction);
    //itemRightClickMenu->platformMenu()
}

/*
    Cleaning up on item remove
    (activeItem, history, ...)
*/
void QPlaylist::onRemoveTableWidgetItem(int row) {
    for (int i = 0; i < playHistory.size(); ++i) {
        if (!playHistory[i] || row == playHistory[i]->row()) {
            playHistory.erase(playHistory.begin() + i);
        }
    }
    for (int i = 0; i < playQueue.size(); ++i) {
        if (!playQueue[i] || row == playQueue[i]->row()) {
            playQueue.erase(playQueue.begin() + i);
        }
    }
    for (int i = row + 1; i < rowCount(); ++i) {
        item(i, Column::Number)->setText(QString::number(i));
    }
    // setting activeItem AFTER history, because else we could get nullptr dereference attempt
    if (activeRowNumber() == row) {
        resetActiveItem();
    }
}

void QPlaylist::updatePlayQueueItems() {
    int i = 0;
    for (auto iter = playQueue.begin(); iter < playQueue.end(); ++iter, ++i) {
        QTableWidgetItem* item = *iter;
        int row = item->row();
        auto metainfo = getMetainfo(row);
        setTitle(row, getPath(row), metainfo.title, metainfo.artist, i);
        //item->setText(QString("%2 [%1]").arg(QString::number(i), item->text()));
    }
    playQueue.pop_front();
}

