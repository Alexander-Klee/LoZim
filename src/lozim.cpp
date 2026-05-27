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
    reloadConfiguration(); // this also loads archives

    // connect(this, &AbstractRunner::prepare, this, [this]() {
    //     // Initialize data for the match session. This gets called from the main thread
    // });
    // connect(this, &AbstractRunner::teardown, this, []() {
    //     // Cleanup data from the match session. This gets called from the main thread
    // });
}

void Lozim::load_archives() {
    archives.clear();

    const KConfigGroup c = config();
    const KConfigGroup archivesGrp = c.group(QString::fromStdString(CONFIG_ARCHIVES_GROUP));

    const QDir dir(zimDirpath);
    const auto files = dir.entryList({QStringLiteral("*.zim")}, QDir::Files | QDir::NoDotAndDotDot);
    for (const QString &file : files) {

        const bool enabled = archivesGrp.readEntry(file + QStringLiteral("/enabled"), true);
        if (!enabled) continue;

        const QString path = dir.absoluteFilePath(file);

        auto archive = ZimArchive(dir, file);
        if (archive.isValid()) {
            archives.append(std::move(archive));
        } else {
            qWarning() << Q_FUNC_INFO << "Unable to use zim archive: " << path;
        }
    }
}

void Lozim::match(KRunner::RunnerContext &context) {
    if (!context.isValid()) return;
    QString query = context.query();
    if (query.startsWith(QLatin1Char('?'))) return;

    if (!triggerWord.isEmpty()) {
        if (!query.startsWith(triggerWord)) return;
        query.remove(0, triggerWord.length());
    }

    const KConfigGroup c = config();
    const KConfigGroup archivesGrp = c.group(QString::fromStdString(CONFIG_ARCHIVES_GROUP));

    QList<KRunner::QueryMatch> matches;

    for (auto archive = archives.begin(); archive != archives.end(); ++archive) {
        if (!archive->isValid()) continue;
        auto results = archive->suggest(query);

        auto link = archivesGrp.readEntry(archive->filename() + QStringLiteral("/link"), "wiki/");
        auto relevance = 1.0;
        for (auto it = results.begin(); it != results.end(); ++it) {
            // auto entry = it.getEntry();
            // auto item = entry.getItem(true);

            KRunner::QueryMatch match(this);
            match.setText(QString::fromStdString(it->getSnippet()));
            match.setSubtext(archive->source());

            // add link to online page for later retrieval
            const QString wikiLink = archive->baseAddress() + link +
                QString::fromLatin1(QUrl::toPercentEncoding(QString::fromStdString(it->getPath())));
            match.setData(wikiLink);
            // match.setActions(actions);

            match.setCategoryRelevance(KRunner::QueryMatch::CategoryRelevance::Highest); // TODO
            match.setRelevance(relevance);
            match.setMultiLine(true);
            match.setIcon(archive->icon());
            relevance -= 0.01;

            matches.append(match);
        }
    }
    context.addMatches(matches);
}

void Lozim::run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match)
{
    Q_UNUSED(context);
    {
        // get wiki url
        const QString wikiLink = match.data().toString();

        // open url in br
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
    const KConfigGroup c = config();

    // config for triggerWord
    triggerWord = c.readEntry(CONFIG_TRIGGERWORD, QStringLiteral("lz"));
    if (!triggerWord.isEmpty()) {
        triggerWord.append(QLatin1Char(' '));
        setTriggerWords({triggerWord});
    } else {
        setMatchRegex(QRegularExpression());
    }

    // config for the zim filepath
    QString defaultDirpath = QDir::homePath() + QStringLiteral("/Documents/wiki/");
    zimDirpath = c.readPathEntry(CONFIG_ZIM_FILEPATH, defaultDirpath);
    QFileInfo pathInfo(zimDirpath);
    if (!pathInfo.isDir()) zimDirpath = defaultDirpath;
    load_archives();

    // advertise help in KRunner '?lozim'
    // TODO also show help when asking about the trigger word: "?lz"
    QList<KRunner::RunnerSyntax> syntaxes;
    KRunner::RunnerSyntax syntax(QStringLiteral("%1:q:").arg(triggerWord), i18n("Finds matching zim file entries for :q:"));
    syntaxes.append(syntax);
    setSyntaxes(syntaxes);
}

K_PLUGIN_CLASS_WITH_JSON(Lozim, "lozim.json")
#include "lozim.moc"