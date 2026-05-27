#include "lozim.h"

#include <iostream>
#include <KConfigGroup>
#include <klocalizedstring.h>
#include <KNotificationJobUiDelegate>
#include <QImage>
#include <KIO/OpenUrlJob>
#include <zim/archive.h>
#include <zim/item.h>

#include "lozim_config.h"
#include "ZimArchive.h"


Lozim::Lozim(QObject *parent, const KPluginMetaData &data)
: AbstractRunner(parent, data)
{
    // KRunner::Action visit_local(QStringLiteral("visit_local"),
    //                      QStringLiteral("edit-copy"),
    //                      QStringLiteral("Visit Local Site"));
    // KRunner::Action visit_online(QStringLiteral("visit_online"),
    //                      QStringLiteral("media-playback-start"),
    //                      QStringLiteral("Visit Online Site"));
    // actions = {visit_local, visit_online};
}

void Lozim::init()
{
    // Setup that should be done once comes here
    reloadConfiguration();

    auto archive = ZimArchive("/home/alex/Dokumente/wiki/wikipedia_en_all_mini_2026-03.zim");
    archives.append(archive);

    qWarning() << triggerWord;

    // connect(this, &AbstractRunner::prepare, this, [this]() {
    //     // Initialize data for the match session. This gets called from the main thread
    // });
    // connect(this, &AbstractRunner::teardown, this, []() {
    //     // Cleanup data from the match session. This gets called from the main thread
    // });
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
        // auto entry = it.getEntry();
        // auto item = entry.getItem(true);

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
        // open wiki url
        auto archive = archives[0];
        const QString wikiLink = archive.baseAddress() +
            QString::fromLatin1(QUrl::toPercentEncoding(match.data().toString()));

        auto url = QUrl(wikiLink);
        // KIO::OpenUrlJob autodeletes itself, so we can just create it and forget it!
        auto *job = new KIO::OpenUrlJob(url);
        job->setUiDelegate(new KNotificationJobUiDelegate(KJobUiDelegate::AutoErrorHandlingEnabled));
        job->setRunExecutables(false);
        job->start();
    }

    // if (match.selectedAction() && match.selectedAction().id() == QLatin1String("visit_local")) {
    //     // TODO
    // } else if (match.selectedAction() && match.selectedAction().id() == QLatin1String("visit_online")) {
    //     // TODO
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

    // TODO also show help when asking about the trigger word: "?lz"
    QList<KRunner::RunnerSyntax> syntaxes;
    KRunner::RunnerSyntax syntax(QStringLiteral("%1:q:").arg(triggerWord), i18n("Finds matching zim file entries for :q:"));
    syntaxes.append(syntax);
    setSyntaxes(syntaxes);
}

K_PLUGIN_CLASS_WITH_JSON(Lozim, "lozim.json")
#include "lozim.moc"