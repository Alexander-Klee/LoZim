#include "lozim.h"

#include <iostream>
#include <KNotificationJobUiDelegate>
#include <QIcon>
#include <QImage>
#include <QPixmap>
#include <KIO/OpenUrlJob>
#include <zim/archive.h>
#include <zim/item.h>

#include "ZimArchive.h"


Lozim::Lozim(QObject *parent, const KPluginMetaData &data)
: AbstractRunner(parent, data)
{
}

void Lozim::init()
{
    // Setup that should be done once comes here
    reloadConfiguration();

    auto archive = ZimArchive("/home/alex/Dokumente/wiki/wikipedia_en_all_mini_2026-03.zim");
    archives.append(archive);

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

    if (!triggerWord.isEmpty()) {
        if (!query.startsWith(triggerWord)) return;
        query.remove(0, triggerWord.length());
    }

    auto archive = archives[0];

    if (!archive.isValid()) return;

    QList<KRunner::QueryMatch> matches;

    auto results = archive.suggest(query);

    auto relevance = 1.0;
    for (auto it = results.begin(); it != results.end(); ++it) {

        KRunner::QueryMatch match(this);
        match.setText(QString::fromStdString(it->getSnippet()));
        match.setSubtext(archive.source());
        match.setData(QString::fromStdString(it->getPath()));

        match.setCategoryRelevance(KRunner::QueryMatch::CategoryRelevance::Highest); // TODO
        match.setMultiLine(true);
        // match.setActions(actions);
        match.setIcon(archive.icon());
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
        auto archive = archives[0];
        // open wiki url
        const QString wikiLink = archive.baseAddress() +
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
    KConfigGroup c = config();
    triggerWord = c.readEntry(CONFIG_TRIGGERWORD, QStringLiteral("lz"));
    if (!triggerWord.isEmpty()) {
        triggerWord.append(QLatin1Char(' '));
        setTriggerWords({triggerWord});
    } else {
        setMatchRegex(QRegularExpression());
    }
    // TODO
    // setSyntaxes({RunnerSyntax(i18nc("Dictionary keyword", "%1:q:", triggerWord), i18n("Finds the definition of :q:."))});
}

K_PLUGIN_CLASS_WITH_JSON(Lozim, "lozim.json")
#include "lozim.moc"