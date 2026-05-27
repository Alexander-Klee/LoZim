//
// Created by alex on 26.05.26.
//

#pragma once

#include <KCModule>
#include <KPluginMetaData>
#include <QLineEdit>

static constexpr char CONFIG_TRIGGERWORD[] = "triggerWord";

class LozimConfig : public KCModule {
    Q_OBJECT

public:
    explicit LozimConfig(QObject *parent,
                         const KPluginMetaData &metaData);

public Q_SLOTS:
    void save() override;
    void load() override;
    void defaults() override;

private:
    QLineEdit *triggerWord;
};
