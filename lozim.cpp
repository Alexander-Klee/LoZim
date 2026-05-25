#include "lozim.h"
#include <QIcon>

Lozim::Lozim(QObject *parent, const KPluginMetaData &data)
: AbstractRunner(parent, data)
{
}

void Lozim::init()
{
    reloadConfiguration();
    connect(this, &AbstractRunner::prepare, this, []() {
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

    QList<KRunner::QueryMatch> matches;
    // qWarning() << query << m_path;

    KRunner::QueryMatch match(this);
    match.setText(QStringLiteral("Lozim!!!!"));
    match.setRelevance(1.0);
    match.setData(QStringLiteral("This is Data."));
    // match.setId(path);
    // QIcon icon = QIcon::fromTheme(mimeDb.mimeTypeForFile(path).iconName());
    const QIcon icon = QIcon::fromTheme(QStringLiteral("applications-education-language"));
    match.setIcon(icon);
    match.setCategoryRelevance(KRunner::QueryMatch::CategoryRelevance::Highest); // TODO
    match.setSubtext(QStringLiteral("Subtext visible on highlight"));
    // match.setMultiLine(true);

    matches.append(match);
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