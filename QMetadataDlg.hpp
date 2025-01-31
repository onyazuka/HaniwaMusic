#ifndef QMETADATADLG_HPP
#define QMETADATADLG_HPP
#include <QDialog>
#include <QLabel>
#include <QGridLayout>

class QMetadataDlg : public QDialog {
    Q_OBJECT
public:
    QMetadataDlg(const QString& path, QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
private:
    void initLayout();
    QLabel* lArtistHeader;
    QLabel* lArtist;
    QLabel* lTitleHeader;
    QLabel* lTitle;
    QLabel* lAlbumHeader;
    QLabel* lAlbum;
    QLabel* lYearHeader;
    QLabel* lYear;
    QLabel* lTrackNumberHeader;
    QLabel* lTrackNumber;
    QLabel* lCommentHeader;
    QLabel* lComment;
    QLabel* lImage;
    QGridLayout* layout;
};

#endif // QMETADATADLG_HPP
