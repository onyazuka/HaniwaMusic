#include "QPlaylist.h"
#include <QResizeEvent>

QPlaylist::QPlaylist()
    : QTableWidget()
{
    setColumnCount(Column::COUNT);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    //horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(this, &QTableWidget::cellDoubleClicked, this, &QPlaylist::onCellDoubleClicked);
    horizontalHeader()->hide();
    verticalHeader()->hide();
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

void QPlaylist::resizeEvent(QResizeEvent* event) {
    QFontMetrics metrics(font());
    QSize numberSz = metrics.size(0, QString::number(rowCount() * 10));
    QSize durationSz = metrics.size(0, " 99:99:99 ");
    setColumnWidth(Column::Number, numberSz.width());
    setColumnWidth(Column::Title, event->size().width() - numberSz.width() - durationSz.width());
    setColumnWidth(Column::Duration, durationSz.width());
}
