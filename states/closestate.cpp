#include "appstate.h"
#include "mainwindow.h"

using namespace vvf;

CloseState *CloseState::getInstance()
{
    static CloseState instance;
    return &instance;
}

void CloseState::processTicket(MainWindow *w, const Ticket &)
{
    w->close();
}
