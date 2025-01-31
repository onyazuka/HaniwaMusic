#include "QMetadataDlg.hpp"
#include "MetaTagsParser/Tag.hpp"
#include "MetaTagsParser/TagScout.hpp"
#include <QApplication>
#include <QScreen>

using namespace tag;
using namespace tag::user;

QMetadataDlg::QMetadataDlg(const QString& path, QWidget *parent, Qt::WindowFlags flags)
    : QDialog(parent, flags)
{
    static const QList<ImageType> ImageTypePrioriry {
        ImageType::FrontCover,
        ImageType::BackCover,
        ImageType::GeneralFileIcon,
        ImageType::FileIcon32x32PNG,
        ImageType::ArtistLogo,
        ImageType::StudioLogo,
        ImageType::Artist,
        ImageType::LinearNotesPage,
        ImageType::MediaLabel,
        ImageType::Composer,
        ImageType::Conductor,
        ImageType::Band,
        ImageType::Composer,
        ImageType::Lyricist,
        ImageType::RecordingLocation,
        ImageType::DuringRecording,
        ImageType::DuringPerformance,
        ImageType::ScreenCapture,
        ImageType::Illustration,
        ImageType::Other,
        ImageType::BrightColoredFish
    };
    static GetMetaInfoConfig config;
    config.textual = true;
    config.duration = false;
    config.images = true;
    auto metainfo = getMetainfo(path.toStdString(), config);
    lArtistHeader = new QLabel(tr("Artist "));
    lArtist = new QLabel(QString::fromStdString(std::get<std::string>(metainfo["artist"])));
    if (lArtist->text().isEmpty()) {
        lArtist->setText("-");
    }
    lTitleHeader = new QLabel(tr("Title "));
    lTitle = new QLabel(QString::fromStdString(std::get<std::string>(metainfo["title"])));
    if (lTitle->text().isEmpty()) {
        lTitle->setText("-");
    }
    lAlbumHeader = new QLabel(tr("Album "));
    lAlbum = new QLabel(QString::fromStdString(std::get<std::string>(metainfo["album"])));
    if (lAlbum->text().isEmpty()) {
        lAlbum->setText("-");
    }
    lYearHeader = new QLabel(tr("Year "));
    lYear = new QLabel(QString::fromStdString(std::get<std::string>(metainfo["year"])));
    if (lYear->text().isEmpty()) {
        lYear->setText("-");
    }
    lTrackNumberHeader = new QLabel(tr("Track number "));
    lTrackNumber = new QLabel(QString::fromStdString(std::get<std::string>(metainfo["trackNumber"])));
    if (lTrackNumber->text().isEmpty()) {
        lTrackNumber->setText("-");
    }
    lCommentHeader = new QLabel(tr("Comment "));
    lComment = new QLabel(QString::fromStdString(std::get<std::string>(metainfo["comment"])));
    if (lComment->text().isEmpty()) {
        lComment->setText("-");
    }
    lImage = new QLabel();
    auto images = std::get<std::vector<tag::user::APICUserData>>(metainfo["images"]);
    int iImageToShow = -1;
    for (auto priority: ImageTypePrioriry) {
        for (size_t i = 0; i < images.size(); ++i) {
            if (images[i].type == priority) {
                iImageToShow = i;
            }
        }
    }
    if (iImageToShow >= 0) {
        auto& img = images[iImageToShow];
        QImage qimage = QImage::fromData(QByteArray((const char*)img.data.first.get(), img.data.second), img.mimeType.data());
        static const float scaleFactor = 400.0f / 2560.0f;
        float width = QApplication::primaryScreen()->geometry().width();
        qimage = qimage.scaled(QSize(std::min(width * scaleFactor, (float)qimage.width()), std::min(width * scaleFactor, (float)qimage.height())), Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation);
        lImage->setPixmap(QPixmap::fromImage(qimage));
    }
    initLayout();
    setWindowTitle(tr("Metadata"));
    setMinimumSize(200, 100);
}

void QMetadataDlg::initLayout() {
    layout = new QGridLayout(this);
    size_t i = 0;
    layout->addWidget(lArtistHeader, i, 0);
    layout->addWidget(lArtist, i++, 1);
    layout->addWidget(lTitleHeader, i, 0);
    layout->addWidget(lTitle, i++, 1);
    layout->addWidget(lAlbumHeader, i, 0);
    layout->addWidget(lAlbum, i++, 1);
    layout->addWidget(lYearHeader, i, 0);
    layout->addWidget(lYear, i++, 1);
    layout->addWidget(lTrackNumberHeader, i, 0);
    layout->addWidget(lTrackNumber, i++, 1);
    layout->addWidget(lCommentHeader, i, 0);
    layout->addWidget(lComment, i++, 1);
    if (!lImage->pixmap().isNull()) {
        layout->addWidget(lImage, i++, 0, 2, 2, Qt::AlignHCenter | Qt::AlignTop);
    }
    setLayout(layout);
    layout->setColumnMinimumWidth(0, 100);
    layout->setColumnMinimumWidth(1, 100);
    layout->setSizeConstraint(QLayout::SetFixedSize);
}
