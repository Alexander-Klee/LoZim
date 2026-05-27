
#include "lozim_config.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KSharedConfig>
#include <KUrlRequester>
#include <QFormLayout>
#include <QLineEdit>

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
    connect(zimDirpath, &KUrlRequester::textChanged, this, &LozimConfig::markAsChanged);

    KCModule::widget()->setLayout(layout);
    LozimConfig::load();
}

void LozimConfig::load()
{
    KSharedConfig::Ptr cfg = KSharedConfig::openConfig(QStringLiteral("krunnerrc"));
    KConfigGroup grp = cfg->group(QStringLiteral("Runners"));
    grp = KConfigGroup(&grp, QStringLiteral("lozim"));

    triggerWord->setText(grp.readEntry(CONFIG_TRIGGERWORD, QStringLiteral("lz")));

    QString defaultDirpath = QDir::homePath() + QStringLiteral("/Dokumente/wiki/");
    zimDirpath->setText(grp.readPathEntry(CONFIG_ZIM_FILEPATH, defaultDirpath));

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
    grp.sync();
}

void LozimConfig::defaults()
{
    KCModule::defaults();
    triggerWord->setText(QStringLiteral("lz"));
    QString defaultDirpath = QDir::homePath() + QStringLiteral("/Dokumente/wiki/");
    triggerWord->setText(defaultDirpath);
    KCModule::markAsChanged();
}

#include "lozim_config.moc"