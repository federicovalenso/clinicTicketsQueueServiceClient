#include "appstate.h"
#include "dlgchooseticket.h"
#include "mainwindow.h"

using namespace vvf;

SelectTicketState* SelectTicketState::getInstance() {
  static SelectTicketState instance;
  return &instance;
}

void SelectTicketState::processTicket(MainWindow* w, const Ticket& ticket) {
  if (isSelected) {
    AppState::processTicket(w, ticket);
    isSelected = false;
  }
}

void SelectTicketState::processTickets(MainWindow* w,
                                       const QVector<Ticket>& tickets) {
  Ticket ticket = DlgChooseTicket::getTicket(w, tickets);
  if (ticket.isValid()) {
    isSelected = true;
    lockTicket(w, ticket);
  }
}
