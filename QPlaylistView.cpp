#include "QPlaylistView.hpp"
#include <QHeaderView>
#include <QMimeData>
#include <QMouseEvent>
#include <QDrag>
#include <QApplication>

QPlaylistView::QPlaylistView(QWidget* parent)
    : QTableWidget(parent)
{
    setColumnCount(Column::COUNT);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    horizontalHeader()->hide();
    verticalHeader()->hide();
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAcceptDrops(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
}

void QPlaylistView::swapRows(int sourceRow, int destRow) {
    if (sourceRow < 0 || sourceRow >= rowCount() || destRow < 0 || destRow >= rowCount()) {
        return;
    }
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
}

void QPlaylistView::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::MouseButton::LeftButton) {
        QTableWidgetItem* item = itemAt(event->position().toPoint());
        if (item) {
            dragStartPosition = event->pos();
            dragRowSource = item->row();
        }
    }
    QTableWidget::mousePressEvent(event);
}

void QPlaylistView::mouseMoveEvent(QMouseEvent* event)  {
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

void QPlaylistView::dragEnterEvent(QDragEnterEvent* event) {
    if (event->mimeData()->hasFormat("text/plain")) {
        event->acceptProposedAction();
    }
}

void QPlaylistView::dragMoveEvent(QDragMoveEvent* event) {
    QTableWidgetItem* it = itemAt(event->position().toPoint());
    //int sourceRow = event->mimeData()->text().toInt();
    int sourceRow = dragRowSource;
    if (it && (it->row() != sourceRow)) {
        if (sourceRow < 0 || sourceRow >= rowCount()) {
            return;
        }
        int destRow = it->row();
        swapRows(sourceRow, destRow);
        selectRow(destRow);
        dragRowSource = destRow;

        if (event->position().y() <= rowHeight(0)) {
            scrollToItem(item(destRow - 1, Column::Title));
        }
        else if (event->position().y() >= (height() - rowHeight(0))) {
            scrollToItem(item(destRow + 1, Column::Title));
        }
    }
    event->acceptProposedAction();
}

void QPlaylistView::dropEvent(QDropEvent* event) {
    event->acceptProposedAction();
}
