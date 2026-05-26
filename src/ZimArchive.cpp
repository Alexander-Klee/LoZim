//
// Created by alex on 26.05.26.
//

#include "ZimArchive.h"

#include <QDebug>
#include <zim/item.h>

ZimArchive::ZimArchive(std::string filepath) {
    // load the archive
    try {
        archive_.emplace(filepath);

        // get base addr
        source_ = QString::fromStdString(archive_->getMetadata("Source"));
        // if (source_ == QStringLiteral("en.wikipedia.org")) source_ = QStringLiteral("en.wikipedia.org/wiki");

        // get icon
        const auto iconBlob = archive_->getIllustrationItem().getData();
        const auto bytes = QByteArray(iconBlob.data(), iconBlob.size());
        const auto img = QImage::fromData(bytes);
        auto fallbackIcon = QIcon::fromTheme(QStringLiteral("applications-education-language"));
        icon_ = img.isNull()? fallbackIcon : QIcon(QPixmap::fromImage(img));

        // get searcher
        searcher_.emplace(archive_.value());
    } catch (const std::exception &e) {
        archive_.reset();
        searcher_.reset();
        qWarning() << "Failed to open ZIM archive:" << e.what();
    }
}

QString ZimArchive::baseAddress() const {
    // FIXME missing /wiki/ or /title/
    return QStringLiteral("https://") + source_ + QStringLiteral("/");
}

zim::SuggestionResultSet ZimArchive::suggest(const QString &query, int limit) {
    auto search = searcher_->suggest(query.toStdString());
    return search.getResults(0, limit);
}
