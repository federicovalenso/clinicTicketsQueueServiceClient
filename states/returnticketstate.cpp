#include "appstate.h"
#include "mainwindow.h"

using namespace vvf;

ReturnTicketState *ReturnTicketState::getInstance()
{
    static ReturnTicketState instance;
    return &instance;
}

void ReturnTicketState::processTicket(MainWindow* w, const Ticket& ticket)
{
    AppState::processTicket(w, ticket);
    changeState(w, BackgroundState::getInstance());
}
