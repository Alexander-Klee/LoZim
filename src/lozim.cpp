#include "lozim.h"

#include <iostream>
#include <KNotificationJobUiDelegate>
#include <QIcon>
#include <QImage>
#include <QPixmap>
#include <KIO/OpenUrlJob>
#include <zim/archive.h>
#include <zim/item.h>
#include <zim/suggestion.h>


Lozim::Lozim(QObject *parent, const KPluginMetaData &data)
: AbstractRunner(parent, data)
{
}

void Lozim::init()
{
    // Setup that should be done once comes here
    reloadConfiguration();

    // load the archive
    try {
        archive_.emplace("/home/alex/Dokumente/wiki/wikipedia_en_all_mini_2026-03.zim");
        if (!archive_) return; // TODO is it a good idea to return here?
        // get base addr
        auto source = archive_->getMetadata("Source");
        baseAddr_ = QString::fromStdString(source);
        // if (baseAddr_ == QStringLiteral("en.wikipedia.org")) baseAddr_ = QStringLiteral("en.wikipedia.org/wiki");

        // get icon
        const auto iconBlob = archive_.value().getIllustrationItem().getData();
        const auto bytes = QByteArray::fromRawData(iconBlob.data(), iconBlob.size());
        const auto img = QImage::fromData(bytes);
        if (img.isNull()) return;
        icon_ = QIcon(QPixmap::fromImage(img));
        searcher_.emplace(archive_.value());
    } catch (const std::exception &e) {
        archive_.reset();
        qWarning() << "Failed to open ZIM archive:" << e.what();
        return;
    }

    connect(this, &AbstractRunner::prepare, this, [this]() {
        // Initialize data for the match session. This gets called from the main thread
    });
    connect(this, &AbstractRunner::teardown, this, []() {
        // Cleanup data from the match session. This gets called from the main thread
    });
}

void Lozim::match(KRunner::RunnerContext &context) {
    if (!context.isValid()) return;
    QString query = context.query();
    if (query.startsWith(QLatin1Char('?'))) return;

    const QString triggerWord = QStringLiteral("lz");
    if (!query.startsWith(triggerWord)) return;
    query.remove(0, triggerWord.length());

    if (!this->archive_ || !this->searcher_) return;
    auto searcher = this->searcher_.value();
    auto fallbackIcon = QIcon::fromTheme(QStringLiteral("applications-education-language"));
    auto icon = (this->icon_)? this->icon_.value() : fallbackIcon;

    QList<KRunner::QueryMatch> matches;

    auto search = searcher.suggest(query.toStdString());
    auto results = search.getResults(0, 5);

    auto relevance = 1.0;
    for (auto it = results.begin(); it != results.end(); ++it) {

        KRunner::QueryMatch match(this);
        match.setText(QString::fromStdString(it->getSnippet()));
        match.setSubtext(baseAddr_.value());
        match.setData(QString::fromStdString(it->getPath()));

        match.setCategoryRelevance(KRunner::QueryMatch::CategoryRelevance::Highest); // TODO
        match.setMultiLine(true);
        // match.setActions(actions);
        match.setIcon(icon);
        match.setRelevance(relevance);
        relevance -= 0.01;

        matches.append(match);
    }
    context.addMatches(matches);
}

void Lozim::run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match)
{
    Q_UNUSED(context);
    {
        // open wiki url
        const QString wikiLink =
            QStringLiteral("https://") +
            baseAddr_.value() +
            QStringLiteral("/") + // FIXME missing /wiki/ or /title/
            QString::fromLatin1(QUrl::toPercentEncoding(match.data().toString()));

        auto url = QUrl(wikiLink);
        auto *job = new KIO::OpenUrlJob(url);
        job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoErrorHandlingEnabled));
        job->setRunExecutables(false);
        job->start();
    }

    // if (match.selectedAction() && match.selectedAction().id() == QLatin1String("visit_local")) {
    //     // TODO
    // } else if (match.selectedAction() && match.selectedAction().id() == QLatin1String("visit_online")) {
    //     // TODO
    //     // KIO::OpenUrlJob autodeletes itself, so we can just create it and forget it!
    //     // QString title = QStringLiteral("Alan Turing");
    // }
}

void Lozim::reloadConfiguration()
{
}

K_PLUGIN_CLASS_WITH_JSON(Lozim, "lozim.json")
#include "lozim.moc"