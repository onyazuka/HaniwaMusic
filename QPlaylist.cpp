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

void DurationGatherer2::onAddFile(const QString& path, int row) {
    //taskQueue.append({path,row,0});
    //cnd.notify_one();
    static GetMetaInfoConfig config;
    config.textual = false;
    config.duration = true;
    config.images = false;
    try {
        auto metainfo = getMetainfo(path.toStdString(), config);
        emit gotDuration((qint64)(QString::fromStdString(std::get<std::string>(metainfo["durationMs"])).toULongLong()), row);
    }
    catch (...) {
        emit gotDuration(0, row);
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
    connect(durationGatherer, &DurationGatherer2::gotDuration, this, &QPlaylist::onUpdateDuration);
    connect(this, &QWidget::customContextMenuRequested, this, &QPlaylist::handleContextMenu);
    durationGathererThread.start();
}

QPlaylist::~QPlaylist() {
    durationGathererThread.quit();
    durationGathererThread.wait();
}

// 'duration' = 0 means unknown duration AFTER reading, -1 - default arg
void QPlaylist::addFile(const QString& path, qint64 durationMs) {
    int row = appendTrack(Track(path, durationMs));
    // unknown duration - sending to duration gatherer
    if (durationMs < 0) {
        emit fileAddedWithUnknownDuration(path, row);
    }
}

void QPlaylist::addFileFromJson(const QJsonObject& obj) {
    QString path = obj.value("path").toString();
    int duration = obj.value("duration").toInt();
    addFile(path, duration);
}

void QPlaylist::addFiles(const QStringList& files) {
    for (const auto& file : files) {
        addFile(file);
    }
}

void QPlaylist::addFolder(const QString& path) {
    QDirIterator it(path, QStringList({"*.mp3", "*.flac"}), QDir::Files, QDirIterator::Subdirectories);
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
            {"duration", tr.durationMs}
        }));
    }
    return jArr;
}

m3u::M3UPlaylist QPlaylist::toM3UPlaylist(const QString& title) const {
    m3u::M3UWriter writer;
    writer.writeParam({"PLAYLIST", title.toStdString()});
    for(int i = 0; i < rowCount(); ++i) {
        Track tr = track(i);
        writer.writeExtinf(std::pair<size_t, std::string>{(size_t)tr.durationMs / 1000, ""});
        writer.writePath(tr.path.toStdString());
    }
    //writer.dumpToFile("/home/onyazuka/playlist.m3u");
    //return "";
    return writer.playlist();
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
    onCellDoubleClicked(nextRow(activeRowNumber()), Column::Title);
}

void QPlaylist::nextRandom() {
    if (empty()) {
        return;
    }
    onNext();
    onCellDoubleClicked(rand() % rowCount(), Column::Title);
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

void QPlaylist::onCellDoubleClicked(int row, int col) {
    if (row < 0 || row >= rowCount()) {
        return;
    }
    QPlaylistView::onCellDoubleClicked(row, col);
    Track tr = track(row);
    emit fileChanged(tr.path);
}

// got 'duration' in ms
void QPlaylist::onUpdateDuration(qint64 durationMs, int row) {
    QTableWidgetItem* it = item(row, Column::Duration);
    if (!it) return;
    /*if (duration < 0) {
        duration = 0;
    }*/
    if (durationMs <= 0) {
        durationMs = 0;
        it->setData(Qt::ForegroundRole, QVariant(QColor(Qt::red)));
    }
    else {
        it->setData(Qt::ForegroundRole, QVariant(QColor(Qt::black)));
    }
    it->setText(durationMsToStrDuration(durationMs));
    item(row, Column::Duration)->setData(Qt::UserRole, (int)durationMs);
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
    if (event->key() == Qt::Key_Delete) {
        itemRemoveAction->setData(currentRow());
        itemRemoveAction->trigger();
    }
    else {
        QTableWidget::keyPressEvent(event);
    }
}

void QPlaylist::initMenu() {
    itemRemoveAction = new QAction("Remove from playlist", this);
    itemShowMetadata = new QAction("Show metadata", this);
    connect(itemRemoveAction, &QAction::triggered, this, [this](){
        int row = itemRemoveAction->data().toInt();
        QTableWidgetItem* it = item(row, Column::Duration);
        if (!it) return;
        if (it->text().isEmpty()) {
            // duration not filled - not initialized - skipping
            return;
        }
        onRemoveTableWidgetItem(it);
        removeRow(row);
    });
    connect(itemShowMetadata, &QAction::triggered, this, [this](){
        QString path = item(currentRow(), Column::Title)->data(Qt::UserRole).toString();
        QMetadataDlg dlg(path, this);
        dlg.exec();
    });
    itemRightClickMenu = new QMenu();
    itemRightClickMenu->addAction(itemShowMetadata);
    itemRightClickMenu->addSeparator();
    itemRightClickMenu->addAction(itemRemoveAction);
    //itemRightClickMenu->platformMenu()
}

/*
    Cleaning up on item remove
    (activeItem, history, ...)
*/
void QPlaylist::onRemoveTableWidgetItem(QTableWidgetItem* it) {
    for (int i = 0; i < playHistory.size(); ++i) {
        if (!playHistory[i] || it->row() == playHistory[i]->row()) {
            playHistory.erase(playHistory.begin() + i);
        }
    }
    for (int i = it->row() + 1; i < rowCount(); ++i) {
        item(i, Column::Number)->setText(QString::number(i));
    }
    // setting activeItem AFTER history, because else we could get nullptr dereference attempt
    if (activeRowNumber() == it->row()) {
        resetActiveItem();
    }
}


