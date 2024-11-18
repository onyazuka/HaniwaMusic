#ifndef QPLAYLIST_H
#define QPLAYLIST_H

#include <QTableWidget>
#include <QHeaderView>
#include <QDir>


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
    void addFile(const QString& path);
    void addFiles(const QStringList& files);
    void addFolder(const QString& path);
    QStringList toStringList() const;
signals:
    void fileChanged(QString path);
public slots:
    void clear();
    void next();
    void prev();
private slots:
    void onCellDoubleClicked(int row, int col);
private:
    void resizeEvent(QResizeEvent *event) override;
    QTableWidgetItem* activeItem = nullptr;
};

#endif // QPLAYLIST_H
