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

class QPlaylist : public QTableWidget {
    Q_OBJECT
public:
    enum Column {
        Number,
        Title,
        Duration,
        COUNT
    };

    QPlaylist();
    ~QPlaylist();
    void addFile(const QString& path);
    void addFiles(const QStringList& files);
    void addFolder(const QString& path);
    QStringList toStringList() const;
signals:
    void fileChanged(QString path);
    void fileAdded(QString path, int row);
public slots:
    void clear();
    void next();
    void nextRandom();
    void prev();
private slots:
    void onCellDoubleClicked(int row, int col);
    void onUpdateDuration(qint64 duration, int row);
    void handleContextMenu(const QPoint& pos);
private:
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void updateColumnWidths(int totalTableWidth);
    void initMenu();
    QTableWidgetItem* activeItem = nullptr;
    QThread durationGathererThread;
    DurationGatherer* durationGatherer;
    QMenu* itemRightClickMenu = nullptr;;
    QAction* itemRemoveAction = nullptr;
};

#endif // QPLAYLIST_H
