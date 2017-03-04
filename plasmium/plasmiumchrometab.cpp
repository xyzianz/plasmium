#include "plasmiumchrometab.h"

PlasmiumChromeTab::PlasmiumChromeTab(const QString &title, const QString &uri, int windowId, int tabId, int tabIndex)
{
    m_title = title;
    m_uri = uri;
    m_windowId = windowId;
    m_tabId = tabId;
    m_tabIndex = tabIndex;
}//PlasmiumChromeTab::PlasmiumChromeTab()

PlasmiumChromeTab::~PlasmiumChromeTab()
{
}//PlasmiumChromeTab::~PlasmiumChromeTab()

int PlasmiumChromeTab::getTabIndex() const
{
    return this->m_tabIndex;
}//PlasmiumChromeTab::getTabIndex()

int PlasmiumChromeTab::getWindowId() const
{
    return this->m_windowId;
}//PlasmiumChromeTab::getWindowId()
