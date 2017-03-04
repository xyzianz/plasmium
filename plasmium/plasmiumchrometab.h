#ifndef PLASMIUMCHROMETAB_H
#define PLASMIUMCHROMETAB_H

#include <QObject>
#include <QString>

class PlasmiumChromeTab: public QObject
{
    Q_OBJECT
    QString m_title, m_uri;
    int m_windowId, m_tabId, m_tabIndex;

public:
    PlasmiumChromeTab(const QString &title, const QString &uri, int windowId, int tabId, int tabIndex);
    ~PlasmiumChromeTab();
    int getTabIndex() const;
    int getWindowId() const;

};


#endif // PLASMIUMCHROMETAB_H
