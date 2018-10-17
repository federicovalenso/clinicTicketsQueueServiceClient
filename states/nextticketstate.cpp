#include "appstate.h"
#include "mainwindow.h"

using namespace vvf;

NextTicketState *NextTicketState::getInstance()
{
    static NextTicketState instance;
    return &instance;
}

void NextTicketState::processTickets(MainWindow *w, const QVector<Ticket>& tickets)
{
    auto oldestTicket = std::min_element(tickets.begin(), tickets.end());
    lockTicket(w, *oldestTicket);
}
