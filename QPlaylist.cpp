#include "QPlaylist.h"
#include <QResizeEvent>
#include <QMenu>
#include <QDirIterator>
#include <QDrag>
#include <QMimeData>
#include <QApplication>
#include "MetaTagsParser/TagScout.hpp"


/*DurationGatherer::DurationGatherer() {
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
}*/

DurationGatherer2::DurationGatherer2() {
    ;
}

DurationGatherer2::~DurationGatherer2() {
    ;
}

void DurationGatherer2::onAddFile(const QString& path, int row) {
    //taskQueue.append({path,row,0});
    //cnd.notify_one();
    try {
        auto metainfo = getMetainfo(path.toStdString());
        emit gotDuration((qint64)(QString::fromStdString(metainfo["durationMs"]).toULongLong()), row);
    }
    catch (...) {
        emit gotDuration(0, row);
    }
}

QPlaylist::QPlaylist(QWidget* parent)
    : QTableWidget(parent)
{
    setColumnCount(Column::COUNT);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    //horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(this, &QTableWidget::cellDoubleClicked, this, &QPlaylist::onCellDoubleClicked);
    horizontalHeader()->hide();
    verticalHeader()->hide();
    initMenu();
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAcceptDrops(true);
    durationGatherer = new DurationGatherer2();
    durationGatherer->moveToThread(&durationGathererThread);
    connect(&durationGathererThread, &QThread::finished, durationGatherer, &QObject::deleteLater);
    connect(this, &QPlaylist::fileAdded, durationGatherer, &DurationGatherer2::onAddFile);
    connect(durationGatherer, &DurationGatherer2::gotDuration, this, &QPlaylist::onUpdateDuration);
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

/*
    selects current row as active item and plays it
*/
void QPlaylist::current() {
    if (activeItem) {
        return;
    }
    onCellDoubleClicked(currentRow(), Column::Title);
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
    while ((size_t)randomHistory.size() >= MaxRandomHistorySize) {
        randomHistory.pop_back();
    }
    randomHistory.push_back(activeItem);
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

void QPlaylist::prevRandom() {
    while (!randomHistory.empty()) {
        QTableWidgetItem* it = randomHistory.back();
        randomHistory.pop_back();
        if (it) {
            onCellDoubleClicked(it->row(), Column::Title);
            return;;
        }
    }
    prev();
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
    activeItem = item(row, Column::Title);
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
    /*if (duration < 0) {
        duration = 0;
    }*/
    if (duration <= 0) {
        duration = 0;
        it->setData(Qt::ForegroundRole, QVariant(QColor(Qt::red)));
    }
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
        if (it) {
            int row = it->row();
            selectRow(row);
            itemRemoveAction->setData(row);
            itemRightClickMenu->exec(event->globalPosition().toPoint());
        }
    }
    else if (event->button() == Qt::MouseButton::LeftButton) {
        dragStartPosition = event->pos();
        dragRowSource = itemAt(event->position().toPoint())->row();
    }
    QTableWidget::mousePressEvent(event);
}

void QPlaylist::mouseMoveEvent(QMouseEvent* event)  {
    if (event->buttons() & Qt::MouseButton::LeftButton) {
        if ((event->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance()) {
            return;
        }
        QTableWidgetItem* it = itemAt(event->position().toPoint());
        if (it) {
            QDrag* drag = new QDrag(this);
            QMimeData* mimeData = new QMimeData;
            mimeData->setText(QString::number(it->row()));
            drag->setMimeData(mimeData);
            drag->exec();
        }
    }
    else {
        QTableWidget::mouseMoveEvent(event);
    }
}

void QPlaylist::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return) {
        onCellDoubleClicked(currentRow(), Column::Title);
    }
    else {
        QTableWidget::keyPressEvent(event);
    }
}

void QPlaylist::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasFormat("text/plain")) {
        event->acceptProposedAction();
    }
}

