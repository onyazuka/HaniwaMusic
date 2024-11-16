#include "QPlaylist.h"

QPlaylist::QPlaylist()
    : QTableWidget()
{
    setColumnCount(Column::COUNT);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    connect(this, &QTableWidget::cellDoubleClicked, this, &QPlaylist::onCellDoubleClicked);
}

void QPlaylist::addFile(const QString& path) {
    int row = rowCount();
    insertRow(rowCount());
    setItem(row, Column::Title, new QTableWidgetItem(path));
    setItem(row, Column::Duration, new QTableWidgetItem("n/a"));
}

void QPlaylist::onCellDoubleClicked(int row, int col) {
    emit fileChanged(item(row, Column::Title)->text());
}
