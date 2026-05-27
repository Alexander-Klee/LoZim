//
// Created by alex on 26.05.26.
//

#pragma once

#include <KCModule>
#include <KUrlRequester>
#include <QBoxLayout>
#include <QCheckBox>
#include <QGroupBox>
#include <QLineEdit>

static constexpr char CONFIG_TRIGGERWORD[] = "triggerWord";
static constexpr char CONFIG_ZIM_FILEPATH[] = "zimDirpath";
static constexpr char CONFIG_ARCHIVES_GROUP[] = "Archives";

struct ArchiveRowWidgets {
    QCheckBox *enabled = nullptr;
    QLineEdit *note = nullptr;     // whatever the text input represents
};

class LozimConfig : public KCModule {
    Q_OBJECT

public:
    explicit LozimConfig(QObject *parent,
                         const KPluginMetaData &metaData);

    void rebuildArchiveList();

public Q_SLOTS:
    void save() override;
    void load() override;
    void defaults() override;

private:
    QLineEdit *triggerWord;
    KUrlRequester *zimDirpath;

    QMap<QString, ArchiveRowWidgets> archiveRows; // key: filename
    QGroupBox *archivesBox;
    QVBoxLayout *archivesLayout;
};
