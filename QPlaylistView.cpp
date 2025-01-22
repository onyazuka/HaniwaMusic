#include "QPlaylistView.hpp"
#include <QHeaderView>
#include <QMimeData>
#include <QMouseEvent>
#include <QDrag>
#include <QApplication>
#include <QTime>
#include <QFileInfo>

Metainfo::Metainfo(int durationMs)
    : durationMs{durationMs}
{
    ;
}

Metainfo::Metainfo(const QString& title, const QString& artist, int durationMs)
    : title{title}, artist{artist}, durationMs{durationMs}
{
    ;
}

Track::Track(const QString& path)
    : path{path}
{
    ;
}

Track::Track(const QString& path, const Metainfo& metainfo)
    : path{path}, metainfo{metainfo}
{
    ;
}

QPlaylistView::QPlaylistView(QWidget* parent)
    : QTableWidget(parent)
{
    setColumnCount(Column::COUNT);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setGridStyle(Qt::PenStyle::NoPen);
    horizontalHeader()->hide();
    verticalHeader()->hide();
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAcceptDrops(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    connect(this, &QTableWidget::cellDoubleClicked, this, &QPlaylistView::onCellDoubleClicked);
}

QString QPlaylistView::durationMsToStrDuration(qint64 durationMs) {
    if (durationMs < 1000 * 60 * 60) {
        return QTime(0,0,0).addMSecs(durationMs).toString("m:ss");
    }
    else {
        return QTime(0,0,0).addMSecs(durationMs).toString("hh:mm:ss");
    }
}

int QPlaylistView::activeRowNumber() const {
    if (!_activeItem) {
        return -1;
    }
    return _activeItem->row();
}

void QPlaylistView::resetActiveItem() {
    _activeItem = nullptr;
}

int QPlaylistView::nextRow(int row) const {
    if (row < 0 || row >= rowCount()) {
        return -1;
    }
    return row + 1;
}

int QPlaylistView::prevRow(int row) const {
    if (row <= 0 || row > rowCount()) {
        return -1;
    }
    return row - 1;
}

void QPlaylistView::clear() {
    QTableWidget::clear();
    setRowCount(0);
    _activeItem = nullptr;
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

int QPlaylistView::appendTrack(const Track& track) {
    int row = rowCount();
    QString duration = track.metainfo.durationMs < 0 ? "" : durationMsToStrDuration(track.metainfo.durationMs);
    insertRow(rowCount());
    setItem(row, Column::Number, new QTableWidgetItem(QString::number(row + 1)));
    QTableWidgetItem* titleItem = new QTableWidgetItem();
    if (!track.metainfo.title.isEmpty() && !track.metainfo.artist.isEmpty()) {
        titleItem->setText(track.metainfo.artist + " - " + track.metainfo.title);
    }
    else {
        titleItem->setText(QFileInfo(track.path).completeBaseName());
    }
    titleItem->setData(Qt::UserRole, track.path);
    setItem(row, Column::Title, titleItem);
    setItem(row, Column::Duration, new QTableWidgetItem(duration));
    if (track.metainfo.durationMs <= 0) {
        item(row, Column::Duration)->setData(Qt::ForegroundRole, QVariant(QColor(Qt::red)));
    }
    for (int col = 0; col < Column::COUNT; ++col) {
        auto curItem = item(row, col);
        curItem->setFlags(curItem->flags() ^ Qt::ItemIsEditable);
    }

    if (duration.isEmpty()) {
        item(row, Column::Duration)->setData(Qt::UserRole, -1);
        item(row, Column::Duration)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
    else {
        item(row, Column::Duration)->setData(Qt::UserRole, (int)track.metainfo.durationMs);
        item(row, Column::Duration)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }
    QVariant v;
    v.setValue(track.metainfo);
    item(row, Column::Title)->setData(Qt::UserRole + (int)UserRoles::Metainfo, v);
    if (rowCount() % 10 == 0) updateColumnWidths(width());
    return row;
}

Track QPlaylistView::track(int row) const {
    Track track;
    track.path = item(row, Column::Title)->data(Qt::UserRole).toString();
    track.metainfo.durationMs = item(row, Column::Duration)->data(Qt::UserRole).toInt();
    Metainfo metainfo = item(row, Column::Title)->data(Qt::UserRole + (int)UserRoles::Metainfo).value<Metainfo>();
    track.metainfo.title = metainfo.title;
    track.metainfo.artist = metainfo.artist;
    return track;
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

void QPlaylistView::resizeEvent(QResizeEvent* event) {
    updateColumnWidths(event->size().width());
    QTableWidget::resizeEvent(event);
}

void QPlaylistView::onCellDoubleClicked(int row, int) {
    if (row < 0 || row >= rowCount()) {
        return;
    }
    if (_activeItem) {
        QTableWidgetItem* it = item(_activeItem->row(), Column::Title);
        QFont font = it->font();
        font.setBold(false);
        it->setFont(font);
    }
    _activeItem = item(row, Column::Title);
    QTableWidgetItem* it = item(row, Column::Title);
    QFont font = it->font();
    font.setBold(true);
    it->setFont(font);
    selectRow(row);
}

void QPlaylistView::updateColumnWidths(int totalTableWidth) {
    QFontMetrics metrics(font());
    QSize numberSz = metrics.size(0, QString::number(rowCount() * 10));
    QSize durationSz = metrics.size(0, " 99:99:99 ");
    setColumnWidth(Column::Number, numberSz.width());
    setColumnWidth(Column::Title, totalTableWidth - numberSz.width() - durationSz.width());
    setColumnWidth(Column::Duration, durationSz.width());
}
