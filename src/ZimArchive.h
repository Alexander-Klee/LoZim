//
// Created by alex on 26.05.26.
//

#pragma once

#include <QIcon>
#include <zim/archive.h>
#include <zim/suggestion.h>

class ZimArchive {
public:
    explicit ZimArchive(std::string filepath);

    [[nodiscard]] bool isValid() const { return archive_.has_value(); }
    [[nodiscard]] const QIcon& icon() const { return icon_; }
    [[nodiscard]] const QString& source() const { return source_; }
    [[nodiscard]] QString baseAddress() const;

    [[nodiscard]] zim::SuggestionResultSet suggest(const QString &query, int limit = 5);

private:
    std::optional<zim::Archive> archive_;
    std::optional<zim::SuggestionSearcher> searcher_;
    QIcon icon_;
    QString source_;

};