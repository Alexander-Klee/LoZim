
#include "lozim_config.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KSharedConfig>
#include <KUrlRequester>
#include <QDir>
#include <QFormLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QScrollArea>

K_PLUGIN_CLASS(LozimConfig)

LozimConfig::LozimConfig(QObject *parent, const KPluginMetaData &metaData)
    : KCModule(parent, metaData)
{
    QFormLayout *layout = new QFormLayout;

    // triggerWord
    triggerWord = new QLineEdit;
    layout->addRow(i18nc("@label:textbox", "Trigger word:"), triggerWord);
    connect(triggerWord, &QLineEdit::textChanged, this, &LozimConfig::markAsChanged);

    // ZIM Filepath
    zimDirpath = new KUrlRequester(KCModule::widget());
    zimDirpath->setMode(KFile::Directory);
    layout->addRow(i18nc("@label:textbox", "ZIM directory:"), zimDirpath);
    connect(zimDirpath, &KUrlRequester::textChanged, this, [this] {
        markAsChanged();
        rebuildArchiveList();
    });

    // Archive selection
    archivesBox = new QGroupBox(i18nc("@title:group", "Archives"), KCModule::widget());
    archivesLayout = new QVBoxLayout(archivesBox);

    // optional: put it in a scroll area if there are many
    auto *scroll = new QScrollArea(KCModule::widget());
    scroll->setWidgetResizable(true);
    scroll->setWidget(archivesBox);
    layout->addRow(scroll);
    rebuildArchiveList();

    KCModule::widget()->setLayout(layout);
    LozimConfig::load();
}

void LozimConfig::rebuildArchiveList()
{
    // clear old checkboxes
    for (auto *cb : std::as_const(archiveChecks)) {
        cb->deleteLater();
    }
    archiveChecks.clear();

    const QString dirPath = zimDirpath->text();
    QDir dir(dirPath);
    const auto files = dir.entryList({QStringLiteral("*.zim")}, QDir::Files | QDir::NoDotAndDotDot);

    // read current saved enabled states
    KSharedConfig::Ptr cfg = KSharedConfig::openConfig(QStringLiteral("krunnerrc"));
    KConfigGroup grp = cfg->group(QStringLiteral("Runners"));
    grp = KConfigGroup(&grp, QStringLiteral("lozim"));
    const KConfigGroup archivesGrp = grp.group(QString::fromStdString(CONFIG_ARCHIVES_GROUP));

    for (const QString &file : files) {
        auto *cb = new QCheckBox(file, /*parent=*/archivesBox);
        cb->setChecked(archivesGrp.readEntry(file, true));
        archivesLayout->addWidget(cb);

        connect(cb, &QCheckBox::toggled, this, &LozimConfig::markAsChanged);
        archiveChecks.insert(file, cb);
    }
}

void LozimConfig::load()
{
    KSharedConfig::Ptr cfg = KSharedConfig::openConfig(QStringLiteral("krunnerrc"));
    KConfigGroup grp = cfg->group(QStringLiteral("Runners"));
    grp = KConfigGroup(&grp, QStringLiteral("lozim"));

    triggerWord->setText(grp.readEntry(CONFIG_TRIGGERWORD, QStringLiteral("lz")));

    QString defaultDirpath = QDir::homePath() + QStringLiteral("/Documents/wiki/");
    zimDirpath->setText(grp.readPathEntry(CONFIG_ZIM_FILEPATH, defaultDirpath));
    rebuildArchiveList();

    KCModule::load();
}

void LozimConfig::save()
{
    KCModule::save();
    KSharedConfig::Ptr cfg = KSharedConfig::openConfig(QLatin1String("krunnerrc"));
    KConfigGroup grp = cfg->group(QStringLiteral("Runners"));
    grp = KConfigGroup(&grp, QStringLiteral("lozim"));

    grp.writeEntry(CONFIG_TRIGGERWORD, triggerWord->text());
    grp.writePathEntry(CONFIG_ZIM_FILEPATH, zimDirpath->text());

    KConfigGroup archivesGrp = grp.group(QString::fromStdString(CONFIG_ARCHIVES_GROUP));
    for (auto it = archiveChecks.cbegin(); it != archiveChecks.cend(); ++it) {
        archivesGrp.writeEntry(it.key(), it.value()->isChecked());
    }

    grp.sync();
}

void LozimConfig::defaults()
{
    KCModule::defaults();
    triggerWord->setText(QStringLiteral("lz"));
    QString defaultDirpath = QDir::homePath() + QStringLiteral("/Documents/wiki/");
    zimDirpath->setText(defaultDirpath);
    KCModule::markAsChanged();
}

#include "lozim_config.moc"