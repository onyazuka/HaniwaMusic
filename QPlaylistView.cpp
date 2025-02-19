#include "QPlaylistView.hpp"
#include <QHeaderView>
#include <QMimeData>
#include <QMouseEvent>
#include <QDrag>
#include <QApplication>
#include <QTime>
#include <QFileInfo>
#include <assert.h>

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
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    horizontalHeader()->hide();
    verticalHeader()->hide();
    setSelectionMode(QAbstractItemView::SingleSelection);
    setAcceptDrops(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarPolicy::ScrollBarAlwaysOff);
    connect(this, &QTableWidget::cellDoubleClicked, this, &QPlaylistView::onCellDoubleClicked);
}

QString QPlaylistView::durationMsToStrDuration(qint64 durationMs) {
    if (durationMs < 0) {
        return "";
    }
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

void QPlaylistView::setDuration(int row, int durationMs) {
    QTableWidgetItem* it = item(row, Column::Duration);
    if (!it && row < rowCount()) {
        it = new QTableWidgetItem();
        setItem(row, Column::Duration, it);
    }
    if (!it) {
        return;
    }
    item(row, Column::Duration)->setData(Qt::UserRole, (int)durationMs);
    if (durationMs >= 0) {
        it->setText(durationMsToStrDuration(durationMs));
        if (durationMs == 0) {
            it->setData(Qt::ForegroundRole, QColor(Qt::red));
        }
        else {
            it->setData(Qt::ForegroundRole, QColor(Qt::black));
        }
    }
}

int QPlaylistView::getDuration(int row) const {
    auto it = item(row, Column::Duration);
    if (it) {
        return it->data(Qt::UserRole).toInt();
    }
    return -1;
}

void QPlaylistView::setTitle(int row, const QString& path, const QString& title, const QString& artist, int queueNum) {
    assert(!path.isEmpty());
    QTableWidgetItem* it = item(row, Column::Title);
    if (!it && row < rowCount()) {
        it = new QTableWidgetItem();
        setItem(row, Column::Title, it);
    }
    if (!it) {
        return;
    }
    if (path.isEmpty() && (title.isEmpty() || artist.isEmpty())) {
        return;
    }
    it->setText((queueNum ? QString("[%1] ").arg(QString::number(queueNum)) : "") + (!title.isEmpty() && !artist.isEmpty() ? (artist + " - " + title) : QFileInfo(path).completeBaseName()));
    QFont font = it->font();
    font.setUnderline(queueNum > 0);
    it->setFont(font);
    it->setData(Qt::UserRole, path);
}

QString QPlaylistView::getPath(int row) const {
    auto it = item(row, Column::Title);
    if (it) {
        return it->data(Qt::UserRole).toString();
    }
    return "";
}

void QPlaylistView::setMetainfo(int row, const Metainfo& metainfo) {
    QVariant vmetainfo;
    vmetainfo.setValue(metainfo);
    item(row, Column::Title)->setData(Qt::UserRole + (int)UserRoles::Metainfo, vmetainfo);
}

Metainfo QPlaylistView::getMetainfo(int row) const {
    auto it = item(row, Column::Title);
    if (it) {
        return it->data(Qt::UserRole + (int)UserRoles::Metainfo).value<Metainfo>();
    }
    else {
        return Metainfo();
    }
}

void QPlaylistView::clear() {
    QTableWidget::clear();
    setRowCount(0);
    _activeItem = nullptr;
}

void QPlaylistView::sortByTitle() {
    QList<QTableWidgetItem*> items;
    for (size_t i = 0; i < rowCount(); ++i) {
        items.push_back(item(i, Column::Title));
    }
    std::sort(items.begin(), items.end(), [this](QTableWidgetItem* it1, QTableWidgetItem* it2) {
        auto meta1 = getMetainfo(it1->row());
        auto meta2 = getMetainfo(it2->row());
        // want to items without titles to go down on the list
        if (meta1.title.isEmpty() && !meta2.title.isEmpty()) {
            return false;
        }
        if (!meta1.title.isEmpty() && meta2.title.isEmpty()) {
            return true;
        }
        if (meta1.title.isEmpty() && meta2.title.isEmpty()) {
            auto path1 = getPath(it1->row());
            auto path2 = getPath(it2->row());
            return path1 < path2;
        }
        return meta1.title < meta2.title;
    });
    for (size_t i = 0; i < items.size(); ++i) {
        swapRows(i, items[i]->row());
    }
}

void QPlaylistView::sortByArtist() {
    QList<QTableWidgetItem*> items;
    for (size_t i = 0; i < rowCount(); ++i) {
        items.push_back(item(i, Column::Title));
    }
    std::sort(items.begin(), items.end(), [this](QTableWidgetItem* it1, QTableWidgetItem* it2) {
        auto meta1 = getMetainfo(it1->row());
        auto meta2 = getMetainfo(it2->row());
        // want to items without titles to go down on the list
        if (meta1.artist.isEmpty() && !meta2.artist.isEmpty()) {
            return false;
        }
        if (!meta1.artist.isEmpty() && meta2.artist.isEmpty()) {
            return true;
        }
        if (meta1.artist.isEmpty() && meta2.artist.isEmpty()) {
            auto path1 = getPath(it1->row());
            auto path2 = getPath(it2->row());
            return path1 < path2;
        }
        if (meta1.artist == meta2.artist) {
            return meta1.title < meta2.title;
        }
        return meta1.artist < meta2.artist;
    });
    for (size_t i = 0; i < items.size(); ++i) {
        swapRows(i, items[i]->row());
    }
}

void QPlaylistView::sortByDuration() {
    QList<QTableWidgetItem*> items;
    for (size_t i = 0; i < rowCount(); ++i) {
        items.push_back(item(i, Column::Title));
    }
    std::sort(items.begin(), items.end(), [this](QTableWidgetItem* it1, QTableWidgetItem* it2) {
        auto dur1 = getDuration(it1->row());
        auto dur2 = getDuration(it2->row());
        return dur1 < dur2;
    });
    for (size_t i = 0; i < items.size(); ++i) {
        swapRows(i, items[i]->row());
    }
}

void QPlaylistView::sortByPath() {
    QList<QTableWidgetItem*> items;
    for (size_t i = 0; i < rowCount(); ++i) {
        items.push_back(item(i, Column::Title));
    }
    std::sort(items.begin(), items.end(), [this](QTableWidgetItem* it1, QTableWidgetItem* it2) {
        auto path1 = getPath(it1->row());
        auto path2 = getPath(it2->row());
        return path1 < path2;
    });
    for (size_t i = 0; i < items.size(); ++i) {
        swapRows(i, items[i]->row());
    }
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
    insertRow(rowCount());
    setItem(row, Column::Number, new QTableWidgetItem(QString::number(row + 1)));
    setTitle(row, track.path, track.metainfo.title, track.metainfo.artist);
    setDuration(row, track.metainfo.durationMs);
    setMetainfo(row, track.metainfo);
    if (rowCount() % 10 == 0) updateColumnWidths(width());
    return row;
}

Track QPlaylistView::track(int row) const {
    Track track;
    track.path = getPath(row);
    track.metainfo.durationMs = getDuration(row);
    Metainfo metainfo = getMetainfo(row);
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
