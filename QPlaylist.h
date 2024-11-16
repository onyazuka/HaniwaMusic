#ifndef QPLAYLIST_H
#define QPLAYLIST_H

#include <QTableWidget>
#include <QHeaderView>
#include <QDir>


class QPlaylist : public QTableWidget {
    Q_OBJECT
public:
    enum Column {
        Title,
        Duration,
        COUNT
    };

    QPlaylist();
    void addFile(const QString& path);
    void addFiles(const QStringList& files);
    void addFolder(const QString& path);
    void clear();
signals:
    void fileChanged(QString path);
private slots:
    void onCellDoubleClicked(int row, int col);
private:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // QPLAYLIST_H
