#pragma once

#include <KRunner/AbstractRunner>
#include <KRunner/Action>
#include "ZimArchive.h"


class Lozim : public KRunner::AbstractRunner
{
    Q_OBJECT

public:
    Lozim(QObject *parent, const KPluginMetaData &data);

    void match(KRunner::RunnerContext &context) override;
    void run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match) override;
    void reloadConfiguration() override;

protected:
    void init() override;

private:
    QList<KRunner::Action> actions;
    QList<ZimArchive> archives;
};