void QPlaylist::dragMoveEvent(QDragMoveEvent* event) {
    QTableWidgetItem* it = itemAt(event->position().toPoint());
    //int sourceRow = event->mimeData()->text().toInt();
    int sourceRow = dragRowSource;
    if (it && (it->row() != sourceRow)) {
        if (sourceRow < 0 || sourceRow >= rowCount()) {
            return;
        }
        int destRow = it->row();
        QList<QTableWidgetItem*> sourceRowItems;
        QList<QTableWidgetItem*> destRowItems;
        for (int col = (int)Column::Title; col < (int)Column::COUNT; ++col) {
            QTableWidgetItem* sourceItem = takeItem(sourceRow, col);
            sourceRowItems.append(sourceItem);
            QTableWidgetItem* destItem = takeItem(destRow, col);
            destRowItems.append(destItem);
        }
        for (int col = (int)Column::Title; col < (int)Column::COUNT; ++col) {
            setItem(sourceRow, col, destRowItems.front());
            destRowItems.pop_front();
            setItem(destRow, col, sourceRowItems.front());
            sourceRowItems.pop_front();
        }
        selectRow(destRow);
        dragRowSource = destRow;

        qDebug() << event->position().y();
        qDebug() << rowHeight(0);
        if (event->position().y() <= rowHeight(0)) {
            scrollToItem(item(destRow - 1, Column::Title));
        }
        else if (event->position().y() >= (height() - rowHeight(0))) {
            scrollToItem(item(destRow + 1, Column::Title));
        }

        /*QTableWidgetItem* newDstItem = new QTableWidgetItem(QString::number(destRow));
        if (activeItem && (activeItem->row() == sourceRow)) {
            activeItem = newDstItem;
        }
        removeRow(sourceRow);
        insertRow(destRow);
        setItem(destRow, Column::Number, newDstItem);
        for (int col = Column::Title; col < Column::COUNT; ++col) {
            setItem(destRow, col, sourceRowItems.front());
            sourceRowItems.pop_front();
        }
        for (int i = std::min(destRow, sourceRow); i <= std::max(destRow, sourceRow); ++i) {
            auto curItem = item(i, Column::Number);
            curItem->setText(QString::number(i + 1));
        }
        selectRow(destRow);
        dragRowSource = destRow;*/
    }
    event->acceptProposedAction();
}

void QPlaylist::dropEvent(QDropEvent* event) {
    /*QTableWidgetItem* it = itemAt(event->position().toPoint());
    if (it) {
        int sourceRow = event->mimeData()->text().toInt();
        if (sourceRow < 0 || sourceRow >= rowCount()) {
            return;
        }
        int destRow = it->row();
        QList<QTableWidgetItem*> sourceRowItems;
        for (int col = (int)Column::Title; col < (int)Column::COUNT; ++col) {
            QTableWidgetItem* sourceItem = takeItem(sourceRow, col);
            sourceRowItems.append(sourceItem);
        }
        QTableWidgetItem* newDstItem = new QTableWidgetItem(QString::number(destRow));
        if (activeItem && (activeItem->row() == sourceRow)) {
            activeItem = newDstItem;
        }
        removeRow(sourceRow);
        insertRow(destRow);
        setItem(destRow, Column::Number, newDstItem);
        for (int col = Column::Title; col < Column::COUNT; ++col) {
            setItem(destRow, col, sourceRowItems.front());
            sourceRowItems.pop_front();
        }
        for (int i = std::min(destRow, sourceRow); i <= std::max(destRow, sourceRow); ++i) {
            auto curItem = item(i, Column::Number);
            curItem->setText(QString::number(curItem->text().toInt() + 1));
        }
    }*/
    event->acceptProposedAction();
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
        onRemoveTableWidgetItem(it);
        removeRow(row);
    });
    itemRightClickMenu = new QMenu();
    itemRightClickMenu->addAction(itemRemoveAction);
    //itemRightClickMenu->platformMenu()
}

/*
    Cleaning up on item remove
    (activeItem, history, ...)
*/
void QPlaylist::onRemoveTableWidgetItem(QTableWidgetItem* item) {
    for (int i = 0; i < randomHistory.size(); ++i) {
        if (!randomHistory[i] || item->row() == randomHistory[i]->row()) {
            randomHistory.erase(randomHistory.begin() + i);
        }
    }
    // setting activeItem AFTER history, because else we could get nullptr dereference attempt
    if (activeItem && (activeItem->row() == item->row())) {
        activeItem = nullptr;
    }
}

