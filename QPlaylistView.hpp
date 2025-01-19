#ifndef QPLAYLISTVIEW_HPP
#define QPLAYLISTVIEW_HPP
#include <QTableWidget>

struct Track {
    Track() = default;
    Track(const QString& path, int duration);
    QString path;
    int durationMs;
};

class QPlaylistView : public QTableWidget {
    Q_OBJECT
public:
    enum Column {
        Number,
        Title,
        Duration,
        COUNT
    };
    QPlaylistView(QWidget* parent);
    void swapRows(int row1, int row2);
    int appendTrack(const Track& track);
    Track track(int row) const;
    QString durationMsToStrDuration(qint64 durationMs);
    int activeRowNumber() const;
    void resetActiveItem();
    inline QTableWidgetItem* activeItem() { return _activeItem; }
    int nextRow(int row) const;
    int prevRow(int row) const;
    inline bool empty() const { return rowCount() == 0; }
public slots:
    void clear();
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void resizeEvent(QResizeEvent *event) override;
protected slots:
    void onCellDoubleClicked(int row, int col);
private:
    void updateColumnWidths(int totalTableWidth);
    QPoint dragStartPosition = QPoint(0,0);
    int dragRowSource = 0;
    // last double-clicked item
    QTableWidgetItem* _activeItem = nullptr;
};

#endif // QPLAYLISTVIEW_HPP
