#pragma once

#include <KRunner/AbstractRunner>

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
    QString m_path;
    QString m_triggerWord;
};