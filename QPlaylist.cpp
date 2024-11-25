#include "QPlaylist.h"
#include <QResizeEvent>
#include <QMenu>
#include <QDirIterator>

DurationGatherer::DurationGatherer() {
    utilityPlayer = new QMediaPlayer();
}

void DurationGatherer::init() {
    timer = new QTimer();
    //timer->setInterval(100);
    QObject::connect(timer, &QTimer::timeout, this, &DurationGatherer::onTimer);
    connect(utilityPlayer, &QMediaPlayer::durationChanged, this, &DurationGatherer::onDuration);
    connect(utilityPlayer, &QMediaPlayer::mediaStatusChanged, this, &DurationGatherer::onMediaStatusChanged);
    //timer->start();
}

void DurationGatherer::onTimer() {
    if (!taskQueue.empty()) {
        timer->stop();
        curTask = taskQueue.front();
        taskQueue.pop_front();
        utilityPlayer->setSource(QUrl::fromLocalFile(curTask.path));
    }
    else {
        //timer->setInterval(100);
        //timer->start();
        timer->stop();
    }
}

void DurationGatherer::onDuration(qint64 duration) {
    emit gotDuration(duration, curTask.row);
    // setting empty source resets QMediaPlayer (I hope)
    //utilityPlayer->setSource(QUrl());
    //onTimer();
}

void DurationGatherer::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::MediaStatus::LoadedMedia) {
        onTimer();
    }
}

void DurationGatherer::onAddFile(const QString& path, int row) {
    if (!inited) {
        init();
        inited = true;
    }
    if (!timer->isActive()) {
        timer->setInterval(100);
        timer->start();
    }
    taskQueue.append({path,row,0});
}

QPlaylist::QPlaylist()
    : QTableWidget()
{
    setColumnCount(Column::COUNT);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    //horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(this, &QTableWidget::cellDoubleClicked, this, &QPlaylist::onCellDoubleClicked);
    horizontalHeader()->hide();
    verticalHeader()->hide();
    initMenu();
    setSelectionMode(QAbstractItemView::SingleSelection);

    durationGatherer = new DurationGatherer();
    durationGatherer->moveToThread(&durationGathererThread);
    connect(&durationGathererThread, &QThread::finished, durationGatherer, &QObject::deleteLater);
    connect(this, &QPlaylist::fileAdded, durationGatherer, &DurationGatherer::onAddFile);
    connect(durationGatherer, &DurationGatherer::gotDuration, this, &QPlaylist::onUpdateDuration);
    connect(this, &QWidget::customContextMenuRequested, this, &QPlaylist::handleContextMenu);
    durationGathererThread.start();
}

QPlaylist::~QPlaylist() {
    durationGathererThread.quit();
    durationGathererThread.wait();
}

void QPlaylist::addFile(const QString& path, const QString& duration) {
    int row = rowCount();
    insertRow(rowCount());
    setItem(row, Column::Number, new QTableWidgetItem(QString::number(row + 1)));
    QTableWidgetItem* titleItem = new QTableWidgetItem(QFileInfo(path).completeBaseName());
    titleItem->setData(Qt::UserRole, path);
    setItem(row, Column::Title, titleItem);
    setItem(row, Column::Duration, new QTableWidgetItem(duration));
    for (int col = 0; col < Column::COUNT; ++col) {
        auto curItem = item(row, col);
        curItem->setFlags(curItem->flags() ^ Qt::ItemIsEditable);
    }

    if (duration.isEmpty()) {
        emit fileAdded(path, row);
    }
    if (rowCount() % 10 == 0) updateColumnWidths(width());
}

void QPlaylist::addFileFromJson(const QJsonObject& obj) {
    QString path = obj.value("path").toString();
    QString duration = obj.value("duration").toString();
    addFile(path, duration);
}

void QPlaylist::addFiles(const QStringList& files) {
    for (const auto& file : files) {
        addFile(file);
    }
}

void QPlaylist::addFolder(const QString& path) {
    QDirIterator it(path, QStringList({"*.mp3", "*.wav", "*.flac"}), QDir::Files, QDirIterator::Subdirectories);
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
        res.append(item(i, Column::Title)->data(Qt::UserRole).toString());
    }
    return res;
}

