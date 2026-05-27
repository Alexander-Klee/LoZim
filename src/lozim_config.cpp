
#include "lozim_config.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>
#include <QFormLayout>
#include <QLineEdit>
#include <KPluginFactory>

K_PLUGIN_CLASS(LozimConfig)

LozimConfig::LozimConfig(QObject *parent, const KPluginMetaData &metaData)
    : KCModule(parent, metaData)
{
    QFormLayout *layout = new QFormLayout;
    triggerWord = new QLineEdit;
    layout->addRow(i18nc("@label:textbox", "Trigger word:"), triggerWord);
    KCModule::widget()->setLayout(layout);
    connect(triggerWord, &QLineEdit::textChanged, this, &LozimConfig::markAsChanged);
    LozimConfig::load();
}

void LozimConfig::load()
{
    KSharedConfig::Ptr cfg = KSharedConfig::openConfig(QStringLiteral("krunnerrc"));
    KConfigGroup grp = cfg->group(QStringLiteral("Runners"));
    grp = KConfigGroup(&grp, QStringLiteral("lozim"));

    triggerWord->setText(grp.readEntry(CONFIG_TRIGGERWORD, QStringLiteral("lz")));
    KCModule::load();
}

void LozimConfig::save()
{
    KCModule::save();
    KSharedConfig::Ptr cfg = KSharedConfig::openConfig(QLatin1String("krunnerrc"));
    KConfigGroup grp = cfg->group(QStringLiteral("Runners"));
    grp = KConfigGroup(&grp, QStringLiteral("lozim"));

    grp.writeEntry(CONFIG_TRIGGERWORD, triggerWord->text());
    grp.sync();
}

void LozimConfig::defaults()
{
    KCModule::defaults();
    triggerWord->setText(QStringLiteral("lz"));
    KCModule::markAsChanged();
}

#include "lozim_config.moc"