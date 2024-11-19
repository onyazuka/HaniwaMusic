#include "QPlaylist.h"
#include <QResizeEvent>
#include <QSignalSpy>

DurationGatherer::DurationGatherer() {
    utilityPlayer = new QMediaPlayer();
}

void DurationGatherer::init() {
    timer = new QTimer();
    timer->setInterval(100);
    QObject::connect(timer, &QTimer::timeout, this, &DurationGatherer::onTimer);
    connect(utilityPlayer, &QMediaPlayer::durationChanged, this, &DurationGatherer::onDuration);
    timer->start();
}

void DurationGatherer::onTimer() {
    if (!taskQueue.empty()) {
        timer->stop();
        curTask = taskQueue.front();
        taskQueue.pop_front();
        utilityPlayer->setSource(QUrl::fromLocalFile(curTask.path));
    }
    else {
        timer->setInterval(100);
        timer->start();
    }
}

void DurationGatherer::onDuration(qint64 duration) {
    emit gotDuration(duration, curTask.row);
    onTimer();
}

void DurationGatherer::onAddFile(const QString& path, int row) {
    if (!inited) {
        init();
        inited = true;
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

    durationGatherer = new DurationGatherer();
    durationGatherer->moveToThread(&durationGathererThread);
    connect(&durationGathererThread, &QThread::finished, durationGatherer, &QObject::deleteLater);
    connect(this, &QPlaylist::fileAdded, durationGatherer, &DurationGatherer::onAddFile);
    connect(durationGatherer, &DurationGatherer::gotDuration, this, &QPlaylist::onUpdateDuration);
    //connect(this, &QTableWidget::customContextMenuRequested, this, &QPlaylist::handleContextMenu);
    durationGathererThread.start();
}

QPlaylist::~QPlaylist() {
    durationGathererThread.quit();
    durationGathererThread.wait();
}

void QPlaylist::addFile(const QString& path) {
    int row = rowCount();
    insertRow(rowCount());
    setItem(row, Column::Number, new QTableWidgetItem(QString::number(row + 1)));
    setItem(row, Column::Title, new QTableWidgetItem(path));
    setItem(row, Column::Duration, new QTableWidgetItem("n/a"));
    for (int col = 0; col < Column::COUNT; ++col) {
        auto curItem = item(row, col);
        curItem->setFlags(curItem->flags() ^ Qt::ItemIsEditable);
    }

    //getAndSetDuration(row, path);
    emit fileAdded(path, row);
    if (rowCount() % 10 == 0) updateColumnWidths(width());
}

void QPlaylist::addFiles(const QStringList& files) {
    for (const auto& file : files) {
        addFile(file);
    }
}

void QPlaylist::addFolder(const QString& path) {
    QDir dir(path);
    for (const auto& fileInfo : dir.entryInfoList(QStringList({"*.mp3", "*.wav", "*.flac"}))) {
        addFile(fileInfo.absoluteFilePath());
    }
}

QStringList QPlaylist::toStringList() const {
    QStringList res;
    for(int i = 0; i < rowCount(); ++i) {
        res.append(item(i, Column::Title)->text());
    }
    return res;
}

void QPlaylist::clear() {
    QTableWidget::clear();
    setRowCount(0);
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

void QPlaylist::onCellDoubleClicked(int row, int) {
    activeItem = item(row, Column::Number);
    selectRow(row);
    emit fileChanged(item(row, Column::Title)->text());
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
}

void QPlaylist::updateColumnWidths(int totalTableWidth) {
    QFontMetrics metrics(font());
    QSize numberSz = metrics.size(0, QString::number(rowCount() * 10));
    QSize durationSz = metrics.size(0, " 99:99:99 ");
    setColumnWidth(Column::Number, numberSz.width());
    setColumnWidth(Column::Title, totalTableWidth - numberSz.width() - durationSz.width());
    setColumnWidth(Column::Duration, durationSz.width());
}