QJsonArray QPlaylist::toJson() const {
    QJsonArray jArr;
    for(int i = 0; i < rowCount(); ++i) {
        jArr.append(QJsonObject({
            {"path", item(i, Column::Title)->data(Qt::UserRole).toString()},
            {"duration", item(i, Column::Duration)->text()}
        }));
    }
    return jArr;
}

int QPlaylist::currentTrackNumber() const {
    if (!activeItem) {
        return -1;
    }
    return activeItem->row();
}

void QPlaylist::clear() {
    QTableWidget::clear();
    setRowCount(0);
    activeItem = nullptr;
}

void QPlaylist::next() {
    if (!activeItem) {
        return;
    }
    int row = activeItem->row();
    if (row == (rowCount() - 1)) {
        return;
    }
    onCellDoubleClicked(row + 1, Column::Title);
}

void QPlaylist::nextRandom() {
    onCellDoubleClicked(rand() % rowCount(), Column::Title);
}

void QPlaylist::prev() {
    if (!activeItem) {
        return;
    }
    int row = activeItem->row();
    if (row == 0) {
        return;
    }
    onCellDoubleClicked(row - 1, Column::Title);
}

void QPlaylist::select(int row) {
    if (row < 0 || row >= rowCount()) {
        return;
    }
    setCurrentCell(row, Column::Title);
}

void QPlaylist::onCellDoubleClicked(int row, int) {
    if (activeItem) {
        QTableWidgetItem* it = item(activeItem->row(), Column::Title);
        QFont font = it->font();
        font.setBold(false);
        it->setFont(font);
    }
    activeItem = item(row, Column::Number);
    QTableWidgetItem* it = item(row, Column::Title);
    QFont font = it->font();
    font.setBold(true);
    it->setFont(font);
    selectRow(row);
    emit fileChanged(item(row, Column::Title)->data(Qt::UserRole).toString());
}

void QPlaylist::onUpdateDuration(qint64 duration, int row) {
    QTableWidgetItem* it = item(row, Column::Duration);
    if (!it) return;
    if (duration < 1000 * 60 * 60) {
        it->setText(QTime(0,0,0).addMSecs(duration).toString("m:ss"));
    }
    else {
        it->setText(QTime(0,0,0).addMSecs(duration).toString("hh:mm:ss"));
    }
}

void QPlaylist::handleContextMenu(const QPoint& pos) {
    QTableWidgetItem *item = itemAt(pos);
    if (item) {
        // do what you want with the item.
    }
}

void QPlaylist::resizeEvent(QResizeEvent* event) {
    updateColumnWidths(event->size().width());
    QTableWidget::resizeEvent(event);
}

void QPlaylist::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MouseButton::RightButton) {
        QTableWidgetItem* it = itemAt(event->position().toPoint());
        int row = it->row();
        selectRow(row);
        itemRemoveAction->setData(row);
        itemRightClickMenu->exec(event->globalPosition().toPoint());
    }
    else {
        QTableWidget::mousePressEvent(event);
    }
}

void QPlaylist::updateColumnWidths(int totalTableWidth) {
    QFontMetrics metrics(font());
    QSize numberSz = metrics.size(0, QString::number(rowCount() * 10));
    QSize durationSz = metrics.size(0, " 99:99:99 ");
    setColumnWidth(Column::Number, numberSz.width());
    setColumnWidth(Column::Title, totalTableWidth - numberSz.width() - durationSz.width());
    setColumnWidth(Column::Duration, durationSz.width());
}

void QPlaylist::initMenu() {
    itemRemoveAction = new QAction("Remove from playlist", this);
    connect(itemRemoveAction, &QAction::triggered, this, [this](){
        int row = itemRemoveAction->data().toInt();
        QTableWidgetItem* it = item(row, Column::Duration);
        if (!it) return;
        if (it->text().isEmpty()) {
            // duration not filled - not initialized - skipping
            return;
        }
        removeRow(row);
    });
    itemRightClickMenu = new QMenu();
    itemRightClickMenu->addAction(itemRemoveAction);
    //itemRightClickMenu->platformMenu()
}

