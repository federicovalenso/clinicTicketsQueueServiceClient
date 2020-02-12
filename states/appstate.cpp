#include "appstate.h"
#include <QDebug>
#include <QMessageBox>
#include <QObject>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QVector>
#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace vvf;

void AppState::processTicket(MainWindow* w, const Ticket& ticket) {
  w->ui_->lblTicketNumber->setText(ticket.ticket_number);
}

void AppState::processTickets(MainWindow*, const QVector<Ticket>&) {}

void AppState::background(MainWindow*) {}

void AppState::nextTicket(MainWindow* w) {
  w->changeState(NextTicketState::getInstance());
  w->getTickets();
  w->setButtonsEnabled(false);
  QTimer::singleShot(3000, w, [w]() { w->setButtonsEnabled(true); });
}

void AppState::returnTicket(MainWindow* w) {
  w->changeState(ReturnTicketState::getInstance());
  w->tickets_processor_->returnCurrentTicket();
}

void AppState::selectTicket(MainWindow* w) {
  w->changeState(SelectTicketState::getInstance());
  w->getTickets(SelectModes::MANUAL);
}

void AppState::voiceTicket(MainWindow* w) {
  w->changeState(VoiceTicketState::getInstance());
  w->tickets_processor_->voiceTicket();
}

void AppState::close(MainWindow* w, QCloseEvent* event) {
  if (w->tickets_processor_->hasActiveTicket()) {
    event->ignore();
    if (QMessageBox::warning(w, QObject::tr("Есть незавершенный талон!"),
                             QObject::tr("Закончить работу?"), QMessageBox::Yes,
                             QMessageBox::No) == QMessageBox::Yes) {
      w->tickets_processor_->finishCurrentTicket();
      w->changeState(CloseState::getInstance());
    }
  }
}

void AppState::error(MainWindow* w, const QString& error) {
  QMessageBox::critical(w, QObject::tr("Ошибка"), error, QMessageBox::Close);
}

void AppState::changeState(MainWindow* w, AppState* state) {
  w->changeState(state);
}

void AppState::lockTicket(MainWindow* w, const Ticket& ticket) {
  w->tickets_processor_->lockTicket(ticket.id);
}

void AppState::getTickets(MainWindow* w, SelectModes mode) {
  if (!w->tickets_processor_->hasActiveTicket()) {
    w->getTickets(mode);
  }
}
