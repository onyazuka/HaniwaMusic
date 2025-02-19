#ifndef QPLAYLISTVIEW_HPP
#define QPLAYLISTVIEW_HPP
#include <QTableWidget>

struct Metainfo {
    Metainfo() = default;
    Metainfo(int durationMs);
    Metainfo(const QString& title, const QString& artist, int durationMs);
    QString title;
    QString artist;
    int durationMs = -1;
};
Q_DECLARE_METATYPE(Metainfo);

struct Track {
    Track() = default;
    Track(const QString& path);
    Track(const QString& path, const Metainfo& metainfo);
    QString path;
    Metainfo metainfo;
};

enum class UserRoles {
    Path,
    Metainfo
};

enum class SortOrder {
    Title,
    Artist,
    Duration,
    Path
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
    void setDuration(int row, int durationMs);
    int getDuration(int row) const;
    void setTitle(int row, const QString& path, const QString& title, const QString& artist, int queueNum = 0);
    QString getPath(int row) const;
    void setMetainfo(int row, const Metainfo& metainfo);
    Metainfo getMetainfo(int row) const;
public slots:
    void clear();
    void sortByTitle(bool asc);
    void sortByArtist(bool asc);
    void sortByDuration(bool asc);
    void sortByPath(bool asc);
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
