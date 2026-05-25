#pragma once

#include <KRunner/AbstractRunner>
#include <zim/archive.h>
#include <zim/suggestion.h>

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
    std::optional<zim::Archive> archive_;
    std::optional<zim::SuggestionSearcher> searcher_;
};