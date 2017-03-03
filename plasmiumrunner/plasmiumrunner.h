#ifndef PLASMIUMRUNNER_H
#define PLASMIUMRUNNER_H

#include <KRunner/AbstractRunner>

class PlasmiumRunner : public Plasma::AbstractRunner
{
    Q_OBJECT

    public:
        explicit PlasmiumRunner(QObject *parent, const QVariantList &args);
        ~PlasmiumRunner();

        void match(Plasma::RunnerContext &context);
        void run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &action);

    private:
        bool m_enabled;
        QString m_path;
        QString m_triggerWord;
};

#endif
