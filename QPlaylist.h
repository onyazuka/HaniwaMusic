#ifndef QPLAYLIST_H
#define QPLAYLIST_H

#include <QTableWidget>
#include <QHeaderView>
#include <QDir>
#include <QThread>
#include <QEventLoop>
#include <QMediaPlayer>
#include <QQueue>
#include <QTimer>
#include <QMenu>
#include <QAction>
#include <QJsonArray>
#include <QJsonObject>
#include <QMutex>
#include <QWaitCondition>
#include <atomic>
#include "M3UPlaylist.hpp"

/*class DurationGatherer : public QObject {
        Q_OBJECT
    struct TaskDescr {
        QString path;
        int row;
        qint64 duration;
    };

public:
    DurationGatherer();
public slots:
    void onAddFile(const QString& path, int row);
    void onDuration(qint64 duration);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onTimer();
    void init();
signals:
    void gotDuration(qint64 duration, int row);
private:
    QMediaPlayer* utilityPlayer = nullptr;
    QQueue<TaskDescr> taskQueue;
    QTimer* timer = nullptr;
    TaskDescr curTask;
    bool inited = false;
};*/

class DurationGatherer2 : public QObject {
    Q_OBJECT
public:
    DurationGatherer2();
    ~DurationGatherer2();
public slots:
    void onAddFile(const QString& path, int row);
signals:
    void gotDuration(qint64 duration, int row);
private:
};

struct SearchCtx {
    QString lastStr;
    int lastRow = 0;
    bool finish = false;
};

class QPlaylist : public QTableWidget {
    Q_OBJECT
    static constexpr size_t MaxRandomHistorySize = 100;
public:
    enum Column {
        Number,
        Title,
        Duration,
        COUNT
    };

    QPlaylist(QWidget* parent = nullptr);
    ~QPlaylist();
    void addFile(const QString& path, qint64 durationMs = -1);
    void addFileFromJson(const QJsonObject& obj);
    void addFiles(const QStringList& files);
    void addFolder(const QString& path);
    void addFilesFromJson(const QJsonArray& json);
    QStringList toStringList() const;
    QJsonArray toJson() const;
    m3u::M3UPlaylist toM3UPlaylist(const QString& title) const;
    int currentTrackNumber() const;
    bool findNext(QString str);
signals:
    void fileChanged(QString path);
    void fileAdded(QString path, int row);
public slots:
    void clear();
    void current();
    void next();
    void nextRandom();
    void prev();
    void prevRandom();
    bool select(int row);
private slots:
    void onCellDoubleClicked(int row, int col);
    void onUpdateDuration(qint64 duration, int row);
    void handleContextMenu(const QPoint& pos);
private:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void updateColumnWidths(int totalTableWidth);
    void initMenu();
    void onRemoveTableWidgetItem(QTableWidgetItem* item);
    QString durationMsToStrDuration(qint64 durationMs);
    QThread durationGathererThread;
    DurationGatherer2* durationGatherer;
    QMenu* itemRightClickMenu = nullptr;
    QAction* itemRemoveAction = nullptr;
    QAction* itemShowMetadata = nullptr;
    QPoint dragStartPosition = QPoint(0,0);
    int dragRowSource = 0;
    SearchCtx searchCtx;

    QTableWidgetItem* activeItem = nullptr;
    QList<QTableWidgetItem*> randomHistory;
};

#endif // QPLAYLIST_H
