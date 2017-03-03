#ifndef PLASMIUM_H
#define PLASMIUM_H

#include <QtCore/QObject>
#include <QSocketNotifier>
#include <QtCore/QStringList>
#include <QtCore/QString>
#include <QFile>
#include <QTextStream>
#include <QDataStream>

class Plasmium: public QObject
{
    Q_OBJECT
    QStringList m_tabs;
    QSocketNotifier *m_notify;
    QFile *m_in, *m_out;
    QDataStream *m_dataStreamIn, *m_dataStreamOut;
    QTextStream *m_textStreamIn, *m_textStreamOut;

public:
    Plasmium();
    ~Plasmium();
    void init();

public slots:
    Q_SCRIPTABLE QString matchTab(const QString &text);
    Q_SCRIPTABLE void switchTab(const QString &text);

private Q_SLOTS:
    void readNativeMessage();
};


#endif // PLASMIUM_H
