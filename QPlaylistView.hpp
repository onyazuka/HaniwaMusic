#ifndef QPLAYLISTVIEW_HPP
#define QPLAYLISTVIEW_HPP
#include <QTableWidget>

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
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    QPoint dragStartPosition = QPoint(0,0);
    int dragRowSource = 0;
};

#endif // QPLAYLISTVIEW_HPP
