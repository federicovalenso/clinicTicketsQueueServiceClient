#include <QObject>
#include <QVector>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QMessageBox>
#include <QDebug>
#include "appstate.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace vvf;

void AppState::processTicket(MainWindow* w, const Ticket &ticket)
{
    w->ui->lblTicketNumber->setText(ticket.ticket_number);
}

void AppState::processTickets(MainWindow*, const QVector<Ticket>&)
{}

void AppState::background(MainWindow*)
{}

void AppState::nextTicket(MainWindow* w)
{
    w->changeState(NextTicketState::getInstance());
    w->getTickets();
    w->setButtonsEnabled(false);
    QTimer::singleShot(3000, w, [w](){w->setButtonsEnabled(true);});
}

void AppState::returnTicket(MainWindow* w)
{
    w->changeState(ReturnTicketState::getInstance());
    w->ticketsProcessor->returnCurrentTicket();
}

void AppState::selectTicket(MainWindow* w)
{
    w->changeState(SelectTicketState::getInstance());
    w->getTickets(SelectModes::MANUAL);
}

void AppState::voiceTicket(MainWindow* w)
{
    w->changeState(VoiceTicketState::getInstance());
    w->ticketsProcessor->voiceTicket();
}

void AppState::close(MainWindow* w, QCloseEvent* event)
{
    if (w->ticketsProcessor->hasActiveTicket() == true) {
        event->ignore();
        if (QMessageBox::warning(
                    w,
                    QObject::tr("Есть незавершенный талон!"),
                    QObject::tr("Закончить работу?"),
                    QMessageBox::Yes,
                    QMessageBox::No) == QMessageBox::Yes) {
            w->ticketsProcessor->finishCurrentTicket();
            w->changeState(CloseState::getInstance());
        }
    }
}

void AppState::error(MainWindow* w, const QString& error)
{
    QMessageBox::critical(w,
                          QObject::tr("Ошибка"),
                          error,
                          QMessageBox::Close);
    w->changeState(BackgroundState::getInstance());
}

void AppState::changeState(MainWindow* w, AppState* state)
{
    w->changeState(state);
}

void AppState::lockTicket(MainWindow* w, const Ticket& ticket)
{
    w->ticketsProcessor->lockTicket(ticket);
}

void AppState::getTickets(MainWindow* w, SelectModes mode)
{
    if (w->ticketsProcessor->hasActiveTicket() == false) {
        w->getTickets(mode);
    }
}
