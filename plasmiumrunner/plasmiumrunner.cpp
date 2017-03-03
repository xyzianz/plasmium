#include "plasmiumrunner.h"


#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusServiceWatcher>
#include <QtDBus/QDBusConnectionInterface>

#include <KLocalizedString>

K_EXPORT_PLASMA_RUNNER(plasmium, PlasmiumRunner)

static const QString s_plasmiumService = QStringLiteral("org.kde.plasmium");
static const QString s_keyword         = QStringLiteral("Plasmium");

PlasmiumRunner::PlasmiumRunner(QObject *parent, const QVariantList &args)
    : AbstractRunner(parent, args),
    m_enabled(true)
{
    setObjectName(s_keyword);
    setIgnoredTypes(Plasma::RunnerContext::NetworkLocation |
                    Plasma::RunnerContext::Executable |
                    Plasma::RunnerContext::FileSystem |
                    Plasma::RunnerContext::Help |
                    Plasma::RunnerContext::ShellCommand);

//    QDBusServiceWatcher *watcher = new QDBusServiceWatcher(s_plasmiumService, QDBusConnection::sessionBus(),
//                                                        QDBusServiceWatcher::WatchForOwnerChange, this);
//    connect(watcher, &QDBusServiceWatcher::serviceOwnerChanged, this, &PlasmiumRunner::checkAvailability);
//    checkAvailability(QString(), QString(), QString());
    
    setSpeed(SlowSpeed);
    setPriority(LowPriority);
    setHasRunOptions(true);
}

PlasmiumRunner::~PlasmiumRunner()
{
}

void PlasmiumRunner::match(Plasma::RunnerContext &context)
{
    if (m_enabled && context.query().compare(s_keyword, Qt::CaseInsensitive) == 0) {
        Plasma::QueryMatch match(this);
        match.setId("plasmium");
        match.setType(Plasma::QueryMatch::ExactMatch);
        match.setIconName(QStringLiteral("chrome"));
        match.setText(i18n("Open Plasmium"));
        match.setRelevance(1.0);
        context.addMatch(match);
    }
}

void PlasmiumRunner::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match)
{
    Q_UNUSED(context)

    // KRun autodeletes itself, so we can just create it and forget it!
//    KRun *opener = new KRun(match.data().toString(), 0);
//    opener->setRunExecutables(false);
}

/*void PlasmiumRunner::checkAvailability(const QString &name, const QString &oldOwner, const QString &newOwner)
{
    Q_UNUSED(oldOwner)

    bool enabled = false;
    if (name.isEmpty()) {
//        enabled = QDBusConnection::sessionBus().interface()->isServiceRegistered(s_kwinService).value();
    } else {
//        enabled = !newOwner.isEmpty();
    }

    if (m_enabled != enabled) {
        m_enabled = enabled;

        if (m_enabled) {
            addSyntax(Plasma::RunnerSyntax(s_keyword, i18n("Opens the Plasmium (Plasma-Chrome/Chromium integration) debug console.")));
        } else {
            setSyntaxes(QList<Plasma::RunnerSyntax>());
        }
    }
    m_enabled = true;
}*/

#include "plasmiumrunner.moc"
