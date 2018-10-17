#include "appstate.h"
#include <QTime>
#include <QDebug>

using namespace vvf;

BackgroundState *BackgroundState::getInstance()
{
    static BackgroundState instance;
    return &instance;
}

void BackgroundState::processTickets(MainWindow* w, const QVector<Ticket>& tickets)
{
    NextTicketState::getInstance()->processTickets(w, tickets);
}

void BackgroundState::background(MainWindow* w)
{
    getTickets(w);
}

void BackgroundState::error(MainWindow *, const QString &error)
{
    qDebug() << QString("%1 -> %2").arg(QTime::currentTime().toString("hh:mm:ss")).arg(error);
}
