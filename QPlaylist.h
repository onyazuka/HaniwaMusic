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

class DurationGatherer : public QObject {
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
};

class DurationGatherer2 : public QObject {
    Q_OBJECT
    struct TaskDescr {
        QString path;
        int row;
        qint64 duration;
    };
public:
    DurationGatherer2();
    ~DurationGatherer2();
public slots:
    void onAddFile(const QString& path, int row);
signals:
    void gotDuration(qint64 duration, int row);
private:
    void run();
    QQueue<TaskDescr> taskQueue;
    QMutex mtx;
    QWaitCondition cnd;
    std::atomic_bool terminated = false;
};

class QPlaylist : public QTableWidget {
    Q_OBJECT
public:
    enum Column {
        Number,
        Title,
        Duration,
        COUNT
    };

    QPlaylist(QWidget* parent = nullptr);
    ~QPlaylist();
    void addFile(const QString& path, const QString& duration = "");
    void addFileFromJson(const QJsonObject& obj);
    void addFiles(const QStringList& files);
    void addFolder(const QString& path);
    void addFilesFromJson(const QJsonArray& json);
    QStringList toStringList() const;
    QJsonArray toJson() const;
    int currentTrackNumber() const;
signals:
    void fileChanged(QString path);
    void fileAdded(QString path, int row);
public slots:
    void clear();
    void next();
    void nextRandom();
    void prev();
    void select(int row);
private slots:
    void onCellDoubleClicked(int row, int col);
    void onUpdateDuration(qint64 duration, int row);
    void handleContextMenu(const QPoint& pos);
private:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
    void updateColumnWidths(int totalTableWidth);
    void initMenu();
    QTableWidgetItem* activeItem = nullptr;
    QThread durationGathererThread;
    DurationGatherer2* durationGatherer;
    QMenu* itemRightClickMenu = nullptr;;
    QAction* itemRemoveAction = nullptr;
    QPoint dragStartPosition = QPoint(0,0);
    int dragRowSource = 0;
};

#endif // QPLAYLIST_H
