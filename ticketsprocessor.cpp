#include <QJsonValue>
#include <QJsonArray>
#include <QJsonDocument>
#include <QMutexLocker>
#include <QDebug>
#include <QDateTime>
#include "ticketsprocessor.h"
#include "appsettings.h"

using namespace vvf;

const QString TicketsProcessor::ID = "id";
const QString TicketsProcessor::TICKET_ACTION = "action_name";
const QString TicketsProcessor::TICKET_NUMBER = "ticket_number";
const QString TicketsProcessor::CREATED_AT = "created_at";
const QString TicketsProcessor::USER_NAME = "user_name";
const QString TicketsProcessor::WINDOW = "window";
const QString TicketsProcessor::ON_SERVICE = "on_service";
const QString TicketsProcessor::IS_DONE = "is_done";
const QString TicketsProcessor::IS_VOICED = "is_voiced";
const QString TicketsProcessor::IS_MANUAL = "is_manual";

Ticket fromQJsonObject(const QJsonObject& object)
{
    Ticket result;
    result.id = object.value(TicketsProcessor::ID).toInt();
    result.ticket_number = object.value(TicketsProcessor::TICKET_NUMBER).toString();
    result.action = object.value(TicketsProcessor::TICKET_ACTION).toString();
    result.on_service = object.value(TicketsProcessor::ON_SERVICE).toBool();
    result.is_done = object.value(TicketsProcessor::IS_DONE).toBool();
    result.is_voiced = object.value(TicketsProcessor::IS_VOICED).toBool();
    result.is_manual = object.value(TicketsProcessor::IS_MANUAL).toBool();
    result.created_at = object.value(TicketsProcessor::CREATED_AT).toDouble();
    return result;
}

TicketsProcessor::TicketsProcessor(QObject *parent)
    : QObject(parent)
{
    requests_processor_ = new RequestsProcessor(this);
    connect(requests_processor_,
            SIGNAL(ticketUpdated(QByteArray)),
            this,
            SLOT(putTicketRequestFinished(QByteArray)));
    connect(requests_processor_,
            SIGNAL(receivedTickets(QByteArray)),
            this,
            SLOT(getTicketRequestFinished(QByteArray)));
    connect(requests_processor_,
            SIGNAL(requestError(QString)),
            this,
            SLOT(requestFinisedWithError(QString)));
}

void TicketsProcessor::getTickets(const QVector<QString> &actions, SelectModes mode) noexcept
{
    QMutexLocker locker(&m);
    finishCurrentTicket();
    actions_ = actions;
    mode_ = mode;
    requests_processor_->sendGetTicketsRequest(mode);
}

void TicketsProcessor::lockTicket(const Ticket &ticket) noexcept
{
    QMutexLocker locker(&m);
    current_ticket_ = ticket;
    current_ticket_.on_service = true;
    requests_processor_->sendUpdateTicketRequest(current_ticket_);
}

void TicketsProcessor::voiceTicket() noexcept
{
    QMutexLocker locker(&m);
    if (current_ticket_.isValid()) {
        current_ticket_.is_voiced = false;
        requests_processor_->sendUpdateTicketRequest(current_ticket_);
    } else {
        emit ticketError(tr("Нет текущего талона"));
    }
}

bool TicketsProcessor::hasActiveTicket() const noexcept
{
    return current_ticket_.isValid() && current_ticket_.is_done == false;
}

void TicketsProcessor::returnCurrentTicket() noexcept
{
    QMutexLocker locker(&m);
    if (current_ticket_.isValid() && current_ticket_.is_done != true) {
        current_ticket_.on_service = false;
        current_ticket_.is_manual = true;
        requests_processor_->sendUpdateTicketRequest(current_ticket_);
        current_ticket_ = {};
        emit receivedTicket(current_ticket_);
    }
}

void TicketsProcessor::finishCurrentTicket() noexcept
{
    if (current_ticket_.isValid() && current_ticket_.is_done != true) {
        current_ticket_.is_done = true;
        current_ticket_.is_voiced = true;
        requests_processor_->sendUpdateTicketRequest(current_ticket_);
    }
}

bool TicketsProcessor::isValidAction(const QString &action) const noexcept
{
    bool result = false;
    for (auto it = actions_.begin(); it != actions_.end(); it++) {
        if (*it == action) {
            result = true;
            break;
        }
    }
    return result;
}

bool TicketsProcessor::isValidTicket(const QJsonObject &ticket) const noexcept
{
        return ticket.contains(ID) &&
                ticket.contains(TICKET_ACTION) &&
                ticket.contains(CREATED_AT);

}

void TicketsProcessor::getTicketRequestFinished(const QByteArray &data) noexcept
{
    QJsonDocument jsonInput = QJsonDocument::fromJson(data);
    if (!jsonInput.isEmpty() && jsonInput.isArray()) {
        QVector<Ticket> validJsonTickets;
        QJsonArray jsonTicketsArr(jsonInput.array());
        SelectModes mode;
        {
            QMutexLocker locker(&m);
            mode = mode_;
        }
        for (const auto jsonTicketVal : jsonTicketsArr) {
            QJsonObject jsonTicketObj(jsonTicketVal.toObject());
            if (isValidTicket(jsonTicketObj) == true) {
                Ticket ticket = fromQJsonObject(jsonTicketObj);
                if (mode == SelectModes::MANUAL ||
                        (isValidAction(ticket.action) &&
                         !ticket.is_manual)) {
                    validJsonTickets.push_back(std::move(ticket));
                }
            } else {
                emit ticketError(tr("Сервер вернул некорректные данные"));
                return;
            }
        }
        if (validJsonTickets.size() > 0) {
            emit receivedTickets(validJsonTickets);
        } else {
            emit ticketError(tr("Нет необслуженных квитанций"));
            emit receivedTicket({});
        }
    } else {
        emit ticketError(tr("Сервер вернул некорректные данные"));
    }
}

void TicketsProcessor::putTicketRequestFinished(const QByteArray &) noexcept
{
    QMutexLocker locker(&m);
    if (current_ticket_.is_done == false) {
        emit receivedTicket(current_ticket_);
    } else {
        emit receivedTicket({});
    }
}

void TicketsProcessor::requestFinisedWithError(const QString &message) const noexcept
{
    emit ticketError(message);
}
