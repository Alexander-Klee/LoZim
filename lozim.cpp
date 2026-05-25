#include "lozim.h"
#include <QIcon>
#include <zim/archive.h>
#include <zim/item.h>
#include <zim/suggestion.h>
#include <iostream>


Lozim::Lozim(QObject *parent, const KPluginMetaData &data)
: AbstractRunner(parent, data)
{
}

void Lozim::init()
{
    reloadConfiguration();

    // load the archive
    try {
        archive_.emplace("/home/alex/Dokumente/wiki/wikipedia_en_all_mini_2026-03.zim");
        if (archive_) searcher_.emplace(archive_.value());
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

    QList<KRunner::QueryMatch> matches;
    // qWarning() << query << m_path;

    auto search = searcher.suggest(query.toStdString());
    auto results = search.getResults(0, 5);

    auto relevance = 1.0;
    for (auto it = results.begin(); it != results.end(); ++it) {

        KRunner::QueryMatch match(this);
        match.setText(QString::fromStdString(it->getSnippet()));
        // match.setSubtext(QString::fromStdString(it->));

        const QIcon icon = QIcon::fromTheme(QStringLiteral("applications-education-language"));
        match.setIcon(icon);
        match.setCategoryRelevance(KRunner::QueryMatch::CategoryRelevance::Highest); // TODO
        // match.setData();
        match.setMultiLine(true);
        match.setRelevance(relevance);
        relevance -= 0.01;

        matches.append(match);
    }
    context.addMatches(matches);
}

void Lozim::run(const KRunner::RunnerContext & /*context*/, const KRunner::QueryMatch &match)
{
}

void Lozim::reloadConfiguration()
{
}

K_PLUGIN_CLASS_WITH_JSON(Lozim, "lozim.json")
#include "lozim.moc"