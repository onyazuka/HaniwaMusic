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
#include "QPlaylistView.hpp"
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
    void onAddFile(const QString& path, QTableWidgetItem* it);
signals:
    void gotMetainfo(Metainfo metainfo, QTableWidgetItem* it);
private:
};

struct SearchCtx {
    QString lastStr;
    int lastRow = 0;
    bool finish = false;
};

class QPlaylist : public QPlaylistView {
    Q_OBJECT
    static constexpr size_t MaxRandomHistorySize = 100;
public:
    QPlaylist(QWidget* parent = nullptr);
    ~QPlaylist();
    void addFile(const QString& path, const Metainfo& metainfo = {});
    void addFileFromJson(const QJsonObject& obj);
    void addFiles(const QStringList& files);
    void addFolder(const QString& path);
    void addFilesFromJson(const QJsonArray& json);
    void addFilesFromM3uPlaylist(const m3u::M3UPlaylist& playlist);
    QStringList toStringList() const;
    QJsonArray toJson() const;
    m3u::M3UPlaylist toM3UPlaylist(const QString& title) const;
    bool exportTo(const QString& title, const QString& path);
    bool importFrom(const QString& path);
    bool findNext(QString str);
    bool empty() const;
signals:
    void fileChanged(QString path);
    void fileAddedWithUnknownDuration(QString path, QTableWidgetItem* it);
public slots:
    void current();
    void next();
    void nextRandom();
    void prev();
    void prevRandom();
    bool select(int row);
private slots:
    void onCellDoubleClicked(int row, int col);
    void onUpdateMetainfo(Metainfo metainfo, QTableWidgetItem* it);
    void handleContextMenu(const QPoint& pos);
    void onNext();
    void onAddToPlayQueue();
private:
    void mousePressEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void initMenu();
    void onRemoveTableWidgetItem(int row);
    void updatePlayQueueItems();
    QThread durationGathererThread;
    DurationGatherer2* durationGatherer;
    QMenu* itemRightClickMenu = nullptr;
    QAction* itemRemoveAction = nullptr;
    QAction* itemShowMetadata = nullptr;
    QAction* itemAddToPlayQueue = nullptr;
    SearchCtx searchCtx;
    QList<QTableWidgetItem*> playHistory;
    QList<QTableWidgetItem*> playQueue;
};

#endif // QPLAYLIST_H
