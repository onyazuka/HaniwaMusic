#include "QPlaylist.h"

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

void QPlaylist::clear() {
    QTableWidget::clear();
    setRowCount(0);
}

void QPlaylist::onCellDoubleClicked(int row, int) {
    emit fileChanged(item(row, Column::Title)->text());
}

void QPlaylist::resizeEvent(QResizeEvent*) {
    QFontMetrics metrics(font());
    QSize durationSz = metrics.size(0, "99:99:99");
    setColumnWidth(0, width() - durationSz.width());
    setColumnWidth(1, durationSz.width());
}
